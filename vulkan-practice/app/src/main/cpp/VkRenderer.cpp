// MIT License
//
// Copyright (c) 2024 Daemyung Jang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cassert>
#include <array>
#include <vector>

#include "VkRenderer.h"
#include "VkUtil.h"
#include "AndroidOut.h"

using namespace std;

VkRenderer::VkRenderer() {
    // ================================================================================
    // 1. VkInstance 생성
    // ================================================================================
    // VkApplicationInfo 구조체 정의
    VkApplicationInfo applicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Practice Vulkan",
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0)
    };

    // 사용할 수 있는 레이어를 얻어온다.
    uint32_t instanceLayerCount;
    VK_CHECK_ERROR(vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr));

    vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
    VK_CHECK_ERROR(vkEnumerateInstanceLayerProperties(&instanceLayerCount,
                                                      instanceLayerProperties.data()));

    // 활성화할 레이어의 이름을 배열로 만든다.
    vector<const char*> instanceLayerNames;
    for (const auto &layerProperty : instanceLayerProperties) {
        instanceLayerNames.push_back(layerProperty.layerName);
    }

    uint32_t instanceExtensionCount; // 사용 가능한 InstanceExtension 개수
    VK_CHECK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr,
                                                          &instanceExtensionCount,
                                                          nullptr));

    vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionCount);
    VK_CHECK_ERROR(vkEnumerateInstanceExtensionProperties(nullptr,
                                                          &instanceExtensionCount,
                                                          instanceExtensionProperties.data()));

    vector<const char *> instanceExtensionNames; // instanceExtensionName을 담는 배열
    for (const auto &properties: instanceExtensionProperties) {
        if (properties.extensionName == string("VK_KHR_surface") ||
            properties.extensionName == string("VK_KHR_android_surface")) {
            instanceExtensionNames.push_back(properties.extensionName);
        }
    }
    assert(instanceExtensionNames.size() == 2); // 반드시 2개의 이름이 필요하기 때문에 확인

    // sType: 구조체의 타입, pApplicationInfo: 어플리케이션의 이름
    // enabledLayerCount, ppEnableLayerNames: 사용할 레이어의 정보를 정의
    VkInstanceCreateInfo instanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32_t>(instanceLayerNames.size()),
        .ppEnabledLayerNames = instanceLayerNames.data(),
        .enabledExtensionCount = static_cast<uint32_t>(instanceExtensionNames.size()),
        .ppEnabledExtensionNames = instanceExtensionNames.data()
    };

    // vkCreateInstance로 인스턴스 생성. 생성된 인스턴스가 mInstance에 쓰여진다.
    VK_CHECK_ERROR(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance));


    // ================================================================================
    // 2. VkPhysicalDevice 선택
    // ================================================================================
    uint32_t physicalDeviceCount;
    VK_CHECK_ERROR(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr));

    vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    VK_CHECK_ERROR(vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, physicalDevices.data()));

    // 간단한 예제를 위해 첫 번째 VkPhysicalDevice를 사용
    mPhysicalDevice = physicalDevices[0];

    VkPhysicalDeviceProperties physicalDeviceProperties; // 이 구조체 안에 GPU에 필요한 모든 정보가 있다.
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);

    aout << "Selected Physical Device Information ↓" << endl;
    aout << setw(16) << left << " - Device Name: "
         << string_view(physicalDeviceProperties.deviceName) << endl;
    aout << setw(16) << left << " - Device Type: "
         << vkToString(physicalDeviceProperties.deviceType) << endl;
    aout << std::hex;
    aout << setw(16) << left << " - Device ID: " << physicalDeviceProperties.deviceID << endl;
    aout << setw(16) << left << " - Vendor ID: " << physicalDeviceProperties.vendorID << endl;
    aout << std::dec;
    aout << setw(16) << left << " - API Version: "
         << VK_API_VERSION_MAJOR(physicalDeviceProperties.apiVersion) << "."
         << VK_API_VERSION_MINOR(physicalDeviceProperties.apiVersion);
    aout << setw(16) << left << " - Driver Version: "
         << VK_API_VERSION_MAJOR(physicalDeviceProperties.driverVersion) << "."
         << VK_API_VERSION_MINOR(physicalDeviceProperties.driverVersion);


    // ================================================================================
    // 3. VkDevice 생성
    // ================================================================================
    uint32_t queueFamilyPropertiesCount;

    //---------------------------------------------------------------------------------
    //** queueFamily 속성을 조회
    // 사용 가능한 queueFamily의 수(=queueFamilyPropertiesCount)를 얻어온다.
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyPropertiesCount, nullptr);

    vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
    // 해당 queueFamily들의 속성을 배열에 얻어온다.
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());
    //---------------------------------------------------------------------------------

    // 특정 queueFamilyProperties가 VK_QUEUE_GRAPHICS_BIT를 지원하는지 확인.
    // 지원하는 queueFamilyProperties를 찾으면 break. queueFamily에 대한 정보는 mQueueFamilyIndex에 저장.
    for (mQueueFamilyIndex = 0;
         mQueueFamilyIndex != queueFamilyPropertiesCount; ++mQueueFamilyIndex) {
        if (queueFamilyProperties[mQueueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            break;
        }
    }

    // 생성할 큐를 정의
    const vector<float> queuePriorities{1.0};
    VkDeviceQueueCreateInfo deviceQueueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = mQueueFamilyIndex,      // queueFamilyIndex
            .queueCount = 1,                            // 생성할 큐의 개수
            .pQueuePriorities = queuePriorities.data()  // 큐의 우선순위
    };

    uint32_t deviceExtensionCount; // 사용 가능한 deviceExtension 개수
    VK_CHECK_ERROR(vkEnumerateDeviceExtensionProperties(mPhysicalDevice,
                                                        nullptr,
                                                        &deviceExtensionCount,
                                                        nullptr));

    vector<VkExtensionProperties> deviceExtensionProperties(deviceExtensionCount);
    VK_CHECK_ERROR(vkEnumerateDeviceExtensionProperties(mPhysicalDevice,
                                                        nullptr,
                                                        &deviceExtensionCount,
                                                        deviceExtensionProperties.data()));

    vector<const char *> deviceExtensionNames;
    for (const auto &properties: deviceExtensionProperties) {
        if (properties.extensionName == string("VK_KHR_swapchain")) {
            deviceExtensionNames.push_back(properties.extensionName);
        }
    }
    assert(deviceExtensionNames.size() == 1); // VK_KHR_swapchain이 반드시 필요하기 때문에 확인

    // 생성할 Device 정의
    VkDeviceCreateInfo deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,                   // 큐의 개수
            .pQueueCreateInfos = &deviceQueueCreateInfo, // 생성할 큐의 정보
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.size()),
            .ppEnabledExtensionNames = deviceExtensionNames.data() // 활성화하려는 deviceExtension들을 넘겨줌
    };

    // vkCreateDevice를 호출하여 Device 생성(= mDevice 생성)
    VK_CHECK_ERROR(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice));
    // 생성된 Device(= mDevice)로부터 큐를 vkGetDeviceQueue를 호출하여 얻어온다.
    vkGetDeviceQueue(mDevice, mQueueFamilyIndex, 0, &mQueue);


    // ================================================================================
    // 4. VkSurface 생성
    // ================================================================================
    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .window = window
    };

    // surface 생성.
    VK_CHECK_ERROR(vkCreateAndroidSurfaceKHR(mInstance, &surfaceCreateInfo, nullptr, &mSurface));

    VkBool32 supported; // surface 지원 여부
    VK_CHECK_ERROR(vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice,
                                                        mQueueFamilyIndex,
                                                        mSurface,
                                                        &supported)); // 지원 여부를 받아옴.
    assert(supported);


    // ================================================================================
    // 5. VkSwapchain 생성
    // ================================================================================
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    VK_CHECK_ERROR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice,
                                                             mSurface,
                                                             &surfaceCapabilities));

    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR;
    for (auto i = 0; i <= 4; ++i) {
        if (auto flag = 0x1u << i; surfaceCapabilities.supportedCompositeAlpha & flag) {
            compositeAlpha = static_cast<VkCompositeAlphaFlagBitsKHR>(flag);
            break;
        }
    }
    assert(compositeAlpha != VK_COMPOSITE_ALPHA_FLAG_BITS_MAX_ENUM_KHR);

    VkImageUsageFlags imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    assert(surfaceCapabilities.supportedUsageFlags & imageUsage);

    uint32_t surfaceFormatCount = 0;
    VK_CHECK_ERROR(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice,
                                                        mSurface,
                                                        &surfaceFormatCount,
                                                        nullptr));

    vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    VK_CHECK_ERROR(vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice,
                                                        mSurface,
                                                        &surfaceFormatCount,
                                                        surfaceFormats.data()));

    uint32_t surfaceFormatIndex = VK_FORMAT_MAX_ENUM;
    for (auto i = 0; i != surfaceFormatCount; ++i) {
        if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM) {
            surfaceFormatIndex = i;
            break;
        }
    }
    assert(surfaceFormatIndex != VK_FORMAT_MAX_ENUM);

    uint32_t presentModeCount;
    VK_CHECK_ERROR(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice,
                                                             mSurface,
                                                             &presentModeCount,
                                                             nullptr));

    vector<VkPresentModeKHR> presentModes(presentModeCount);
    VK_CHECK_ERROR(vkGetPhysicalDeviceSurfacePresentModesKHR(mPhysicalDevice,
                                                             mSurface,
                                                             &presentModeCount,
                                                             presentModes.data()));

    uint32_t presentModeIndex = VK_PRESENT_MODE_MAX_ENUM_KHR;
    for (auto i = 0; i != presentModeCount; ++i) {
        if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR) {
            presentModeIndex = i;
            break;
        }
    }
    assert(presentModeIndex != VK_PRESENT_MODE_MAX_ENUM_KHR);

    VkSwapchainCreateInfoKHR swapchainCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = mSurface,
            .minImageCount = surfaceCapabilities.minImageCount,
            .imageFormat = surfaceFormats[surfaceFormatIndex].format,
            .imageColorSpace = surfaceFormats[surfaceFormatIndex].colorSpace,
            .imageExtent = surfaceCapabilities.currentExtent,
            .imageArrayLayers = 1,
            .imageUsage = imageUsage,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = surfaceCapabilities.currentTransform,
            .compositeAlpha = compositeAlpha,
            .presentMode = presentModes[presentModeIndex]
    };

    VK_CHECK_ERROR(vkCreateSwapchainKHR(mDevice, &swapchainCreateInfo, nullptr, &mSwapchain));

    uint32_t swapchainImageCount;
    VK_CHECK_ERROR(vkGetSwapchainImagesKHR(mDevice, mSwapchain, &swapchainImageCount, nullptr));

    mSwapchainImages.resize(swapchainImageCount);
    VK_CHECK_ERROR(vkGetSwapchainImagesKHR(mDevice,
                                           mSwapchain,
                                           &swapchainImageCount,
                                           mSwapchainImages.data()));

    // ================================================================================
    // 6. VkCommandPool 생성
    // ================================================================================
    VkCommandPoolCreateInfo commandPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // command buffer를 개별적으로 초기화 가능하게 설정
            .queueFamilyIndex = mQueueFamilyIndex
    };

    VK_CHECK_ERROR(vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &mCommandPool)); // mCommandPool 생성

    // ================================================================================
    // 6. VkCommandBuffer 할당
    // ================================================================================
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{ // 할당하려는 command buffer 정의
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = mCommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
    };

    VK_CHECK_ERROR(vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &mCommandBuffer));

    // ================================================================================
    // 7. VkCommandBuffer 기록 시작
    // ================================================================================
    VkCommandBufferBeginInfo commandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT // 한 번만 기록되고 다시 리셋 될 것이라는 의미
    };

    // mCommandBuffer를 기록중인 상태로 변경.
    VK_CHECK_ERROR(vkBeginCommandBuffer(mCommandBuffer, &commandBufferBeginInfo));

    for (auto swapchainImage : mSwapchainImages) { // 스왑체인 이미지만큼 for문을 돈다.
        // ================================================================================
        // 8. VkImage 색상 초기화
        // ================================================================================
        VkClearColorValue clearColorValue{
                .float32 = {0.6431, 0.7765, 0.2235, 1.0} // 초록색으로 clearColor 지정.
        };

        VkImageSubresourceRange imageSubresourceRange { // clear할 영역 지정.
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
        };

        // 해당 swapchainImage를 초록색으로 clear해준다.
        vkCmdClearColorImage(mCommandBuffer,
                             swapchainImage,
                             VK_IMAGE_LAYOUT_UNDEFINED,
                             &clearColorValue,
                             1,
                             &imageSubresourceRange);
    }

    // ================================================================================
    // 9. VkCommandBuffer 기록 종료
    // ================================================================================
    VK_CHECK_ERROR(vkEndCommandBuffer(mCommandBuffer)); // mCommandBuffer는 Executable 상태가 된다.

    // ================================================================================
    // 10. VkCommandBuffer 제출
    // ================================================================================
    VkSubmitInfo submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &mCommandBuffer
    };

    // submitInfo 구조체를 넘김으로써 commandBuffer 정보를 queue에 제출
    VK_CHECK_ERROR(vkQueueSubmit(mQueue, 1, &submitInfo, VK_NULL_HANDLE));
    // commandBuffer를 vkQueueSubmit에 제출했지만 해당 Command buffer가 실행이 됐을지 안 됐을지 알 수 없다. CPU와 GPU는 따로따로 돌기 때문에 항상 실행이 됐다는 보장을 할 수 없다. 그래서 이를 보장하기 위해 vkQueueWaitIdle를 호출하여 이 queue에 제출한 Command buffer가 모두 다 실행되는 것을 보장한다.
    VK_CHECK_ERROR(vkQueueWaitIdle(mQueue));
}

VkRenderer::~VkRenderer() {
    vkFreeCommandBuffers(mDevice, mCommandPool, 1, &mCommandBuffer);
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyDevice(mDevice, nullptr); // Device 파괴. queue의 경우 Device를 생성하면서 생겼기 때문에 따로 파괴하는 API가 존재하지 않는다.
    vkDestroyInstance(mInstance, nullptr);
}

void VkRenderer::render() {
    // ================================================================================
    // 1. 화면에 출력할 수 있는 VkImage 얻기
    // ================================================================================
    uint32_t swapchainImageIndex;
    VK_CHECK_ERROR(vkAcquireNextImageKHR(mDevice,
                                         mSwapchain,
                                         UINT64_MAX,
                                         VK_NULL_HANDLE,
                                         VK_NULL_HANDLE,
                                         &swapchainImageIndex)); // 사용 가능한 이미지 변수에 담기

    // ================================================================================
    // 2. VkImage 화면에 출력
    // ================================================================================
    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .swapchainCount = 1,
            .pSwapchains = &mSwapchain,
            .pImageIndices = &swapchainImageIndex
    };

    VK_CHECK_ERROR(vkQueuePresentKHR(mQueue, &presentInfo)); // 화면에 출력.
    VK_CHECK_ERROR(vkQueueWaitIdle(mQueue));
}

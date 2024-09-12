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

#ifndef PRACTICE_VULKAN_VKUTIL_H
#define PRACTICE_VULKAN_VKUTIL_H

#include <string_view>
#include <string>
#include <vector>
#include <random>
#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "AndroidOut.h"

#ifndef NDEBUG

inline std::string vkToString(VkResult vkResult) {
    switch (vkResult) {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:
            return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:
            return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED:
            return "VK_PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_KHR:
            return "VK_ERROR_NOT_PERMITTED_KHR";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR:
            return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR:
            return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR:
            return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR:
            return "VK_OPERATION_NOT_DEFERRED_KHR";
        default:
            return "Unhandled VkResult";
    }
}

#define VK_CHECK_ERROR(vkFunction)                                                     \
    do {                                                                               \
        if (auto vkResult = vkFunction; vkResult != VK_SUCCESS) {                      \
            aout << #vkFunction << " returns " << vkToString(vkResult) << "." << endl; \
            abort();                                                                   \
        }                                                                              \
    } while (0)
#else
#define VK_CHECK_ERROR(vkFunction)                                                     \
    do {                                                                               \
        vkFunction                                                                     \
    } while (0)
#endif

inline std::string_view vkToString(VkPhysicalDeviceType physicalDeviceType) {
    switch (physicalDeviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "Other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        default:
            return "Unknown";
    }
}

typedef enum VkShaderType {
    VK_SHADER_TYPE_VERTEX = shaderc_vertex_shader,
    VK_SHADER_TYPE_FRAGMENT = shaderc_fragment_shader
} VkShaderType;

inline VkResult
vkCompileShader(std::string_view shaderCode, VkShaderType shaderType,
                std::vector<uint32_t> *shaderBinary) {
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution distribution('a', 'z');
    std::string tag('a', 4);

    for (auto i = 0; i < 16; ++i) {
        tag[i] = static_cast<char>(distribution(generator));
    }

    shaderc::Compiler compiler;
    auto result = compiler.CompileGlslToSpv(shaderCode.data(),
                                            shaderCode.size(),
                                            static_cast<shaderc_shader_kind>(shaderType),
                                            tag.c_str());

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        aout << result.GetErrorMessage() << std::endl;
        return VK_ERROR_UNKNOWN;
    }

    *shaderBinary = std::vector(result.cbegin(), result.cend());
    return VK_SUCCESS;
}

inline VkResult
vkGetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties &physicalDeviceMemoryProperties,
                     const VkMemoryRequirements &memoryRequirements,
                     VkMemoryPropertyFlags memoryPropertyFlags,
                     uint32_t *memoryTypeIndex) {
    for (auto i = 0; i != physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (memoryRequirements.memoryTypeBits & (1 << i)) {
            if ((physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
                 memoryPropertyFlags) == memoryPropertyFlags) {
                *memoryTypeIndex = i;
                return VK_SUCCESS;
            }
        }
    }

    *memoryTypeIndex = UINT32_MAX;
    return VK_ERROR_UNKNOWN;
}

#endif //PRACTICE_VULKAN_VKUTIL_H

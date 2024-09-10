# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)

project("practicevulkan")

####################################################################################################
# shaderc 정의
####################################################################################################
set(CMAKE_SHADERRC ${CMAKE_ANDROID_NDK}/sources/third_party/shaderc)
add_library(shaderc INTERFACE)

target_include_directories(shaderc INTERFACE
        ${CMAKE_SHADERRC}/include)

target_link_libraries(shaderc INTERFACE
        ${CMAKE_SHADERRC}/libs/system/${CMAKE_ANDROID_ARCH_ABI}/libshaderc.a)

####################################################################################################
# practicevulkan 정의
####################################################################################################
find_package(game-activity REQUIRED CONFIG)
find_package(Vulkan REQUIRED)

add_library(practicevulkan SHARED
        VkRenderer.h
        VkRenderer.cpp
        VkUtil.h
        main.cpp
        AndroidOut.cpp)

target_compile_definitions(practicevulkan PRIVATE
        VK_USE_PLATFORM_ANDROID_KHR)

target_link_libraries(practicevulkan
        game-activity::game-activity
        android
        log
        Vulkan::Vulkan
        shaderc)

####################################################################################################
# shaderctest 정의
####################################################################################################
find_package(googletest REQUIRED CONFIG)
find_package(junit-gtest REQUIRED CONFIG)

add_library(shaderctest SHARED
        ShadercTest.cpp)

target_link_libraries(shaderctest PRIVATE
        googletest::gtest
        junit-gtest::junit-gtest
        shaderc)
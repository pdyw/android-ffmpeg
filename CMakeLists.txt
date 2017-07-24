#version
cmake_minimum_required(VERSION 3.6)

#lib include
set(CMAKE_VERBOSE_MAKEFILE on)
set(libs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

#so
add_library(avcodec-57 SHARED IMPORTED )
set_target_properties(avcodec-57 PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavcodec-57.so")

add_library(avfilter-6 SHARED IMPORTED )
set_target_properties(avfilter-6 PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavfilter-6.so")

add_library(avformat-57 SHARED IMPORTED )
set_target_properties(avformat-57 PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavformat-57.so")

add_library(avutil-55  SHARED IMPORTED )
set_target_properties(avutil-55 PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libavutil-55.so")

add_library(swresample-2 SHARED IMPORTED )
set_target_properties(swresample-2 PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libswresample-2.so")

add_library(swscale-4 SHARED IMPORTED )
set_target_properties(swscale-4 PROPERTIES
    IMPORTED_LOCATION "${libs}/${ANDROID_ABI}/libswscale-4.so")

#evn
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fexceptions -frtti")


#self
add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
              src/main/cpp/native-lib_old.cpp
              src/main/cpp/player.cpp
              src/main/cpp/player-jni.cpp
            )



find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log)

find_library(
            android-lib
            android
)

target_link_libraries(
    native-lib
    avcodec-57
    avfilter-6
    avformat-57
    avutil-55
    swresample-2
    swscale-4

    ${log-lib}
    ${android-lib}


    )

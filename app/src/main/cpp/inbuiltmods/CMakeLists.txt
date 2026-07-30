cmake_minimum_required(VERSION 3.22)
project(inbuiltmods LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(inbuiltmods SHARED
    src/common/transition.cpp
    src/fps/fps.cpp
    src/snaplook/snaplook.cpp
    src/gyro/gyro.cpp

    # Modul Zoom Engine (Hanya file backend & JNI Bridge)
    src/zoom/zoom_jni.cpp
    src/zoom/Core/Config.cpp
    src/zoom/CameraHook/CameraHook.cpp
    src/zoom/ZoomController/ZoomController.cpp
)

target_include_directories(inbuiltmods PRIVATE 
    src
    src/zoom
    ${CMAKE_CURRENT_SOURCE_DIR}/../preloader/src
)

find_library(log-lib log)
find_library(egl-lib EGL)

target_link_libraries(inbuiltmods
    PRIVATE
    preloader
    ${log-lib}
    ${egl-lib}
)

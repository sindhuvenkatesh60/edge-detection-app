cmake_minimum_required(VERSION 3.18.1)

project("edgedetection")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -frtti -fexceptions")

# Find OpenCV
find_path(OPENCV_INCLUDE_DIR opencv2/opencv.hpp
    PATHS ${CMAKE_SOURCE_DIR}/../../../libs/OpenCV-android-sdk/sdk/native/jni/include)
    
find_library(OPENCV_LIB opencv_java4
    PATHS ${CMAKE_SOURCE_DIR}/../../../libs/OpenCV-android-sdk/sdk/native/libs/${ANDROID_ABI})

add_library(
    edgedetection
    SHARED
    native-lib.cpp
)

target_include_directories(edgedetection PRIVATE ${OPENCV_INCLUDE_DIR})
target_link_libraries(edgedetection ${OPENCV_LIB} android log)
cmake_minimum_required(VERSION 3.18.1)

project("edgedetection")

<<<<<<< HEAD
# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add your native source files
add_library(
		edgedetection
		SHARED
		native-lib.cpp
		edge_detector.cpp
		image_processor.cpp
)

# Link OpenCV libraries
target_link_libraries(
		edgedetection
		${OpenCV_LIBS}
		android
		log
		jnigraphics
)

# Set properties
set_target_properties(edgedetection PROPERTIES
		CXX_EXTENSIONS OFF
		POSITION_INDEPENDENT_CODE ON
)
=======
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
>>>>>>> 256b516098975afc85a50ec838aac4e8c57d69c6

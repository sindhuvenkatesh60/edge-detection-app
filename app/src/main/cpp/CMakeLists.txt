cmake_minimum_required(VERSION 3.18.1)

project("edgedetection")

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
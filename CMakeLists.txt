cmake_minimum_required(VERSION 3.20)
project(QR_IO)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add include directories for OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(
        QR_IO
        main.cpp
        BitBuffer.cpp
        BitBuffer.h
        QrCode.cpp
        QrCode.h
        QrSegment.cpp
        QrSegment.h
        Mode.cpp
        Mode.h
)

# Link against OpenCV libraries
target_link_libraries(QR_IO ${OpenCV_LIBS})

cmake_minimum_required(VERSION 3.20)
project(QR_IO)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add include directories for OpenCV headers
include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(.)

add_executable(
        QR_IO
        demo.cpp
        Qrio/BitStream.cpp
        Qrio/BitStream.h
        Qrio/SquareMatrix.cpp
        Qrio/SquareMatrix.h
        Qrio/Designator.h
        Qrio/DataAnalyzer.cpp
        Qrio/DataAnalyzer.h
        Qrio/DataSegment.cpp
        Qrio/DataSegment.h
        Qrio/Encoder.cpp
        Qrio/Encoder.h
        Qrio/ErrorCorrectionEncoder.cpp
        Qrio/ErrorCorrectionEncoder.h
        Qrio/Structurer.cpp
        Qrio/Structurer.h
        Qrio/Ecl.h
        Qrio/QrCode.cpp
        Qrio/main.cpp
        Qrio/ImageBinarization.hpp
        Qrio/ImageBinarization.cpp
        Qrio/FinderPatternModel.hpp
        Qrio/Filesystem.cpp
        Qrio/Filesystem.hpp
        Qrio/Generator.cpp
        Qrio/Generator.hpp
        Qrio/CodeFinder.hpp
        Qrio/CodeFinder.cpp
        Qrio/QrCode.h Qrio/Ecl.cpp)

target_compile_options(QR_IO PRIVATE -Wall -Wextra -Wpedantic)

# Link against OpenCV libraries
target_link_libraries(QR_IO ${OpenCV_LIBS})

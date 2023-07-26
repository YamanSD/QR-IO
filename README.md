# QR-IO

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenCV](https://img.shields.io/badge/opencv-%23white.svg?style=for-the-badge&logo=opencv&logoColor=white)

![QR-IO - QR Code](./images/qrio0.png)

## Description

QR-IO is a mini-library for QR code generation & reading.
It is based on the ISO/IEC 18004:2015 standard.
The project utilizes OpenCV2 to generate the QR images.
Currently supports generating QR codes only.

## Features

- Easy-to-use C++ interface.
- Utilizes the OpenCV library for image processing and QR code generation.
- Generates high-quality QR code images.
- Supports the most efficient encoding.
- Supports automatic numeric, alphanumeric, & byte encoding.
- Supports manual ECI usage.
- Supports manual structured append without ECI.
- Supports manual Kanji encoding.

## Upcoming features

- QR reading.
- Automatic structured append.
- Automatic Kanji encoding.

## Requirements

Before running the program, ensure you have the following installed on your system:

- C++ Compiler that supports C++20.
- OpenCV2
- CMake

## Usage

1. Clone the QR-IO repository to your local machine.
2. Add your main to the directory.
3. Modify the following section in the [CMakeLists.txt](./CMakeLists.txt) file:
  ```text
      ... 
      add_executable(
        QR_IO
        demo.cpp <- Replace with the name of your main file and add your files
      ...
  ```
5. Navigate to the directory & run the following commands:
   - `cmake .`
   - `make`
      - On Unix:    `./QR_IO`
      - On Windows: `.\QR_IO`

- You can check the [demo.cpp](./demo.cpp) for example usage.
- You can also use the pre-compiled executables included in the project.

## Example QRs 



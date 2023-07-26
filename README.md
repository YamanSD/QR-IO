# QR-IO

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white)
![OpenCV](https://img.shields.io/badge/opencv-%23white.svg?style=for-the-badge&logo=opencv&logoColor=white)

![QR-IO - QR Code](./images/qrio_0.png)

## Description

QR-IO is a mini-library for QR code generation & reading.
It is based on the ISO/IEC 18004:2015 standard.
The project utilizes OpenCV2 to generate the QR images.
Currently supports generating QR codes only.

## Features

- Easy-to-use C++ interface.
- Utilizes the OpenCV library for image processing and QR code generation.
- Generates high-quality QR code images.
- Supports the most efficient encoding for all strings that are either pure Kanji or do not contain any Kanji.
- Supports automatic encoding.
- Supports manual ECI usage.
- Supports manual structured append without ECI.
- Supports FNC1 encoding.

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

- "This is a test" (1-L):
  
     ![QR-IO - QR Code](./images/qrw_0L.png)

- "This is a test" (1-M):
 
    ![QR-IO - QR Code](./images/qrw_0M.png)


- "This is a test" (1-Q):
 
    ![QR-IO - QR Code](./images/qrw_0Q.png)

- "This is a test" (1-H):
 
    ![QR-IO - QR Code](./images/qrw_0H.png)

- "This is a test" (25-H):
 
    ![QR-IO - QR Code](./images/qrw_0B.png)

- "This is a test" (1-H):
 
    ![QR-IO - QR Code](./images/qrw_0D.png)

- "This is a test" (1-H):
 
    ![QR-IO - QR Code](./images/qrw_0E.png)

- Check qrw_1 in [demo.cpp](./demp.cpp):
 
    ![QR-IO - QR Code](./images/qrw_1.png)

- Check qrw_2 in [demo.cpp](./demp.cpp):
 
    ![QR-IO - QR Code](./images/qrw_2.png)

- "This is a Kanji test: \u935F\uE4AA\u89D7\uE4AA!":
 
    ![QR-IO - QR Code](./images/qrw_2A.png)

- "You can also use regular strings!":
 
    ![QR-IO - QR Code](./images/qr_0.png)

- An innocent google search link ;) (non-malicious):
 
    ![QR-IO - QR Code](./images/qr_1.png)

- "Hello world!" in binary:

    ![QR-IO - QR Code](./images/qr_2.png)

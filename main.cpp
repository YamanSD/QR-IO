/*
 * MIT License
 * Copyright (c) 2023 Yolo
 *
 * Permission is hereby granted free of charge to any person obtaining a copy
 * of this software and associated documentation files (the "Software") to deal
 * in the Software without restriction including without limitation the rights
 * to use copy modify merge publish distribute sublicense and/or sell
 * copies of the Software and to permit persons to whom the Software is
 * furnished to do so subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND EXPRESS OR
 * IMPLIED INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT TORT OR OTHERWISE ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Qrio/DataAnalyzer.h>
#include <Qrio/Encoder.h>
#include <Qrio/ErrorCorrectionEncoder.h>
#include <Qrio/Structurer.h>
#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"

using namespace std;
using namespace Qrio;

void saveMatrixAsImageWithBorder(const Structurer& matrix, const std::string& filename, int borderSize, const cv::Scalar& borderColor) {
    // Define the size of each square (10px by 10px)
    const int squareSize = 10;

    // Calculate the size of the output image (including the border on all four sides)
    int rows = (matrix.size() + 2 * borderSize) * squareSize;
    int cols = (matrix[0].size() + 2 * borderSize) * squareSize;

    // Create an image to store the matrix data with border
    cv::Mat image(rows, cols, CV_8UC3, borderColor);

    // Draw the squares based on the matrix data
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[i].size(); ++j) {
            int x = (j + borderSize) * squareSize; // Apply the horizontal border
            int y = (i + borderSize) * squareSize; // Apply the vertical border
            cv::Rect rect(x, y, squareSize, squareSize);
            if (matrix[i][j] == 0) {
                // Draw white square (1) - (255, 255, 255) represents white color
                cv::rectangle(image, rect, cv::Scalar(255, 255, 255), cv::FILLED);
            } else {
                // Draw black square (0)
                cv::rectangle(image, rect, cv::Scalar(0, 0, 0), cv::FILLED);
            }
        }
    }

    // Save the image to the specified filename
    cv::imwrite(filename, image);
}

void saveMatrixAsImage(const Structurer& matrix, const std::string& filename) {
    // Define the size of each square (10px by 10px)
    const int squareSize = 10;

    // Calculate the size of the output image
    int rows = (matrix.size() + 2) * squareSize;
    int cols = (matrix[0].size() + 2) * squareSize;

    // Create an image to store the matrix data
    cv::Mat image(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0)); // Initialize with black pixels

    const auto temp{rows};

    // Draw the squares based on the matrix data
    for (int i = -1; i < temp; ++i) {
        for (int j = -1; j < temp; ++j) {
            cv::Rect rect((j + 1) * squareSize, (i + 1) * squareSize,
                          squareSize, squareSize);

            if (i == -1 or j == -1 or j == temp - 1 or i == temp - 1) {
                cv::rectangle(image, rect, cv::Scalar(255, 255, 255), cv::FILLED);
                continue;
            }

            if (matrix[i][j] == 0) {
                // Draw white square (1) - (255, 255, 255) represents white color
                cv::rectangle(image, rect, cv::Scalar(255, 255, 255), cv::FILLED);
            } else {
                // Draw black square (0)
                cv::rectangle(image, rect, cv::Scalar(0, 0, 0), cv::FILLED);
            }
        }
    }

    // Save the image to the specified filename
    cv::imwrite(filename,  image);
}

void printQr(const Structurer &qr) {
    int border = 4;
//    cout << qr.size() << endl;
//
//    cout << -border << ' ' << qr.size() + border << endl;
//    unsigned y = -border;
//
//    cout << (y < qr.size() + border) << endl;

    for (int y = 0; y < qr.size(); y++) {
        for (int x = 0; x < qr.size(); x++) {
            cout << (qr.at(x, y) ? "##" : "  ");
        }
        cout << endl;
    }

    cout << "ALL GOOD " << qr.final_mask << endl;
}

void save(const string& file_name) {

}


int main() {
    const wstring t{L"01234567"};
    wstring j{L"\ue4aa\u935fA"};

    auto w = DataAnalyzer(t, 1, Ecl::M);
    auto temp{Encoder(w)};
    ErrorCorrectionEncoder k{temp};
    Structurer f{k, 2};
//

std::string filename = "./output_image.png";

saveMatrixAsImageWithBorder(f, filename, 2, {255, 255, 255});

return 0;
}



/*
 * MIT License
 * Copyright (c) 2023 Yolo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string>

#include "opencv2/opencv.hpp"
#include "QrCode.h"


namespace Qrio {
    using std::string, std::wstring;
    using cv::Scalar;

    QrCode::QrCode(const wstring& data): matrix{ErrorCorrectionEncoder(Encoder(DataAnalyzer(data, 10)))} {
    }

    void QrCode::save(const string& filename, int borderSize, const cv::Scalar& borderColor) const {
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
}

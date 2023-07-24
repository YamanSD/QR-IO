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
    using cv::imwrite, cv::rectangle, cv::FILLED,
            cv::Mat, cv::Rect, cv::Scalar;

    QrCode::QrCode(const wstring& data):
    matrix{ErrorCorrectionEncoder(Encoder(DataAnalyzer(data, getVersion(data))))} {
    }

    /*
     * Pre-Conditions:
     *      File name to save the QR code image at,
     *      optional scale in pixels (default 10px),
     *      optional border_width (default 4X, Check 6.3.8),
     *      optional RGB light_color (default white),
     *      optional RGB dark_color (default black).
     *
     * Post-Conditions:
     *      Saves the QR code as an image under the given file name,
     *      in the local directory or the directory specified in the file name.
     */
    void QrCode::save(const string& filename,
                      int scale,
                      int border_width,
                      const Scalar& light_color,
                      const Scalar& dark_color) const {
        const int S{static_cast<int>(matrix.size())};

        /* Calculate the side of the output image (including the border on all four sides) */
        const int N{(S + 2 * border_width) * scale};

        /* Create an image to store the matrix data with border */
        Mat image(N, N, CV_8UC3, light_color);

        /* Draw the squares */
        for (int i{0}; i < N; i++) {
            for (int j{0}; j < N; j++) {
                Rect rect((j + border_width) * scale,
                              (i + border_width) * scale,
                              scale,
                              scale);

                if (i < S and j < S and matrix.at(i, j)) {
                    rectangle(image, rect, dark_color, FILLED);
                } else {
                    rectangle(image, rect, light_color, FILLED);
                }
            }
        }

        /* Save the image to the specified filename */
        imwrite(filename, image);
    }
}

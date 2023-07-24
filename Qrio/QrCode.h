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

#ifndef QR_IO_QRCODE_H
#define QR_IO_QRCODE_H

#include <string>
#include <variant>
#include <vector>

#include "opencv2/opencv.hpp"

#include "DataAnalyzer.h"
#include "Encoder.h"
#include "ErrorCorrectionEncoder.h"
#include "Structurer.h"


namespace Qrio {
    class QrCode final {
    public:
        // TODO finalize
        explicit QrCode(const std::string&);

        // TODO add options
        explicit QrCode(const std::wstring&);

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
        void save(const std::string& file_name,
                  int scale = 10,
                  int border_width = 4,
                  const cv::Scalar& light_color = {255, 255, 255},
                  const cv::Scalar& dark_color = {0, 0, 0}) const;

        [[nodiscard]] static std::vector<QrCode> makeStructured(
                const std::vector<std::variant<std::wstring, std::string>>&);

    private:
        /* Stores the generated QR code */
        Structurer matrix;

        [[nodiscard]] static int getVersion(const std::wstring&);
    };
}


#endif //QR_IO_QRCODE_H

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
#include <unordered_map>
#include <variant>
#include <vector>

#include "opencv2/opencv.hpp"

#include "DataAnalyzer.h"
#include "Designator.h"
#include "Ecl.h"
#include "Encoder.h"
#include "ErrorCorrectionEncoder.h"
#include "Structurer.h"


namespace Qrio {
    class QrCode final {
    public:
        /*
         * Pre-Conditions:
         *      Data string (0x5C values must be doubled),
         *
         *      optional override mode used to determine the encoding method manually
         *      (The entire QR will be encoded in the given type)
         *      default is Designator::TERMINATOR,
         *
         *      optional ECL (Error Correction Level) default is Low (Ecl::L),
         *
         *      optional version to specify the version of the QR code
         *      (Default is -1 under which a suitable version is determined,
         *       if the given version does not fit the data
         *       and ECL or the data is too large, a length exception is thrown),
         *
         *      optional ECI unordered_map, where the indices of the ECI
         *      are mapped to their ECI value
         *      (Each ECI will be placed at the given index in the encoding),
         *
         *      optional fnc1 (1 indicates the usage of FNC1-1st position,
         *                      2 indicates the usage of FNC1-2nd position),
         *
         *      optional structured append ID of the given QR (must be between 0 and 15),
         *
         *      optional structured append count of the given QR (number of linked QRs),
         *
         *      optional mask to specify the mask of the QR (-1 for auto).
         *
         * Post-Conditions:
         *      Performs the necessary operations on the data to generate a QR boolean matrix,
         *      where a 0 indicates a light square and a 1 indicates a dark square.
         */
        explicit QrCode(const std::variant<std::wstring, std::string>&,
                        Ecl ecl = Ecl::L,
                        Designator override_mode = Designator::TERMINATOR,
                        int version = -1,
                        int mask = -1,
                        int fnc1 = 0,
                        int struct_id = -1,
                        int struct_count = -1);

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

        /*
         * Pre-Conditions:
         *      Vector of data QR codes,
         *      ECL for all the QR codes,
         *      encoding method for all QR codes,
         *      version for all the QR codes,
         *      mask for all the QR codes,
         *      fnc1 mode for all QR codes.
         *
         * Post-Conditions:
         *      Generates QR codes for the given data strings.
         */
        [[nodiscard]] static std::vector<QrCode> makeStructured(
                const std::vector<std::wstring>&,
                Ecl ecl = Ecl::L,
                Designator override_mode = Designator::TERMINATOR,
                int version = -1,
                int mask = -1,
                int fnc1 = 0);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the QR version.
         */
        [[nodiscard, maybe_unused]] int getVersion() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the QR mask.
         */
        [[nodiscard, maybe_unused]] int getMask() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the QR ECL.
         */
        [[nodiscard, maybe_unused]] Ecl getEcl() const;
    private:
        /* Stores the generated QR code */
        Structurer matrix;

        /*
         * Pre-Conditions:
         *      Data string,
         *      ECL,
         *      Preferred version to be used.
         *
         * Post-Conditions:
         *      Returns the preferred version if it can store the data string,
         *      otherwise if it is not specified (or invalid), an appropriate
         *      version is determined.
         *      If no version can store the given QR code, a length exception is thrown.
         */
        [[nodiscard]] static int getVersion(
                const std::variant<std::wstring, std::string>&,
                Ecl, int, Designator);

        /*
         * Pre-Conditions:
         *      Data string.
         *
         * Post-Conditions:
         *      Returns the processed data string that can be used.
         *      Iff the given data string is invalid,
         *      invalid_argument exception is thrown.
         */
        [[nodiscard]] static std::wstring processedData(
                const std::variant<std::wstring, std::string>&);

        /*
         * Pre-Conditions:
         *      Data string,
         *      ECL,
         *      Preferred version to be used.
         *
         * Post-Conditions:
         *      Returns true if the given version works for the ECL
         *      and data, otherwise false.
         */
        [[nodiscard]] static bool testVersion(
                const std::variant<std::wstring, std::string>&,
                Ecl, int, Designator);

        /*
         * Pre-Conditions:
         *      Data string containing valid ECIs.
         *
         * Post-Conditions:
         *      Generates the ECI map.
         */
        [[nodiscard]] static std::unordered_map<size_t, int> getEci(
                const std::variant<std::wstring, std::string>&);

        /*
         * Pre-Conditions:
         *      Data string.
         *
         * Post-Conditions:
         *      Returns a wstring.
         */
        [[nodiscard]] static std::wstring extractWideString(
                const std::variant<std::wstring, std::string>&);
    };
}


#endif //QR_IO_QRCODE_H

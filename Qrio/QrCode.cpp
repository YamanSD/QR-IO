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

#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "opencv2/opencv.hpp"
#include "QrCode.h"


namespace Qrio {
    using std::string, std::wstring, std::variant,
    std::move, std::min, std::length_error, std::holds_alternative,
    std::get, std::unordered_map, std::invalid_argument, std::stoi,
    std::to_string, std::vector;
    using cv::imwrite, cv::rectangle, cv::FILLED,
            cv::Mat, cv::Rect, cv::Scalar;

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

    /*
     * Pre-Conditions:
     *      Data string,
     *
     *      optional override mode used to determine the encoding method manually
     *      (The entire QR will be encoded in the given type),
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
    QrCode::QrCode(const variant<wstring, string>& data, Ecl ecl, Designator override_mode,
                   int version, int mask, int fnc1, int struct_id, int struct_count):
                   matrix{ErrorCorrectionEncoder(Encoder(
                           DataAnalyzer(processedData(data), getVersion(data, ecl, version, override_mode),
                                        ecl, override_mode, getEci(data),
                                        fnc1, struct_id, struct_count))), mask} {}

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
    int QrCode::getVersion(const variant<wstring, string>& data,
                           Ecl ecl,
                           int preferred_version,
                           Designator mode) {
        /* Minimum & maximum possible versions, based on the standard */
        const static int MIN_VERSION{DataAnalyzer::MIN_VERSION},
                            MAX_VERSION{DataAnalyzer::MAX_VERSION};

        if (preferred_version < MIN_VERSION
            or MAX_VERSION < preferred_version) {
            /* Generate a new version */
            int high{MAX_VERSION}, low{MIN_VERSION},
                mid, prev_success{-1};

            while (low <= high) {
                mid = (high + low) / 2;

                if (testVersion(data, ecl, mid, mode)) {
                    prev_success = mid;
                    high = mid - 1;
                } else {
                    low = mid + 1;
                }
            }

            if (prev_success == -1) {
                throw length_error("Data too long");
            }

            return prev_success;
        }

        /* Use preferred version */
        if (testVersion(data, ecl, preferred_version, mode)) {
            return preferred_version;
        } else {
            throw length_error("Given preferred version does not fit data");
        }
    }

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
    bool QrCode::testVersion(const variant<wstring, string>& data,
                             Ecl ecl, int version, Designator mode) {
        try {
            if (holds_alternative<wstring>(data)) {
                DataAnalyzer analyzer{get<wstring>(data), version, ecl, mode};
                Encoder encoder{analyzer};
            } else {
                wstring temp{get<string>(data).begin(), get<string>(data).end()};
                DataAnalyzer analyzer{temp, version, ecl};
                Encoder encoder{analyzer};
            }

            return true;
        } catch (...) {
            return false;
        }
    }

    /*
     * Pre-Conditions:
     *      Data string.
     *
     * Post-Conditions:
     *      Returns the processed data string that can be used.
     *      Iff the given data string is invalid,
     *      invalid_argument exception is thrown.
     */
    wstring QrCode::processedData(const variant<wstring, string>& data) {
        wstring result{extractWideString(data)};
        wstring result_cpy{result};
        const auto N{result_cpy.size()};

        const auto throw_exc{[](size_t index) {
            throw invalid_argument("\nInvalid ECI symbol at "
            + to_string(index)
            + "\n");
        }};

        for (size_t i{0}; i < N; i++) {
            if (result_cpy[i] == 0x5C) {
                if (i == N - 2) {
                    if (result_cpy[i + 1] == 0x5C) {
                        break;
                    } else {
                        throw_exc(i);
                    }
                }

                if (result_cpy[i + 1] == 0x5C) {
                    continue;
                }

                try {
                    stoi(result_cpy.substr(i + 1, 6));
                } catch (...) {
                    throw_exc(i);
                }

                result.erase(i, 7);
                i += 7;
            }
        }

        return result;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the QR version.
     */
    int QrCode::getVersion() const {
        return matrix.ec_encoder.encoder.analyzer.getVersion();
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the QR mask.
     */
    int QrCode::getMask() const {
        return matrix.final_mask;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the QR ECL.
     */
    Ecl QrCode::getEcl() const {
        return matrix.ec_encoder.encoder.analyzer.getEcl();
    }

    /*
     * Pre-Conditions:
     *      Data string.
     *
     * Post-Conditions:
     *      Generates the ECI map.
     */
    unordered_map<size_t, int> QrCode::getEci(
            const variant<wstring, string>& crude_data) {
        const auto& data{extractWideString(crude_data)};
        const auto N{data.size()};

        unordered_map<size_t, int> result{};

        for (size_t i{0}; i < N - 6; i++) {
            if (data[i] == 0x5C and data[i + 1] != 0x5C) {
                result[i] = stoi(data.substr(i + 1, 6));
            }
        }

        return result;
    }

    /*
     * Pre-Conditions:
     *      Data string.
     *
     * Post-Conditions:
     *      Returns a wstring.
     */
    wstring QrCode::extractWideString(const variant<wstring, string>& data) {
        if (holds_alternative<wstring>(data)) {
            return get<wstring>(data);
        }

        return wstring{get<string>(data).begin(),
                       get<string>(data).end()};
    }

    /*
     * Pre-Conditions:
     *      Vector of data QR codes,
     *      ECL for all the QR codes,
     *      version for all the QR codes,
     *      mask for all the QR codes.
     *
     * Post-Conditions:
     *      Generates QR codes for the given data strings.
     */
    vector<QrCode> QrCode::makeStructured(const vector<wstring>& data,
                                          Ecl ecl,
                                          Designator override_mode,
                                          int version,
                                          int mask,
                                          int fnc1) {
        if (data.empty() or 16 < data.size()) {
            throw invalid_argument(
                    "Structured append requires at least 1 data string and at most 16\n"
                    );
        }

        const int N{static_cast<int>(data.size())};
        vector<QrCode> result;

        for (int i{0}; i < N; i++) {
            result.emplace_back(
                    QrCode(data[i],
                           ecl,
                           override_mode,
                           version,
                           mask,
                           fnc1,
                           i,
                           N)
                    );
        }

        return result;
    }
}

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

#include <cctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Mode.h"
#include "QrSegment.h"


namespace qr_io {
    using std::domain_error, std::length_error, std::stol,
            std::string, std::vector, std::to_string;

    QrSegment::QrSegment(const vector<int>& data):
    mode{ModeType::BYTE}, buffer{}, size{data.size()} {
        if (UINT_MAX < data.size()) {
            throw length_error("Data too long");
        }

        for (auto b: data) {
            buffer.appendBits(b, 8);
        }
    }

    QrSegment::QrSegment(ModeType mode_type,
                         const string& data):
                         mode{mode_type}, buffer{}, size{data.size()} {
        int accumData{0}, accumCount{0};

        if (mode_type == ModeType::NUMERIC) {
            for (char c: data) {
                if (c < '0' or '9' < c) {
                    throw domain_error("\nNumeric char is '"
                                            + to_string(c) + "'\n");
                }

                accumData = accumData * 10 + c - '0';
                accumCount++;

                if (accumCount == 3) {
                    buffer.appendBits(accumData, 10);
                    accumData = accumCount = 0;
                }
            }

            if (accumCount) {
                buffer.appendBits(accumData, 3 * accumCount + 1);
            }
        } else if (mode_type == ModeType::ALPHANUMERIC) {
            int index;

            for (auto c: data) {
                index = validAlphanumeric(c);

                if (index == -1) {
                    throw domain_error("\nAlphanumeric char is '"
                                            + to_string(c) + "'\n");
                }

                accumData = accumData * 45 + index;
                accumCount++;

                if (accumCount == 2) {
                    buffer.appendBits(accumData, 11);
                    accumData = accumCount = 0;
                }
            }

            if (accumCount) {
                buffer.appendBits(accumData, 6);
            }
        } else if (mode_type == ModeType::BYTE) {
            vector<int> bytes{};

            for (auto c: data) {
                bytes.push_back(c);
            }

            *this = QrSegment(bytes);
        } else if (mode_type == ModeType::ECI) {
            const long value{stol(data)};

            if (value < 0 or 1'000'000L <= value) {
                throw domain_error("\nECI value out of range (" + data + ")\n");
            } else if (value >> 7 == 0) {
                buffer.appendBits(static_cast<int>(value), 8);
            } else if (value >> 14 == 0) {
                buffer.appendBits(2, 2);
                buffer.appendBits(static_cast<int>(value), 14);
            } else {
                buffer.appendBits(6, 3);
                buffer.appendBits(static_cast<int>(value), 21);
            }

            size = 0;
        } else {
            throw domain_error("Invalid mode");
        }
    }

    QrSegment::QrSegment(const string& data):
        QrSegment(getDataType(data), data) {}

    QrSegment::QrSegment(long value):
    QrSegment(ModeType::ECI, to_string(value)) {}

    const BitBuffer& QrSegment::getData() const {
        return buffer;
    }

    size_t QrSegment::getDataSize() const {
        return getData().size();
    }

    size_t QrSegment::getSize() const {
        return size;
    }

    const Mode& QrSegment::getMode() const {
        return mode;
    }

    size_t QrSegment::getTotalBits(const vector<QrSegment>& segments,
                                int version) {
        size_t result{0}, ccbits;

        for (const auto& s: segments) {
            ccbits = s.mode.charWidth(version);

            if ((1L << ccbits) <= s.getSize()
                    or INT_MAX - result < ccbits + 4) {
                return -1;
            }

            result += 4 + ccbits;

            if (static_cast<int>(INT_MAX - result) < s.getDataSize()) {
                return -1;
            }

            result += s.getDataSize();
        }

        return result;
    }

    ModeType QrSegment::getDataType(const string& str) {
        bool isNumeric{true};

        for (auto c: str) {
            if (not validAlphanumeric(c)) {
                return ModeType::BYTE;
            }

            if (not isdigit(c)) {
                isNumeric = false;
            }
        }

        return isNumeric ? ModeType::NUMERIC : ModeType::ALPHANUMERIC;
    }

    int QrSegment::validAlphanumeric(char c) {
        static const std::string& DOMAIN {
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:"
        };

        for (int i{0}; i < DOMAIN.size(); i++) {
            if (DOMAIN[i] == c) {
                return i;
            }
        }

        return -1;
    }
}

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

#include <algorithm>
#include <unordered_map>
#include <utility>
#include <stdexcept>
#include <vector>

#include "DataAnalyzer.h"
#include "Ecl.h"


namespace Qrio {
    using std::domain_error, std::all_of, std::min, std::move,
            std::unordered_map, std::string, std::vector, std::wstring,
            std::range_error;

    /*
     * Pre-Conditions:
     *      Data wstring,
     *      Version to be used.
     *
     * Post-Conditions:
     *      Segments contains optimized DataSegments,
     *      data contains copy of the given data wstring
     *
     *
     * Initializes the data members.
     * Fills the segments with the optimal DataSegments.
     */
    DataAnalyzer::DataAnalyzer(wstring data_cpy, int version, Ecl ecl, Designator override_mode,
                               unordered_map<size_t, int> eci):
    version{version}, data{move(data_cpy)}, ecl{ecl}, eci{move(eci)} {
        checkVersion();
        checkOverrideMode(override_mode);

        if (override_mode == Designator::NUMERIC or isNumeric(data)) {
            push_back(DataSegment{data, 0, data.size(), Designator::NUMERIC});
            return;
        } else if (override_mode == Designator::ALPHANUMERIC or isAlphanumeric(data)) {
            push_back(DataSegment{data, 0, data.size(), Designator::ALPHANUMERIC});
            return;
        } else if (override_mode == Designator::KANJI or isKanji(data)) {
            push_back(DataSegment{data, 0, data.size(), Designator::KANJI});
            return;
        } else if (override_mode == Designator::BYTE or isByte(data)) {
            push_back(DataSegment{data, 0, data.size(), Designator::BYTE});
            return;
        } else if (override_mode != Designator::TERMINATOR) {
            push_back(DataSegment{data, 0, data.size(), override_mode});
            return;
        }

        auto current_mode{getInitialMode()};
        const auto range{getVersionRange()};
        size_t left{0}, current{0}, n{data.size()}, temp;
        bool switched, was_kanji;

        while (current < n) {
            switched = false;

            if (current_mode == Designator::NUMERIC) {
                if (isKanji(data[current])) {
                    switched = true;
                    current_mode = Designator::KANJI;
                } else if (isByte(data[current])) {
                    switched = true;
                    current_mode = Designator::BYTE;
                } else if (isAlphanumeric(data[current])) {
                    switched = true;
                    current_mode = Designator::ALPHANUMERIC;
                }

                if (switched) {
                    push_back(DataSegment{data, left,
                                          current, Designator::NUMERIC});
                    left = current;
                }
            } else if (current_mode == Designator::ALPHANUMERIC) {
                if (isKanji(data[current])) {
                    switched = true;
                    current_mode = Designator::KANJI;
                } else if (isByte(data[current])) {
                    switched = true;
                    current_mode = Designator::BYTE;
                }

                temp = countNumeric(data, current);

                if (13 + 2 * range <= temp) {
                    switched = true;
                    current_mode = Designator::NUMERIC;
                }

                if (switched) {
                    push_back(DataSegment{data, left,
                                          current, Designator::ALPHANUMERIC});
                    left = current;
                }
            } else { // Kanji and Byte
                was_kanji = current_mode == Designator::KANJI;

                if (current_mode == Designator::BYTE
                    and 9 + min(3 * range, 4) <= countKanji(data, current)) {
                    switched = true;
                    current_mode = Designator::KANJI;
                }

                temp = countAlphanumeric(data, current);

                if (11 + min(4 * range, 5) <= temp) {
                    switched = true;
                    current_mode = Designator::ALPHANUMERIC;
                }

                if (not switched) {
                    temp = countNumeric(data, current);

                    if (6 + min(2 * range, 3) <= temp
                    or (6 + range <= temp and isAlphanumeric(data[current + temp]))) {
                        switched = true;
                        current_mode = Designator::NUMERIC;
                    }
                }

                if (switched) {
                    push_back(DataSegment{data, left,
                                          current, was_kanji ? Designator::KANJI
                                                             : Designator::BYTE});
                    left = current;
                }
            }

            current++;
        }

        if (left < n) {
            push_back(DataSegment{data, left, current, current_mode});
        }
    }

    /*
     * Pre-Conditions:
     *      Data wstring,
     *      Version to be used.
     *
     * Post-Conditions:
     *      Segments contains optimized DataSegments,
     *      data contains copy of the given data wstring
     *
     *
     * Initializes the data members.
     * Fills the segments with the optimal DataSegments.
     *
     * Check Annex J.
     */
    DataAnalyzer::DataAnalyzer(const string& data_cpy, int version,
                               Ecl ecl, Designator override_mode,
                               const unordered_map<size_t, int>& eci):
    DataAnalyzer(
            move(wstring().assign(data_cpy.begin(), data_cpy.end())),
            version,
            ecl,
            override_mode,
            eci) {}

    /*
     * Pre-Conditions:
     *      A character c.
     *
     * Post-Conditions:
     *      Returns true if the given wchar_t is numeric.
     *
     * Check Annex J.
     */
    bool DataAnalyzer::isNumeric(wchar_t c) {
        return 0x30 <= c and c <= 0x39;
    }

    /*
     * Pre-Conditions:
     *      A character c.
     *
     * Post-Conditions:
     *      Returns true if the given wchar_t is alphanumeric.
     *
     * Check Annex J.
     */
    bool DataAnalyzer::isAlphanumeric(wchar_t c) {
        return c == 0x20 or c == 0x24
                or c == 0x25 or c == 0x2A
                or c == 0x2B or c == 0x3A
                or (0x2D <= c and c <= 0x2F)
                or (0x41 <= c and c <= 0x5A);
    }

    /*
     * Pre-Conditions:
     *      A character c.
     *
     * Post-Conditions:
     *      Returns true if the given wchar_t is byte.
     *
     * Check Annex J.
     */
    bool DataAnalyzer::isByte(wchar_t c) {
        return c == 0x2C
                or (0x00 <= c and c <= 0x1F)
                or (0x21 <= c and c <= 0x23)
                or (0x26 <= c and c <= 0x29)
                or (0x3B <= c and c <= 0x40)
                or (0x5B <= c and c <= 0x7F)
                or (0xA0 <= c and c <= 0xDF);
    }

    /*
     * Pre-Conditions:
     *      A character c.
     *
     * Post-Conditions:
     *      Returns true if the given wchar_t is a kanji.
     *
     * Check Annex H.
     */
    bool DataAnalyzer::isKanji(long b0, long b1) {
        return ((0xE0 <= b0 and b0 <= 0xEA) or (0x81 <= b0 and b0 <= 0x9F))
                and ((0x40 <= b1 and b1 <= 0x7E) or (0x80 <= b1 and b1 <= 0xFC))
                or ((0xEA <= b0 and b0 <= 0xEB)
                and ((0x40 <= b1 and b1 <= 0x7E) or (0x80 <= b1 and b1 <= 0xBF)));
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring.
     *
     * Post-Conditions:
     *      Returns the initial mode.
     *
     * Check Annex J.
     */
    Designator DataAnalyzer::getInitialMode() const {
        // Case 1
        if (isByte(data[0])) {
            return Designator::BYTE;
        }

        const auto range{getVersionRange()};

        // Case 2
        if (isKanji(data[0])) {
            if (isAlphanumeric(data[1]) or isNumeric(data[1])) {
                return Designator::KANJI;
            } else if (isByte(data[1])
                and (range == 2 ? 6 : 5) <= countKanji(data, 1)) {
                return Designator::BYTE;
            }
        }

        // Case 3
        if (isAlphanumeric(data[0])) {
            const auto temp{countAlphanumeric(data)};

            if (temp < 6 + range
                and (isByte(data[temp] or isKanji(data[temp])))) {
                return Designator::BYTE;
            }

            return Designator::ALPHANUMERIC;
        }

        // Case 4
        if (isNumeric(data[0])) {
            const auto temp{countNumeric(data)};

            if (temp < (range == 2 ? 5 : 4)
                and (isByte(data[temp] or isKanji(data[temp])))) {
                return Designator::BYTE;
            }

            if (temp < 7 + range and isAlphanumeric(data[temp])) {
                return Designator::ALPHANUMERIC;
            }

            return Designator::NUMERIC;
        }

        return Designator::BYTE;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the value associated with a given version range.
     *
     * Review J.2 page 100 of the standard.
     */
    int DataAnalyzer::getVersionRange() const {
        return 27 <= version ? 2 : (10 <= version ? 1 : 0);
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns true if the given wstring consists exclusively of bytes.
     */
    bool DataAnalyzer::isByte(const wstring& data) {
        return all_of(data.begin(), data.end(), [](auto c) {
            return isByte(c);
        });
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns true if the given wstring consists exclusively of numerics.
     */
    bool DataAnalyzer::isNumeric(const wstring& data) {
        return all_of(data.begin(), data.end(), [](auto c) {
            return isNumeric(c);
        });
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns true if the given wstring consists exclusively of alphanumerics.
     */
    bool DataAnalyzer::isAlphanumeric(const wstring& data) {
        return all_of(data.begin(), data.end(), [](auto c) {
            return isAlphanumeric(c);
        });
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns the number of the byte chars at the start
     *      of the wstring.
     */
    size_t DataAnalyzer::countByte(const wstring& data, size_t start) {
        size_t counter{0};

        for (auto i{start}; i < data.size(); i++) {
            if (not isByte(data[i])) {
                break;
            }

            counter++;
        }

        return counter;
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns the number of the numeric chars at the start
     *      of the wstring.
     */
    size_t DataAnalyzer::countNumeric(const wstring& data, size_t start) {
        size_t counter{0};

        for (auto i{start}; i < data.size(); i++) {
            if (not isNumeric(data[i])) {
                break;
            }

            counter++;
        }

        return counter;
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns the number of the alphanumeric chars at the start
     *      of the wstring.
     */
    size_t DataAnalyzer::countAlphanumeric(const wstring& data, size_t start) {
        size_t counter{0};

        for (auto i{start}; i < data.size(); i++) {
            if (not isAlphanumeric(data[i])) {
                break;
            }

            counter++;
        }

        return counter;
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns the number of the Kenji byte pairs at the start
     *      of the subwstring.
     */
    size_t DataAnalyzer::countKanji(const wstring& data, size_t start) {
        size_t counter{0};

        for (size_t i{start}; i < data.size(); i += 2) {
            // If the bit pairs are not Kanji break.
            if (not isKanji(data[i])) {
                break;
            }

            counter++;
        }

        return counter;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Throws a domain exception if the version is out of bounds.
     */
    void DataAnalyzer::checkVersion() const {
        if (version < MIN_VERSION or MAX_VERSION < version) {
            throw domain_error("Data too long or version out of bounds [1, 40]");
        }
    }

    /*
     * Pre-Conditions:
     *      A constant reference to a wstring,
     *      starting index.
     *
     * Post-Conditions:
     *      Returns true if the given wstring consists exclusively of Kanji characters.
     */
    bool DataAnalyzer::isKanji(const wstring& data) {
        return all_of(data.begin(), data.end(), [](auto c) {
            return isKanji(c);
        });
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the chosen QR version.
     */
    int DataAnalyzer::getVersion() const {
        return version;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns a constant reference to the data wstring.
     */
    const wstring &DataAnalyzer::getData() const {
        return data;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the chosen ECL.
     */
    Ecl DataAnalyzer::getEcl() const {
        return ecl;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the given ECI.
     */
    const unordered_map<size_t, int>& DataAnalyzer::getEci() const {
        return eci;
    }

    /*
     * Pre-Conditions:
     *      A character c.
     *
     * Post-Conditions:
     *      Returns true if the given wchar_t is kanji.
     */
    bool DataAnalyzer::isKanji(wchar_t c) {
        return isKanji(c / (16 * 16), c % (16 * 16));
    }

    /*
     * Pre-Conditions:
     *      Data string.
     *
     * Post-Conditions:
     *      Returns true if the given data can be encoded in alphanumeric mode.
     */
    bool DataAnalyzer::isCompatibleAlphanumeric(const wstring& data) {
        return all_of(data.begin(), data.end(), [](auto c) {
            return isNumeric(c) or isAlphanumeric(c);
        });
    }

    /*
     * Pre-Conditions:
     *      Data string.
     *
     * Post-Conditions:
     *      Returns true if the given data can be encoded in Byte mode.
     */
    bool DataAnalyzer::isCompatibleByte(const wstring& data) {
        return all_of(data.begin(), data.end(), [](auto c) {
            return isNumeric(c) or isAlphanumeric(c) or isByte(c);
        });
    }

    /*
     * Pre-Conditions:
     *      Data string.
     *
     * Post-Conditions:
     *      Returns true if the given data can be encoded in Kanji mode.
     */
    bool DataAnalyzer::isCompatibleKanji(const wstring& data) {
        return all_of(data.begin(), data.end(), [](auto c) {
            return isNumeric(c) or isAlphanumeric(c) or isByte(c) or isKanji(c);
        });
    }

    /*
     * Pre-Conditions:
     *      Data initialized.
     *
     * Post-Conditions:
     *      Throws a range error if the given designator is
     *      invalid with the data.
     */
    void DataAnalyzer::checkOverrideMode(Designator override_mode) const {
        if (override_mode != Designator::TERMINATOR) {
            switch (override_mode) {
                case Designator::NUMERIC:
                    if (not isNumeric(data)) {
                        throw range_error("Invalid override mode");
                    }
                    break;
                case Designator::ALPHANUMERIC:
                    if (not isCompatibleAlphanumeric(data)) {
                        throw range_error("Invalid override mode");
                    }
                    break;
                case Designator::KANJI:
                    if (not isCompatibleKanji(data)) {
                        throw range_error("Invalid override mode");
                    }
                    break;
                case Designator::BYTE:
                    if (not isCompatibleByte(data)) {
                        throw range_error("Invalid override mode");
                    }
                    break;
                default:
                    return;
            }
        }
    }
}

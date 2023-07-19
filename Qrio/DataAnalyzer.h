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

#ifndef QR_IO_DATAANALYZER_H
#define QR_IO_DATAANALYZER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "DataSegment.h"
#include "Designator.h"
#include "Ecl.h"


namespace Qrio {
    /*
     * DataAnalyzer: 1.5.4
     *
     * Divides the given data string into DataSegments in the most optimal way.
     * The optimization is based on Annex J of ISO/IEC 18004:2015 page 99.
     * Responsible for Step 1 of the encoding procedure.
     */
    class DataAnalyzer final: public std::vector<DataSegment> {
    public:
        /* Based on table 9 page 38 */
        const int EccPerBlock[4][41] = {
                // Version: (note that index 0 is for padding, and is set to an illegal value)
                {-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24,
                 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26,
                 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
                 30, 30},  // Low

                {-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22,
                 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28,
                 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
                 28, 28},  // Medium

                {-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26,
                 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30,
                 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
                 30, 30},  // Quartile

                {-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28,
                 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30,
                 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
                 30, 30},  // High
        };

        /* Based on table 9 page 38 */
        const int NumberOfEccBlocks[4][41] = {
                // Version: (note that index 0 is for padding, and is set to an illegal value)
                {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,
                  4, 4, 4, 4, 6, 6, 6, 6, 7, 8, 8,
                  9, 9, 10, 12, 12, 12, 13, 14, 15,
                  16, 17, 18, 19, 19, 20, 21, 22, 24,
                  25},  // Low

                {-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,
                  5, 8, 9, 9, 10, 10, 11, 13, 14,
                  16, 17, 17, 18, 20, 21, 23, 25,
                  26, 28, 29, 31, 33, 35, 37, 38,
                  40, 43, 45, 47, 49},  // Medium

                {-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,
                  8, 10, 12, 16, 12, 17, 16, 18, 21,
                  20, 23, 23, 25, 27, 29, 34, 34, 35,
                  38, 40, 43, 45, 48, 51, 53, 56, 59,
                  62, 65, 68},  // Quartile

                {-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8,
                  11, 11, 16, 16, 18, 16, 19, 21, 25,
                  25, 25, 34, 30, 32, 35, 37, 40,
                  42, 45, 48, 51, 54, 57, 60, 63,
                  66, 70, 74, 77, 81},  // High
        };

        /*
         * Pre-Conditions:
         *      Data string,
         *      Version to be used.
         *
         * Post-Conditions:
         *      Segments contains optimized DataSegments,
         *      data contains copy of the given data string
         *
         *
         * Initializes the data members.
         * Fills the segments with the optimal DataSegments.
         */
        explicit DataAnalyzer(std::wstring,
                              int,
                              Ecl ecl = Ecl::L,
                              Designator override_mode = Designator::TERMINATOR,
                              std::unordered_map<size_t, int> eci = {});

        /*
         * Pre-Conditions:
         *      Data string,
         *      Version to be used.
         *
         * Post-Conditions:
         *      Segments contains optimized DataSegments,
         *      data contains copy of the given data string
         *
         *
         * Initializes the data members.
         * Fills the segments with the optimal DataSegments.
         */
        explicit DataAnalyzer(const std::string&, int,
                              Ecl ecl = Ecl::L,
                              Designator override_mode = Designator::TERMINATOR,
                              const std::unordered_map<size_t, int>& eci = {});

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the chosen QR version.
         */
        [[nodiscard]] int getVersion() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a constant reference to the data string.
         */
        [[nodiscard]] const std::wstring& getData() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the chosen ECL.
         */
        [[nodiscard]] Ecl getEcl() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the given ECI.
         */
        [[nodiscard]] std::unordered_map<size_t, int>& getEci();
    private:
        /*
         * Map of ECIs to be placed in the encoding.
         * Each key is an index, & each value is the ECI value.
         * Automatic detection of ECIs is not feasible, due to the lack of the]
         * AIM ECI standard that covers that information.
         */
        std::unordered_map<size_t, int> eci;

        /* Version of the QR code */
        int version;

        /* Stores a copy of the data string, used by segments */
        std::wstring data;

        /* Error correction level */
        Ecl ecl;

        /* Version boundaries for a QR code */
        const static int MIN_VERSION{1}, MAX_VERSION{40};

        /*
         * Pre-Conditions:
         *      A character c.
         *
         * Post-Conditions:
         *      Returns true if the given wchar_t is numeric.
         */
        [[nodiscard]] inline static bool isNumeric(wchar_t);

        /*
         * Pre-Conditions:
         *      A character c.
         *
         * Post-Conditions:
         *      Returns true if the given wchar_t is alphanumeric.
         */
        [[nodiscard]] inline static bool isAlphanumeric(wchar_t);

        /*
         * Pre-Conditions:
         *      A character c.
         *
         * Post-Conditions:
         *      Returns true if the given wchar_t is byte.
         */
        [[nodiscard]] inline static bool isByte(wchar_t);

        /*
         * Pre-Conditions:
         *      First byte of a char,
         *      Second byte of a char.
         *
         * Post-Conditions:
         *      Returns true if the given bytes of
         */
        [[nodiscard]] inline static bool isKanji(long, long);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns true if the given string consists exclusively of bytes.
         */
        [[nodiscard]] static bool isByte(const std::wstring&);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns true if the given string consists exclusively of numerics.
         */
        [[nodiscard]] static bool isNumeric(const std::wstring&);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns true if the given string consists exclusively of alphanumerics.
         */
        [[nodiscard]] static bool isAlphanumeric(const std::wstring&);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns true if the given string consists exclusively of Kanji characters.
         */
        [[nodiscard]] static bool isKanji(const std::wstring&);

        /*
         * Pre-Conditions:
         *      A character c.
         *
         * Post-Conditions:
         *      Returns true if the given wchar_t is kanji.
         */
        [[nodiscard]] static bool isKanji(wchar_t);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns the number of the numeric chars at the start
         *      of the substring
         */
        [[nodiscard]] static size_t countNumeric(const std::wstring&, size_t start = 0);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns the number of the byte chars at the start
         *      of the substring
         */
        [[nodiscard, maybe_unused]]
            static size_t countByte(const std::wstring&, size_t start = 0);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns the number of the alphanumeric chars at the start
         *      of the substring.
         */
        [[nodiscard]] static size_t countAlphanumeric(const std::wstring&, size_t start = 0);

        /*
         * Pre-Conditions:
         *      A constant reference to a string,
         *      starting index.
         *
         * Post-Conditions:
         *      Returns the number of the Kenji byte pairs at the start
         *      of the substring.
         */
        [[nodiscard]] static size_t countKanji(const std::wstring &data, size_t start = 0);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the value associated with a given version range.
         *
         * Review J.2 page 100 of the standard.
         */
        [[nodiscard]] int getVersionRange() const;

        /*
         * Pre-Conditions:
         *      A constant reference to a string.
         *
         * Post-Conditions:
         *      Returns the initial mode.
         */
        [[nodiscard]] Designator getInitialMode() const;

        /*
         * Pre-Conditions:
         *      Data initialized.
         *
         * Post-Conditions:
         *      Throws a domain exception if the version is out of bounds.
         */
        void checkVersion() const;

        /*
         * Pre-Conditions:
         *      Data initialized.
         *
         * Post-Conditions:
         *      Throws a range error if the given designator is
         *      invalid with the data.
         */
        void checkOverrideMode(Designator override_mode) const;

        /*
         * Pre-Conditions:
         *      Data string.
         *
         * Post-Conditions:
         *      Returns true if the given data can be encoded in alphanumeric mode.
         */
        [[nodiscard]] static bool isCompatibleAlphanumeric(const std::wstring&);

        /*
         * Pre-Conditions:
         *      Data string.
         *
         * Post-Conditions:
         *      Returns true if the given data can be encoded in Byte mode.
         */
        [[nodiscard]] static bool isCompatibleByte(const std::wstring&);

        /*
         * Pre-Conditions:
         *      Data string.
         *
         * Post-Conditions:
         *      Returns true if the given data can be encoded in Kanji mode.
         */
        [[nodiscard]] static bool isCompatibleKanji(const std::wstring&);
    };
}


#endif //QR_IO_DATAANALYZER_H

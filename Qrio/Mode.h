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

#ifndef QR_IO_MODE_H
#define QR_IO_MODE_H

#include "Designator.h"


namespace Qrio {
    /*
     * Mode: 1.0
     *
     * Encapsulates the following Designator enum class:
     *      Numeric, Alphanumeric, ECI, Kanji, or Byte.
     * Provides vital functionality to work with the QR char Modes.
     */
    class Mode final {
    public:
        /*
         * Pre-Conditions:
         *      Designator to be encapsulated,
         *      Designator must be either Numeric, Alphanumeric, ECI,
         *      Kanji, or Byte.
         *
         * Post-Conditions:
         *      Mode instance is created, mode has the given Designator value.
         */
        explicit Mode(Designator);

        /*
         * Pre-Conditions:
         *      mode is Designator::ECI,
         *      given ECI assignment value is in [0, 999'999].
         *
         * Post-Conditions
         *      Returns the ECI designator for the given integer.
         *      Either 8, 16, or 24.
         *
         * Based on Table 4 of ISO/IEC 18004:2015 page 24.
         */
        [[nodiscard]] int getEciDesignator(long) const;

        /*
         * Pre-Conditions:
         *      Version of the QrSymbol guaranteed in [1, 40].
         *
         * Post-Conditions:
         *      Returns the number of bits in the character count indicator
         *      for a QR code, based on the given version & mode type.
         */
        [[nodiscard]] int getCountBitLength(int) const;

    private:
        /*
         * Table for the number of bits in character count indicator for
         * QR code.
         * Each row represents a mode indicator;
         * each column represents a range of versions.
         *
         * Based on Table 3 of ISO/IEC 18004:2015 page 23.
         */
        constexpr static int countBitLengthTable[4][3] {
            {10, 12, 14},   // Numeric
            {9, 11, 13},    // Alphanumeric
            {8, 16, 16},    // Byte
            {8, 10, 12},    // Kanji
        };

        /*
         * Designator enum value describing how the data bits are interpreted
         * Designator must be either Numeric, Alphanumeric, ECI,
         * Kanji, or Byte.
         */
        Designator mode;

        /*
         * Pre-Conditions:
         *      None
         *
         * Post-Conditions:
         *      Returns the index of the modeType in the countBitLengthTable.
         */
        [[nodiscard]] int getBitLengthIndex(int) const;
    };
}


#endif //QR_IO_MODE_H

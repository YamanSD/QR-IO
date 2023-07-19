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

#include "Designator.h"
#include "Mode.h"


namespace Qrio {
    using std::domain_error;

    /*
     * Pre-Conditions:
     *      Designator to be encapsulated.
     *
     * Post-Conditions:
     *      Mode instance is created, mode has the given Designator value.
     */
    Mode::Mode(Designator mode): mode{mode} {}

    /*
     * Pre-Conditions:
     *      mode is Designator::ECI,
     *      given ECI assignment value is in [0, 999'999].
     *
     * Post-Conditions
     *      Returns the ECI designator for the given integer.
     *      Either 8, 16, or 24 bits in length.
     *
     * Based on Table 4 of ISO/IEC 18004:2015 page 24.
     */
    long Mode::getEciDesignator(long eci_value) {
        if (eci_value < 0 or 999'999 < eci_value) {
            throw domain_error("ECI value out of bounds [0, 999'999]");
        }

        if (eci_value < 128) {
            return eci_value | (0b0L << 7);
        } else if (eci_value < 16'384) {
            return eci_value | (0b10L << 14);
        }

        return eci_value | (0b110L << 21);
    }

    /*
     * Pre-Conditions:
     *      Version of the QrSymbol guaranteed in [1, 40].
     *
     * Post-Conditions:
     *      Returns the number of bits in the character count indicator
     *      for a QR code, based on the given version & mode type.
     */
    int Mode::getCountBitLength(int version) const {
        return countBitLengthTable[getBitLengthIndex()][getVersionIndex(version)];
    }

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns the index of the modeType in the countBitLengthTable.
     */
    int Mode::getBitLengthIndex() const {
        switch (mode) {
            case Designator::NUMERIC:
                return 0;
            case Designator::ALPHANUMERIC:
                return 1;
            case Designator::BYTE:
                return 2;
            case Designator::KANJI:
                return 3;
            default:
                throw domain_error("ModeType invalid");
        }
    }

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns the index of the version in the countBitLengthTable.
     */
    int Mode::getVersionIndex(int version) {
        return version <= 9 ? 0 : (version <= 26 ? 1 : 2);
    }
}

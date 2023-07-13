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

#include "Mode.h"


namespace qr_io {
    /*
     * Pre-Conditions:
     *      ModeType to be encapsulated.
     *
     * Post-Conditions:
     *      Mode instance is created, mode has the given ModeType value.
     */
    Mode::Mode(ModeType mode): mode{mode} {}

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns a decimal int representing the mode bits.
     */
    int Mode::getModeBits() const {
        return static_cast<int>(mode);
    }

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns the index corresponding the Mode type,
     *      in the char_widths array.
     */
    int Mode::modeIndex() const {
        if (mode == ModeType::NUMERIC) {
            return 0;
        } else if (mode == ModeType::ALPHANUMERIC) {
            return 1;
        } else if (mode == ModeType::BYTE) {
            return 2;
        } else if (mode == ModeType::KANJI) {
            return 3;
        } else if (mode == ModeType::ECI) {
            return 4;
        }

        // Will never reach here.
        return -1;
    }

    /*
     * Pre-Conditions:
     *      Version number of the QR
     *
     * Post-Conditions:
     *      Returns the bit width of the character count field for a segment in
     *      this mode in a QR Code at the given version number.
     *      The result is in the range [0, 16].
     */
    int Mode::charWidth(int version) const {
        return char_widths[modeIndex()][(version + 7) / 17];
    }

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns the ModeType encapsulated by the Mode instance.
     */
    ModeType Mode::getModeType() const {
        return mode;
    }
}

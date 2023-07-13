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


namespace qr_io {
    /*
     * Enumeration class for the mode types specified by the standard.
     * Each mode type corresponds to its binary representation in the standard.
     */
    enum class ModeType {
        NUMERIC = 0b0001,
        ALPHANUMERIC = 0b0010,
        BYTE = 0b0100,
        KANJI = 0b1000,
        ECI = 0b0111,
    };

    /*
     * Encapsulates the ModeType enum class.
     * Provides vital functionality to work with the QR char Modes.
     */
    class Mode {
    public:
        /*
         * Pre-Conditions:
         *      ModeType to be encapsulated.
         *
         * Post-Conditions:
         *      Mode instance is created, mode has the given ModeType value.
         */
        Mode(ModeType);

        /*
         * Pre-Conditions:
         *      None
         *
         * Post-Conditions:
         *      Returns a decimal int representing the mode bits.
         */
        [[nodiscard]] int getModeBits() const;

        /*
         * Pre-Conditions:
         *      Version number of the QR
         *
         * Post-Conditions:
         *      Returns the bit width of the character count field for a segment in
		 *      this mode in a QR Code at the given version number.
         *      The result is in the range [0, 16].
         */
        [[nodiscard]] int charWidth(int) const;
    private:
        /*
         * Pre-Conditions:
         *      None
         *
         * Post-Conditions:
         *      Returns the index corresponding the Mode type,
         *      in the char_widths array.
         */
        [[nodiscard]] int modeIndex() const;

        /*
         * Table for mapping a version to a char width.
         * Row is decided by the ModeType, while column is decided by
         * the version.
         */
        constexpr static int char_widths[5][3] {
            {10, 12, 14},  // Numeric
            {9, 11, 13},   // Alphanumeric
            {8, 16, 16},   // Byte
            {8, 10, 12},   // Kanji
            {0, 0, 0}      // ECI
        };

        /* ModeType enum value representing the actual value of the Mode */
        ModeType mode;
    };
}


#endif //QR_IO_MODE_H

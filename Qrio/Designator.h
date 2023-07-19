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

#ifndef QR_IO_DESIGNATOR_H
#define QR_IO_DESIGNATOR_H


namespace Qrio {
    /*
     * Enumeration class for the five mode types specified by the standard &
     * the terminator sequence along with FNC1 & structured append.
     * Each mode type corresponds to its binary representation in the standard.
     *
     * Based on Table 2 of ISO/IEC 18004:2015 page 23.
     */
    enum class Designator {
        NUMERIC = 0b0001,
        ALPHANUMERIC = 0b0010,
        BYTE = 0b0100,
        KANJI = 0b1000,
        ECI = 0b0111,
        FNC1_1st = 0b0101,
        FNC1_2nd = 0b1001,
        APPEND = 0b0011,
        TERMINATOR = 0b0000,
    };
}


#endif //QR_IO_DESIGNATOR_H

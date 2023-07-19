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

#include "BitStream.h"


namespace Qrio {
    using std::domain_error, std::endl,
            std::ostream, std::to_string,
            std::wstring, std::stoi;

    /*
     * Pre-Conditions:
     *      0 <= n <= 31,
     *      value < 2 ^ n
     * Post-Conditions:
     *      Lower n bits of value appended into the buffer.
     *      Their order remains intact.
     *
     * Assertion is performed by the checkInput function.
     */
    void BitStream::appendBits(int value, size_t n) {
        /* Assert 0 <= n <= 31, value < 2 ^ n */
        checkInput(value, n);

        /* Append bit by bit from n-th bit to the 0-th bit */
        for (int i{0}; i < n; i++) {
            push_back(value & (1 << (n - i - 1)));
        }
    }

    /*
     * Checks if 0 <= n <= 31, value < 2 ^ n.
     *          If true continue.
     *          Otherwise, std::domain_error is thrown
     */
    void BitStream::checkInput(int value, size_t n) {
        /*
         * Checks if n is less than 0,
         * then if n is greater than 31,
         * then if 2 ^ n <= value
         */
        if (n < 0 or 31 < n or (1 << n) <= value) {
            throw domain_error(
                    "\n0 <= n <= 31, value < 2 ^ n;\n"
                    "but n = (" + to_string(n) + ") "
                    "& value = (" + to_string(value) + ")\n");
        }
    }

    /*
     * Pre-Conditions:
     *      0 <= n <= 31,
     *      value < 2 ^ n
     * Post-Conditions:
     *      Lower n bits of value appended into the buffer.
     *      Their order remains intact.
     */
    void BitStream::appendBits(const wstring& value, size_t n) {
        appendBits(stoi(value), n);
    }

    /*
     * Pre-Conditions:
     *      None
     * Post-Conditions:
     *      Displays the bits in the buffer, followed by a newline character.
     *
     * Mainly used for debugging purposes.
     */
    ostream& operator<<(ostream& out, const BitStream& buffer) {
        for (auto b: buffer) {
            out << b;
        }

        return out << endl;
    }
}

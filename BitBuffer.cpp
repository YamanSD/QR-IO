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

#include "BitBuffer.h"


namespace qr_io {
    using std::domain_error, std::endl,
            std::ostream, std::to_string,
            std::uint32_t;

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
    void BitBuffer::appendBits(uint32_t value, int n) {
        /* Assert 0 <= n <= 31, value < 2 ^ n */
        checkInput(value, n);

        /* Append bit by bit from n-th bit to the 0-th bit */
        for (int i{0}; i < n; i++) {
            push_back(value & (1 << (n - i)));
        }
    }

    /*
     * Checks if 0 <= n <= 31, value < 2 ^ n.
     *          If true continue.
     *          Otherwise, std::domain_error is thrown
     */
     void BitBuffer::checkInput(uint32_t value, int n) {
        /*
         * Checks if n is less than 0,
         * then if n is greater than 31,
         * then if 2 ^ n <= value (i.e. if value has a set 32nd bit)
         */
        if (n < 0 or 31 < n or value >> n) {
            throw domain_error(
                    "\n0 <= n <= 31, value < 2 ^ n;\n"
                    "but n = (" + to_string(n) + ") "
                    "& value = (" + to_string(value) + ")\n");
        }
    }

    /*
     * Pre-Conditions:
     *      None
     * Post-Conditions:
     *      Displays the bits in the buffer, followed by a newline character.
     *
     * Mainly used for debugging purposes.
     */
    ostream& operator<<(ostream& out, const BitBuffer& buffer) {
         for (auto b: buffer) {
             out << b;
         }

         return out << endl;
     }
}

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

#ifndef QR_IO_BITBUFFER_H
#define QR_IO_BITBUFFER_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

namespace qr_io {
    /*
     * BitBuffer: 1.0
     *
     * Used to store bits dynamically.
     * Inherits from a std::vector<bool>.
     *
     * Public Methods:
     *      void appendBits(std::uint32_t value, int n);
     *          Appends the lower n bits of value into the bit buffer.
     *          The order of the bits must remain intact;
     *          (i.e. append from the most major, to the least major bit)
     *          Assertions: 0 <= n <= 31, value < 2 ^ n.
     *
     * Private Methods:
     *      static void checkInput(std::uint32_t value, int n) const;
     *          Checks if 0 <= n <= 31, value < 2 ^ n.
     *          If true continue.
     *          Otherwise, std::domain_error is thrown.
     */
    class BitBuffer final: public std::vector<bool>  {
    public:
        /*
         * Pre-Conditions:
         *      0 <= n <= 31,
         *      value < 2 ^ n
         * Post-Conditions:
         *      Lower n bits of value appended into the buffer.
         *      Their order remains intact.
         */
        void appendBits(std::uint32_t, int);
    private:
        /*
         * Checks if 0 <= n <= 31, value < 2 ^ n.
         *          If true continue.
         *          Otherwise, std::domain_error is thrown
         */
        static void checkInput(std::uint32_t, int);
    };

    /*
     * Pre-Conditions:
     *      None
     * Post-Conditions:
     *      Displays the bits in the buffer, followed by a newline character.
     *
     * Mainly used for debugging purposes.
     */
    std::ostream& operator<<(std::ostream&, const BitBuffer&);
}



#endif //QR_IO_BITBUFFER_H

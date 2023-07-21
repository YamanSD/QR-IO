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

#ifndef QR_IO_ERRORCORRECTIONENCODER_H
#define QR_IO_ERRORCORRECTIONENCODER_H

#include <deque>
#include <vector>

#include "Encoder.h"


namespace Qrio {
    /*
     * ErrorCorrectionEncoder: 1.2.3
     *
     * Responsible for adding error correction bits into the
     * encoded bit stream.
     * Responsible for Step 3 of the encoding procedure.
     *
     * Check 7.5 & 7.6
     */
    class ErrorCorrectionEncoder final: public std::vector<int> {
    public:
        /* Encoder instance from the previous layer */
        Encoder encoder;

        /*
         * Pre-Conditions:
         *      Reference to the encoder.
         *
         * Post-Conditions:
         *      Error correction codewords are generated.
         */
        explicit ErrorCorrectionEncoder(Encoder&);
    private:
        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a Reed-Solomon ECC generator polynomial for the given degree.
         *
         * Implemented as a lookup table instead of an algorithm.
         */
        [[nodiscard]] std::vector<int> reedSolomonDivisor() const;

        /*
         * Pre-Conditions:
         *      Two field elements.
         *
         * Post-Conditions:
         *      Returns the product of the two given field elements modulo GF(2^8/0x11D).
         */
        [[nodiscard]] static int reedSolomonMultiply(int, int);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a new byte string representing the given data
         *      with the appropriate error correction codewords appended to it,
         *      based on this object's version and error correction level.
         */
        void appendEccAndInterleave();

        /*
         * Pre-Conditions:
         *      Data,
         *      divisor polynomials.
         *
         * Post-Conditions:
         *      Returns the Reed-Solomon error correction codeword for the
         *      given data and divisor polynomials.
         */
        [[nodiscard]] static std::deque<int> reedSolomonRemainder(
                const std::vector<int> &data,
                const std::vector<int> &divisor);
    };
}


#endif //QR_IO_ERRORCORRECTIONENCODER_H

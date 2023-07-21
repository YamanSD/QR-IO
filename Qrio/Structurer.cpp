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

#include "Structurer.h"


namespace Qrio {
    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Calculates the penalty score for the current state of the
     *      matrix.
     *
     * Check 7.8.3
     */
    long Structurer::getPenalty() {
        /* Based on Table 11 page 54 */
        static const int penalties[4]{
            3, 3, 40, 10
        };

        long result{0};

//        for (int y{0}; )

        return result;
    }

    /*
     * Pre-Conditions:
     *      Reference to the ErrorCorrectionEncoder from the previous layer,
     *      optional mask.
     *
     * Post-Conditions:
     *      Fills the QR code matrix with the data bits & other information,
     *      a 0 represents a light module, while a 1 represents a black module.
     *      Applies the mask.
     *
     * Check 7.7 -> 7.10
     */
    Structurer::Structurer(ErrorCorrectionEncoder& ec_encoder, int mask):
    ec_encoder{ec_encoder},
    SquareMatrix(ec_encoder.getMatrixSize()),  // Initialize super class
    function_modules(ec_encoder.getMatrixSize()),
    mask{mask} {

    }
}

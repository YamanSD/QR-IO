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

#ifndef QR_IO_STRUCTURER_H
#define QR_IO_STRUCTURER_H

#include "ErrorCorrectionEncoder.h"
#include "SquareMatrix.h"


namespace Qrio {
    /*
     * Structurer: 1.0
     *
     * Responsible for structuring the final message, place modules,
     * data mask, & place the format information.
     * Responsible for Steps 4, 5, 6, & 7 of the encoding procedure.
     */
    class Structurer final: public SquareMatrix {
    public:
        /*
         * Pre-Conditions:
         *      Reference to the ErrorCorrectionEncoder from the previous layer.
         *
         * Post-Conditions:
         *      Fills the QR code matrix with the data bits & other information,
         *      a 0 represents a light module, while a 1 represents a black module.
         *
         * Check 7.7
         */
        explicit Structurer(ErrorCorrectionEncoder&);
    private:
//        static constexpr int penalties[4]{
//            3, 3, 40, 10
//        };

    };
}


#endif //QR_IO_STRUCTURER_H

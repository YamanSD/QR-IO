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

#include "Encoder.h"


namespace Qrio {
    /*
     * ErrorCorrectionEncoder: 1.0
     *
     * Responsible for adding error correction bits into the
     * encoded bit stream.
     * Responsible for Step 3 of the encoding procedure.
     */
    class ErrorCorrectionEncoder final {
    public:
        /*
         * Pre-Conditions:
         *      Reference to the encoder.
         *
         * Post-Conditions:
         *      Error correction codewords are generated.
         */
        explicit ErrorCorrectionEncoder(Encoder&);
    private:
    };
}


#endif //QR_IO_ERRORCORRECTIONENCODER_H

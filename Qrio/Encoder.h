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

#ifndef QR_IO_ENCODER_H
#define QR_IO_ENCODER_H

#include "BitStream.h"
#include "DataAnalyzer.h"
#include "DataSegment.h"


namespace Qrio {
    /*
     * Encoder: 1.0
     *
     * Encodes the DataSegments in the DataAnalyzer into a BitStream.
     * Encoding is done based on section 7, Annex H,
     * & Annex I of ISO/IEC 18004:2015.
     * Responsible for Step 2 of the encoding procedure.
     */
    class Encoder final {
    public:
        /*
         * Pre-Conditions:
         *      Constant reference to DataAnalyzer of the data.
         *
         * Post-Conditions:
         *      buffer contains the encoded contents of the DataSegments
         *      in the DataAnalyzer.
         */
        explicit Encoder(const DataAnalyzer&);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a const iterator to the start of the bit stream.
         */
        [[nodiscard]] BitStream::const_iterator& begin() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a const iterator to the end of the bit stream.
         */
        [[nodiscard]] BitStream::const_iterator& end() const;

        /*
         * Pre-Conditions:
         *      Index of the bit in the stream.
         *
         * Post-Conditions:
         *      Returns a const reference to the bool at the given index.
         */
        [[nodiscard]] const bool& operator[](size_t) const;
    private:
        BitStream stream;

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream.
         */
        [[nodiscard]] static int encode(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Numeric form.
         */
        [[nodiscard]] static int encodeNumeric(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Alphanumeric form.
         */
        [[nodiscard]] static int encodeAlpha(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Byte form.
         */
        [[nodiscard]] static int encodeByte(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Kenji form.
         */
        [[nodiscard]] static int encodeKenji(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in ECI form.
         */
        [[nodiscard]] static int encodeEci(const DataSegment&);
    };
}


#endif //QR_IO_ENCODER_H

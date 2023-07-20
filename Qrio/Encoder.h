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

#include <utility>

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
    class Encoder final: public BitStream {
    public:
        /* Data analyzer from the previous layer */
        DataAnalyzer analyzer;

        /*
         * Pre-Conditions:
         *      Constant reference to DataAnalyzer of the data.
         *
         * Post-Conditions:
         *      buffer contains the encoded contents of the DataSegments
         *      in the DataAnalyzer.
         */
        explicit Encoder(DataAnalyzer&);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the number of data codewords,
         *      accounting for the error correction codewords.
         */
        [[nodiscard]] int getDataCodewordsCount() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the number of data bits that can be stored.
         */
        [[nodiscard]] int getVersionBitCount() const;
    private:
        /*
         * Used to determine whether we added the FNC1,
         * after the first mode indicator.
         */
        bool added_fnc1{false};

        /* Stores the final 8-bit codewords */
        std::vector<int> codewords;

        /*
         * Table for the number of bits in character count indicator for
         * QR code.
         * Each row represents a mode indicator;
         * each column represents a range of versions.
         *
         * Based on Table 3 of ISO/IEC 18004:2015 page 23.
         */
        constexpr static int countBitLengthTable[4][3] {
            {10, 12, 14},   // Numeric
            {9, 11, 13},    // Alphanumeric
            {8, 16, 16},    // Byte
            {8, 10, 12},    // Kanji
        };

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream.
         */
        void encode(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Numeric form.
         */
        void encodeNumeric(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Alphanumeric form.
         */
        void encodeAlpha(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Byte form.
         */
        void encodeByte(const DataSegment&);

        /*
         * Pre-Conditions:
         *      Constant reference to DataSegment.
         *
         * Post-Conditions:
         *      Encodes the contents of the DataSegment into binary form,
         *      bits are then appended to the bit stream, in Kenji form.
         */
        void encodeKanji(const DataSegment &data);

        /*
         * Pre-Conditions:
         *      None
         *
         * Post-Conditions:
         *      Returns the index of the modeType in the countBitLengthTable.
         */
        [[nodiscard]] static int getBitLengthIndex(Designator);

        /*
         * Pre-Conditions:
         *      None
         *
         * Post-Conditions:
         *      Returns the index of the version in the countBitLengthTable.
         */
        [[nodiscard]] static int getVersionIndex(int);

        /*
         * Pre-Conditions:
         *      mode is Designator::ECI,
         *      given ECI assignment value is in [0, 999'999].
         *
         * Post-Conditions
         *      Returns the ECI designator for the given integer.
         *      Either 8, 16, or 24.
         *
         * Based on Table 4 of ISO/IEC 18004:2015 page 24.
         */
        [[nodiscard]] static std::pair<long, int> getEciDesignator(long);

        /*
         * Pre-Conditions:
         *      Version of the QrSymbol guaranteed in [1, 40].
         *
         * Post-Conditions:
         *      Returns the number of bits in the character count indicator
         *      for a QR code, based on the given version & mode type.
         */
        [[nodiscard]] static int getCountBitLength(int, Designator);

        /*
         * Pre-Conditions:
         *      Constant reference to a data segment.
         *
         * Post-Conditions:
         *      Encodes the mode type and the char count into the stream,
         *      returns the size of the data.
         */
        [[nodiscard]] size_t encodeMode(const DataSegment&);

        /*
         * Pre-Conditions:
         *      An alphanumeric character.
         *
         * Post-Conditions:
         *      Returns the value of the given char.
         */
        [[nodiscard]] static int mapAlphanumericChar(wchar_t);

        /*
         * Pre-Conditions:
         *      A byte character.
         *
         * Post-Conditions:
         *      Returns the value of the given char.
         */
        [[nodiscard]] static int mapByteChar(wchar_t);

        /*
         * Pre-Conditions:
         *      Data segment reference,
         *      index to check.
         *
         * Post-Conditions:
         *      If the given index has an ECI value, encode it.
         */
        void checkEci(const DataSegment&, size_t);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Encodes the FNC1 information into the bitstream.
         */
        void encodeFnc1();

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns true if the QR symbol has FNC1.
         */
        [[nodiscard]] bool hasFnc1() const;
    };
}


#endif //QR_IO_ENCODER_H

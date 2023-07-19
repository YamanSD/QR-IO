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


#include <stdexcept>
#include <string>
#include <utility>

#include "Encoder.h"


namespace Qrio {
    using std::domain_error, std::move;

    /*
     * Pre-Conditions:
     *      Constant reference to DataAnalyzer of the data.
     *
     * Post-Conditions:
     *      buffer contains the encoded contents of the DataSegments
     *      in the DataAnalyzer.
     */
    Encoder::Encoder(DataAnalyzer& data): analyzer{data} {
        encodeNumeric(analyzer[0]);
    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream.
     */
    void Encoder::encode(const DataSegment& data) {
    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream, in Numeric form.
     *
     * Check 7.4.3
     */
    void Encoder::encodeNumeric(const DataSegment& data) {
        const auto n{data.size()};

        appendBits(data.getTypeBits(), 4);
        appendBits(static_cast<int>(n),
                   getCountBitLength(analyzer.getVersion(), data.getType())
                   );

        for (size_t i{0}; 3 <= n - i; i += 3) {
            appendBits(data.substr(i, 3), 10);
        }

        const auto rem{n % 3};

        if (rem) {
            appendBits(data.substr(n - rem, rem), rem == 2 ? 7 : 4);
        }
    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream, in Alphanumeric form.
     */
    void Encoder::encodeAlpha(const DataSegment& data) {

    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream, in Byte form.
     */
    void Encoder::encodeByte(const DataSegment &) {
    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream, in Kenji form.
     */
    void Encoder::encodeKenji(const DataSegment &) {
    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment,
     *      position of the ECI,
     *      Value of the ECI.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream, in ECI form.
     */
    void Encoder::encodeEci(const DataSegment &, size_t, int) {
    }


    /*
     * Pre-Conditions:
     *      mode is Designator::ECI,
     *      given ECI assignment value is in [0, 999'999].
     *
     * Post-Conditions
     *      Returns the ECI designator for the given integer.
     *      Either 8, 16, or 24 bits in length.
     *
     * Based on Table 4 of ISO/IEC 18004:2015 page 24.
     */
    long Encoder::getEciDesignator(long eci_value) {
        if (eci_value < 0 or 999'999 < eci_value) {
            throw domain_error("ECI value out of bounds [0, 999'999]");
        }

        if (eci_value < 128) {
            return eci_value | (0b0L << 7);
        } else if (eci_value < 16'384) {
            return eci_value | (0b10L << 14);
        }

        return eci_value | (0b110L << 21);
    }

    /*
     * Pre-Conditions:
     *      Version of the QrSymbol guaranteed in [1, 40].
     *
     * Post-Conditions:
     *      Returns the number of bits in the character count indicator
     *      for a QR code, based on the given version & mode type.
     */
    int Encoder::getCountBitLength(int version, Designator mode) {
        return countBitLengthTable[getBitLengthIndex(mode)][getVersionIndex(version)];
    }

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns the index of the modeType in the countBitLengthTable.
     */
    int Encoder::getBitLengthIndex(Designator mode) {
        switch (mode) {
            case Designator::NUMERIC:
                return 0;
            case Designator::ALPHANUMERIC:
                return 1;
            case Designator::BYTE:
                return 2;
            case Designator::KANJI:
                return 3;
            default:
                throw domain_error("ModeType invalid");
        }
    }

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns the index of the version in the countBitLengthTable.
     */
    int Encoder::getVersionIndex(int version) {
        return version <= 9 ? 0 : (version <= 26 ? 1 : 2);
    }
}

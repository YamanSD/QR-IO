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
#include <vector>
#include <utility>

#include "Encoder.h"


namespace Qrio {
    using std::domain_error, std::min, std::move, std::pair,
            std::string, std::vector, std::wstring;

    /*
     * Pre-Conditions:
     *      Constant reference to DataAnalyzer of the data.
     *
     * Post-Conditions:
     *      buffer contains the encoded contents of the DataSegments
     *      in the DataAnalyzer.
     */
    Encoder::Encoder(const DataAnalyzer& data): analyzer{data}, codewords(0) {
        if (analyzer.struct_count != -1 and analyzer.struct_id != -1) {
            appendSequenceIndicator();
            appendParityData(analyzer.getData());
        }

        for (auto& segment: data) {
            encode(segment);
        }

        const auto capacity{8 * getDataCodewordsCount()};

        appendBits(static_cast<int>(Designator::TERMINATOR),
                   min(4, static_cast<int>(capacity - size())));
        appendBits(static_cast<int>(Designator::TERMINATOR),
                   (8 - size() % 8) % 8);

        assert(size() % 8 == 0);

        // Pad with alternating bytes until data capacity is reached
        for (int padByte{0xEC}; size() < capacity; padByte ^= 0xEC ^ 0x11)
            appendBits(padByte, 8);

        // Pack bits into bytes in big endian
        codewords = vector<int>(size() / 8);

        for (size_t i{0}; i < size(); i++) {
            codewords.at(i >> 3) |= at(i) << (7 - (i & 7));
        }
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
        switch (data.getType()) {
            case Designator::NUMERIC:
                encodeNumeric(data);
                break;
            case Designator::ALPHANUMERIC:
                encodeAlpha(data);
                break;
            case Designator::BYTE:
                encodeByte(data);
                break;
            case Designator::KANJI:
                encodeKanji(data);
                break;
            default:
                throw domain_error("Invalid mode in encoder");
        }
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
        const auto n{encodeMode(data)};

        for (size_t i{0}; 3 <= n - i; i += 3) {
            checkEci(data, i);
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
     *
     * Check 7.4.4
     */
    void Encoder::encodeAlpha(const DataSegment& data) {
        const auto n{encodeMode(data)};
        int c0, c1;

        for (size_t i{0}; 2 <= n - i; i += 2) {
            checkEci(data, i);
            c0 = mapAlphanumericChar(data[i]),
            c1 = mapAlphanumericChar(data[i + 1]);

            appendBits(45 * c0 + c1, 11);
        }

        if (n % 2) {
            appendBits(mapAlphanumericChar(data[n - 1]), 6);
        }
    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream, in Byte form.
     *
     * Check 7.4.5
     */
    void Encoder::encodeByte(const DataSegment& data) {
        const auto n{encodeMode(data)};
        int mapped_value;

        for (size_t i{0}; i < n; i++) {
            checkEci(data, i);
            mapped_value = mapByteChar(data[i]);
            appendBits(mapped_value, 8);
        }
    }

    /*
     * Pre-Conditions:
     *      Constant reference to DataSegment.
     *
     * Post-Conditions:
     *      Encodes the contents of the DataSegment into binary form,
     *      bits are then appended to the bit stream, in Kenji form.
     *
     * Check 7.4.6
     */
    void Encoder::encodeKanji(const DataSegment& data) {
        const auto n{encodeMode(data)};
        wchar_t c, c0, c1;

        for (size_t i{0}; i < n; i++) {
            checkEci(data, i);
            c = data[i];

            if (0x8140 <= c and c <= 0x9FFC) {
                c -= 0x8140;
            } else if (0xE040 <= c and c <= 0xEBBF) {
                c -= 0xC140;
            } else {
                appendBits(c, 13);
                continue;
            }

            c0 = c / (16 * 16);
            c1 = c % (16 * 16);

            appendBits(c0 * 0xC0 + c1, 13);
        }
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
    pair<long, int> Encoder::getEciDesignator(long eci_value) {
        if (eci_value < 0 or 999'999 < eci_value) {
            throw domain_error("ECI value out of bounds [0, 999'999]");
        }

        if (eci_value < 128) {
            return {eci_value | (0b0L << 7), 8};
        } else if (eci_value < 16'384) {
            return {eci_value | (0b10L << 14), 16};
        }

        return {eci_value | (0b110L << 21), 24};
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

    /*
     * Pre-Conditions:
     *      Constant reference to a data segment.
     *
     * Post-Conditions:
     *      Encodes the mode type and the char count into the stream,
     *      returns the size of the data.
     */
    size_t Encoder::encodeMode(const DataSegment& data) {
        if (not added_fnc1 and hasFnc1()) {
            encodeFnc1();
            added_fnc1 = true;
        }

        appendBits(data.getTypeBits(), 4);
        appendBits(static_cast<long>(data.size()),
                   getCountBitLength(analyzer.getVersion(), data.getType())
                   );

        return data.size();
    }

    /*
     * Pre-Conditions:
     *      An alphanumeric character.
     *
     * Post-Conditions:
     *      Returns the value of the given char.
     */
    int Encoder::mapAlphanumericChar(wchar_t c) {
        /*
         * Set of alphanumeric characters, ordered by their value.
         *
         * Check table 5 page 26.
         */
        const static string alphanumeric_order{
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:"
        };

        for (int i{0}; i < alphanumeric_order.size(); i++) {
            if (alphanumeric_order[i] == c) {
                return i;
            }
        }

        // Unreachable
        return -1;
    }

    /*
     * Pre-Conditions:
     *      A byte character.
     *
     * Post-Conditions:
     *      Returns the value of the given char.
     */
    int Encoder::mapByteChar(wchar_t c) {
        /*
         * Set of byte characters, ordered by their value.
         *
         * Check table 6 page 27.
         */
        const static wstring byte_order{
            L"\u0001\u0002\u0003\u0004\u0005\u0006\u0007\u0008"
                "\u0009\u000a\u000b\u000c\u000d\u000e\u000f\u0010"
                "\u0011\u0012\u0013\u0014\u0015\u0016\u0017\u0018"
                "\u0019\u001a\u001b\u001c\u001d\u001e\u001f\u0020"
                "\u0021\u0022\u0023\u0024\u0025\u0026\u0027\u0028"
                "\u0029\u002a\u002b\u002c\u002d\u002e\u002f\u0030"
                "\u0031\u0032\u0033\u0034\u0035\u0036\u0037\u0038"
                "\u0039\u003a\u003b\u003c\u003d\u003e\u003f\u0040"
                "\u0041\u0042\u0043\u0044\u0045\u0046\u0047\u0048"
                "\u0049\u004a\u004b\u004c\u004d\u004e\u004f\u0050"
                "\u0051\u0052\u0053\u0054\u0055\u0056\u0057\u0058"
                "\u0059\u005a\u005b\u005c\u005d\u005e\u005f\u0060"
                "\u0061\u0062\u0063\u0064\u0065\u0066\u0067\u0068"
                "\u0069\u006a\u006b\u006c\u006d\u006e\u006f\u0070"
                "\u0071\u0072\u0073\u0074\u0075\u0076\u0077\u0078"
                "\u0079\u007a\u007b\u007c\u007d\u007e\u007f\u0080"
                "\u0081\u0082\u0083\u0084\u0085\u0086\u0087\u0088"
                "\u0089\u008a\u008b\u008c\u008d\u008e\u008f\u0090"
                "\u0091\u0092\u0093\u0094\u0095\u0096\u0097\u0098"
                "\u0099\u009a\u009b\u009c\u009d\u009e\u009f\u00a0"
                "\u00a1\u00a2\u00a3\u00a4\u00a5\u00a6\u00a7\u00a8"
                "\u00a9\u00aa\u00ab\u00ac\u00ad\u00ae\u00af\u00b0"
                "\u00b1\u00b2\u00b3\u00b4\u00b5\u00b6\u00b7\u00b8"
                "\u00b9\u00ba\u00bb\u00bc\u00bd\u00be\u00bf\u00c0"
                "\u00c1\u00c2\u00c3\u00c4\u00c5\u00c6\u00c7\u00c8"
                "\u00c9\u00ca\u00cb\u00cc\u00cd\u00ce\u00cf\u00d0"
                "\u00d1\u00d2\u00d3\u00d4\u00d5\u00d6\u00d7\u00d8"
                "\u00d9\u00da\u00db\u00dc\u00dd\u00de\u00df\u00e0"
                "\u00e1\u00e2\u00e3\u00e4\u00e5\u00e6\u00e7\u00e8"
                "\u00e9\u00ea\u00eb\u00ec\u00ed\u00ee\u00ef\u00f0"
                "\u00f1\u00f2\u00f3\u00f4\u00f5\u00f6\u00f7\u00f8"
                "\u00f9\u00fa\u00fb\u00fc\u00fd\u00fe\u00ff"
        };

        // Check if c is \0
        if (c == '\u0000') {
            return 0;
        }

        for (int i{0}; i < byte_order.size(); i++) {
            if (byte_order[i] == c) {
                return i + 1;
            }
        }

        // Unreachable
        return -1;
    }

    /*
     * Pre-Conditions:
     *      Data segment reference,
     *      index to check.
     *
     * Post-Conditions:
     *      If the given index has an ECI value, encode it.
     */
    void Encoder::checkEci(const DataSegment& data, size_t index) {
        index += data.getStart();
        auto eci = analyzer.getEci();

        if (eci.find(index) != eci.end()) {
            const auto& p{getEciDesignator(eci[index])};

            appendBits(static_cast<int>(Designator::ECI), 4);

            if (hasFnc1()) {
                encodeFnc1();
            }

            appendBits(p.first, p.second);
        }
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Encodes the FNC1 information into the bitstream.
     */
    void Encoder::encodeFnc1() {
        appendBits(
                static_cast<int>(analyzer.fnc1_value == 1 ? Designator::FNC1_1st
                                                   : Designator::FNC1_2nd), 4);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns true if the QR symbol has FNC1.
     */
    bool Encoder::hasFnc1() const {
        return analyzer.fnc1_value != 0;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the number of data codewords,
     *      accounting for the error correction codewords.
     */
    int Encoder::getDataCodewordsCount() const {
        return getVersionBitCount() / 8
                - analyzer.getEccPerBlock() * analyzer.getEccBlocksCount();
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the number of data bits that can be stored.
     */
    int Encoder::getVersionBitCount() const {
        const auto version{analyzer.getVersion()};

        int result = (16 * version + 128) * version + 64;

        if (2 <= version) {
            int numAlign = version / 7 + 2;
            result -= (25 * numAlign - 10) * numAlign - 55;
            if (7 <= version)
                result -= 36;
        }

        assert(208 <= result && result <= 29648);
        return result;
    }

    /*
     * Pre-Conditions:
     *      Data string.
     *
     * Post-Conditions:
     *      Generates the parity data for structured append,
     *      then appends it to the stream.
     *
     * Check 8.3
     */
    void Encoder::appendParityData(const wstring& data) {
        int result{0};

        for (auto c: data) {
            c = mapByteChar(c);

            if (DataAnalyzer::isKanji(c)) {
                result ^= c / (16 * 16);
                result ^= c % (16 * 16);
            } else {
                result ^= c;
            }
        }

        appendBits(result, 8);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Appends the structured append designator,
     *      appends the symbol sequence indicator
     *      for structured append into the stream.
     *
     * Check 8.2
     */
    void Encoder::appendSequenceIndicator() {
        appendBits(static_cast<int>(Designator::APPEND), 4);
        appendBits(analyzer.struct_id, 4);
        appendBits(analyzer.struct_count, 4);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      None.
     *
     * Default constructor used temporarily by the QrCode class.
     */
    Encoder::Encoder() = default;
}

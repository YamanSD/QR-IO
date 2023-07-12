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

#ifndef QR_IO_QRCODE_H
#define QR_IO_QRCODE_H

#include <array>
#include <string>
#include <vector>

#include "QrSegment.h"


namespace qr_io {
    typedef std::vector<std::vector<bool>> matrix;

    /*
     * Error correction levels.
     *
     * L: Low,
     * M: Medium,
     * Q: Quartile,
     * H: High.
     *
     * Based on the levels in the standard.
     */
    enum class Ecc {
        L = 0,
        M = 1,
        Q = 2,
        H = 3,
    };

    /*
     * A QR code symbol.
     * Based on the description of ISO/IEC 18004 standard.
     * Instances represent a square grid of dark and light cells.
     * The class covers the QR Code Model 2 specification, supporting all versions (sizes)
     * from 1 to 40, all 4 error correction levels, and 4 character encoding modes.
     */
    class QrCode final {
    public:
        [[nodiscard]] bool getModule(int, int) const;

        [[nodiscard]] int getMask() const;

        [[nodiscard]] Ecc getErrorCorrectionLevel() const;

        [[nodiscard]] int getSize() const;

        [[nodiscard]] int getVersion() const;

        QrCode(int, Ecc, const std::vector<int>&, int);

        QrCode(const std::vector<QrSegment>&, Ecc,
               int minVersion = 1,
               int maxVersion = 40,
               int mask = -1,
               bool boostEcl = true);

        QrCode(const std::vector<int>&, Ecc);

        QrCode(const std::string&, Ecc);

        [[nodiscard]] int getFormatBits(Ecc);

    private:
        static const int N1_PENALTY{3}, N2_PENALTY{3},
                            N3_PENALTY{40}, N4_PENALTY{10};

        static const int MIN_VERSION{1}, MAX_VERSION{40};

        Ecc ecc_level;
        int mask;
        int size;
        int version;
        matrix modules;
        matrix isFunction;

        [[nodiscard]] int getEccNumber() const;

        [[nodiscard]] int getEccsPerBlock(int) const;

        [[nodiscard]] int getErrCorrectionPerBlock(int) const;

        [[nodiscard]] bool getBit(long n, int i) const;

        void finderPenaltyAddHistory(int, std::array<int, 7>&) const;

        [[nodiscard]] int finderPenaltyTerminateAndCount(bool, int, std::array<int, 7>&) const;

        [[nodiscard]] int finderPenaltyCountPatterns(const std::array<int, 7>&) const;

        [[nodiscard]] int reedSolomonMultiply(int x, int y);

        [[nodiscard]] std::vector<int> reedSolomonComputeRemainder(const std::vector<int>&,
                                                                    const std::vector<int>&);

        [[nodiscard]] std::vector<int> reedSolomonComputeDivisor(int);

        [[nodiscard]] int getNumDataCodewords(int, Ecc);

        [[nodiscard]] int getNumRawDataModules(int);

        [[nodiscard]] std::vector<int> getAlignmentPatternPositions() const;

        [[nodiscard]] long getPenaltyScore() const;

        void applyMask(int);

        void drawCodewords(const std::vector<int>&);

        [[nodiscard]] std::vector<int> addEccAndInterleave(const std::vector<int>&) const;

        [[nodiscard]] bool module(int, int) const;

        void setFunctionModule(int, int, bool);

        void drawAlignmentPattern(int, int);

        void drawFinderPattern(int, int);

        void drawVersion();

        void drawFormatBits(int);

        void drawFunctionPatterns();
    };
}


#endif //QR_IO_QRCODE_H

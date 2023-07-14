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

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "QrCode.h"


namespace qr_io {
    using std::domain_error, std::length_error,
            std::string, std::vector, std::to_string,
            std::min;

    int QrCode::getFormatBits() const {
        return (int)ecc_level;
    }

    QrCode::QrCode(int version, Ecc level, const vector<int>& data, int mask):
    version{version}, ecc_level{level}, mask{mask},
    size{4 * version + 17}, modules(size), isFunction(size) {
        checkVersion();
        checkMask();

        drawFunctionPatterns();
        drawCodewords(addEccAndInterleave(data));

        if (mask == -1) {
            long minPenalty{LONG_MAX}, penalty;

            for (int i{0}; i < 8; i++) {
                applyMask(i);
                drawFormatBits(i);

                penalty = getPenaltyScore();

                if (penalty < minPenalty) {
                    mask = i;
                    minPenalty = penalty;
                }

                applyMask(i);
            }

            this->mask = mask;
            applyMask(mask);
            drawFormatBits(mask);
        }

        isFunction.clear();
        isFunction.shrink_to_fit();
    }

    QrCode::QrCode(const vector<QrSegment>& data, Ecc level,
                   int minVersion, int maxVersion,
                   int mask, bool boostEcl): mask{mask}, ecc_level{level},
                   size{-1}, version{-1} {
        checkVersionBounds(minVersion, maxVersion);
        checkMask();

        long dataUsedBits, dataCapacityBits;

        for (int v{minVersion}; v <= maxVersion; v++) {
            version = v;
            dataCapacityBits = 8 * getNumDataCodewords(v, level);
            dataUsedBits = QrSegment::getTotalBits(data, v);

            if (dataUsedBits != -1 and dataUsedBits <= dataCapacityBits) {
                break;
            } else if (v >= maxVersion) {
                throw length_error("\nSegment too long\n"
                                   "Data length " + to_string(dataUsedBits) + " bits,\n"
                                   "Max capacity " + to_string(dataCapacityBits) + " bits\n");
            }
        }

        // Increase the error correction level while the data still fits in the current version number
        for (Ecc newEcl : {Ecc::M, Ecc::Q, Ecc::H}) {  // From low to high
            if (boostEcl
                and dataUsedBits <= 8 * getNumDataCodewords(version, newEcl)) {
                ecc_level = newEcl;
            }
        }

        BitBuffer buffer{};

        for (const QrSegment& s: data) {
            buffer.appendBits(s.getMode().getModeBits(), 4);
            buffer.appendBits(s.getSize(), s.getMode().charWidth(version));
            buffer.insert(buffer.end(), s.getData().begin(), s.getData().end());
        }

        if (buffer.size() != dataUsedBits) {
            throw domain_error("Buffer size not equal to data bits size");
        }

        dataCapacityBits = 8 * getNumDataCodewords(version, ecc_level);

        if (dataCapacityBits < buffer.size()) {
            throw domain_error("Data bits less than buffer size");
        }

        buffer.appendBits(0, min(4, static_cast<int>(dataCapacityBits - buffer.size())));
        buffer.appendBits(0, (8 - static_cast<int>(buffer.size() % 8)) % 8);

        if (buffer.size() % 8) {
            throw domain_error("Buffer size not divisible by 8");
        }

        for (int pad{0xEC}; buffer.size() < dataCapacityBits; pad ^= 0xFD) {
            buffer.appendBits(pad, 8);
        }

        vector<int> dataCodewords(buffer.size() / 8);

        for (size_t i{0}; i < buffer.size(); i++) {
            dataCodewords.at(i >> 3) |= buffer.at(i) << (7 - (i & 7));
        }

        *this = QrCode(version, ecc_level, dataCodewords, mask);
    }

    QrCode::QrCode(const vector<int>& data, Ecc level):
            QrCode({QrSegment(data)}, level) {}

    QrCode::QrCode(const string& data, Ecc level):
            QrCode(makeSegments(data), level) {}

    void QrCode::checkVersion() const {
        if (version < MIN_VERSION or MAX_VERSION < version) {
            throw domain_error("Version out of range [1, 40]");
        }
    }

    void QrCode::checkMask() const {
        if (mask < -1 or 7 < mask) {
            throw domain_error("Mask value out of range [-1, 7]");
        }
    }

    void QrCode::checkVersionBounds(int min_version, int max_version) {
        if (min_version < MIN_VERSION
            or MAX_VERSION < min_version
            or MAX_VERSION < max_version) {
            throw domain_error("Invalid version boundaries");
        }
    }

    int QrCode::getEccIndex(Ecc ecc_level) {
        switch (ecc_level) {
            case (Ecc::L):
                return 0;
            case (Ecc::M):
                return 1;
            case (Ecc::Q):
                return 2;
            case (Ecc::H):
                return 3;
        }
    }

    int QrCode::getEccIndex() const {
        return getEccIndex(ecc_level);
    }

    bool QrCode::getBit(long n, int i) {
        return ((n >> i) & i);
    }

    int QrCode::getVersion() const {
        return version;
    }

    int QrCode::getSize() const {
        return size;
    }

    Ecc QrCode::getErrorCorrectionLevel() const {
        return ecc_level;
    }

    int QrCode::getMask() const {
        return mask;
    }

    bool QrCode::getModule(int x, int y) const {
        return 0 <= x and x < getSize() and 0 <= y and y < getSize() and module(x, y);
    }

    bool QrCode::module(int x, int y) const {
        return modules.at(y).at(x);
    }

    int QrCode::getEccsPerBlock(int v, Ecc ecc) {
        const static int table[4][41] {
            {-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22,
             24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30,
             30, 30, 30, 30, 30, 30, 30, 30, 30},  // Low

            {-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24,
             28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
             28, 28, 28, 28, 28, 28, 28, 28, 28},  // Medium

            {-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30,
             24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30,
             30, 30, 30, 30, 30, 30, 30, 30, 30},  // Quartile

            {-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24,
             30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30,
             30, 30, 30, 30, 30, 30, 30, 30, 30},  // High
        };

        return table[getEccIndex(ecc)][v];
    }

    int QrCode::getEccsPerBlock(int v) const {
        return getEccsPerBlock(v, ecc_level);
    }

    int QrCode::getErrCorrectionPerBlock(int v, Ecc ecc) {
        const static int table[4][41] {
            {-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 4, 6, 6, 6, 6, 7,
             8, 8, 9, 9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19,
             20, 21, 22, 24, 25},  // Low

            {-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5, 5, 8, 9, 9, 10, 10, 11,
             13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33,
             35, 37, 38, 40, 43, 45, 47, 49},  // Medium

            {-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8, 8, 10, 12, 16, 12, 17, 16,
             18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45,
             48, 51, 53, 56, 59, 62, 65, 68},  // Quartile

            {-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16,
             19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51,
             54, 57, 60, 63, 66, 70, 74, 77, 81},  // High
        };

        return table[getEccIndex(ecc)][v];
    }

    int QrCode::getErrCorrectionPerBlock(int v) const {
        return getErrCorrectionPerBlock(v, ecc_level);
    }

    vector<QrSegment> QrCode::makeSegments(const string& data) {
        if (data.empty()) {
            return {};
        }

        return {QrSegment(data)};
    }

    int QrCode::getEccNumber() const {
        return (int)ecc_level;
    }

    void QrCode::drawFunctionPatterns() {
        for (int i{0}; i < size; i++) {
            setFunctionModule(6, i, i % 2 == 0);
            setFunctionModule(i, 6, i % 2 == 0);
        }

        drawFinderPattern(3, 3);
        drawFinderPattern(size - 4, 3);
        drawFinderPattern(3, size - 4);

        const auto& alignPatPos{getAlignmentPatternPositions()};
        const auto numAlign{alignPatPos.size()};

        for (size_t i{0}; i < numAlign; i++) {
            for (size_t j{0}; j < numAlign; j++) {
                if ((i or (j and j != numAlign - 1)) and (j or i != numAlign - 1)) {
                    drawAlignmentPattern(alignPatPos.at(i), alignPatPos.at(j));
                }
            }
        }

        drawFormatBits(0);
        drawVersion();
    }

    void QrCode::drawFormatBits(int m) {
        // Calculate error correction code and pack bits
        int data = getFormatBits() << 3 | m;  // errCorrLvl is uint2, msk is uint3
        int rem = data;
        for (int i = 0; i < 10; i++)
            rem = (rem << 1) ^ ((rem >> 9) * 0x537);
        int bits = (data << 10 | rem) ^ 0x5412;  // uint15
        assert(bits >> 15 == 0);

        // Draw first copy
        for (int i = 0; i <= 5; i++)
            setFunctionModule(8, i, getBit(bits, i));
        setFunctionModule(8, 7, getBit(bits, 6));
        setFunctionModule(8, 8, getBit(bits, 7));
        setFunctionModule(7, 8, getBit(bits, 8));
        for (int i = 9; i < 15; i++)
            setFunctionModule(14 - i, 8, getBit(bits, i));

        // Draw second copy
        for (int i = 0; i < 8; i++)
            setFunctionModule(size - 1 - i, 8, getBit(bits, i));
        for (int i = 8; i < 15; i++)
            setFunctionModule(8, size - 15 + i, getBit(bits, i));
        setFunctionModule(8, size - 8, true);  // Always dark
    }

    void QrCode::drawVersion() {
        if (version < 7)
            return;

        // Calculate error correction code and pack bits
        int rem = version;  // version is uint6, in the range [7, 40]
        for (int i = 0; i < 12; i++)
            rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
        long bits = static_cast<long>(version) << 12 | rem;  // uint18
        assert(bits >> 18 == 0);

        // Draw two copies
        for (int i = 0; i < 18; i++) {
            bool bit = getBit(bits, i);
            int a = size - 11 + i % 3;
            int b = i / 3;
            setFunctionModule(a, b, bit);
            setFunctionModule(b, a, bit);
        }
    }

    void QrCode::drawFinderPattern(int x, int y) {
        for (int dy = -4; dy <= 4; dy++) {
            for (int dx = -4; dx <= 4; dx++) {
                int dist = std::max(std::abs(dx), std::abs(dy));  // Chebyshev/infinity norm
                int xx = x + dx, yy = y + dy;
                if (0 <= xx && xx < size && 0 <= yy && yy < size)
                    setFunctionModule(xx, yy, dist != 2 && dist != 4);
            }
        }
    }

    void QrCode::drawAlignmentPattern(int x, int y) {
        for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++)
                setFunctionModule(x + dx, y + dy, std::max(std::abs(dx), std::abs(dy)) != 1);
        }
    }

    void QrCode::setFunctionModule(int x, int y, bool isDark) {
        size_t ux = static_cast<size_t>(x);
        size_t uy = static_cast<size_t>(y);
        modules.at(uy).at(ux) = isDark;
        isFunction.at(uy).at(ux) = true;
    }

    vector<int> QrCode::addEccAndInterleave(const vector<int> &data) const {
        if (data.size() != static_cast<unsigned int>(getNumDataCodewords(version)))
            throw std::invalid_argument("Invalid argument");

        // Calculate parameter numbers
        int numBlocks = getErrCorrectionPerBlock(version);
        int blockEccLen = getEccsPerBlock(version);
        int rawCodewords = getNumRawDataModules(version) / 8;
        int numShortBlocks = numBlocks - rawCodewords % numBlocks;
        int shortBlockLen = rawCodewords / numBlocks;

        // Split data into blocks and append ECC to each block
        vector<vector<int> > blocks;
        const vector<int> rsDiv = reedSolomonComputeDivisor(blockEccLen);
        for (int i = 0, k = 0; i < numBlocks; i++) {
            vector<int> dat(data.cbegin() + k, data.cbegin() + (k + shortBlockLen - blockEccLen + (i < numShortBlocks ? 0 : 1)));
            k += static_cast<int>(dat.size());
            const vector<int> ecc = reedSolomonComputeRemainder(dat, rsDiv);
            if (i < numShortBlocks)
                dat.push_back(0);
            dat.insert(dat.end(), ecc.cbegin(), ecc.cend());
            blocks.push_back(std::move(dat));
        }

        // Interleave (not concatenate) the bytes from every block into a single sequence
        vector<int> result;
        for (size_t i = 0; i < blocks.at(0).size(); i++) {
            for (size_t j = 0; j < blocks.size(); j++) {
                // Skip the padding byte in short blocks
                if (i != static_cast<unsigned int>(shortBlockLen - blockEccLen) || j >= static_cast<unsigned int>(numShortBlocks))
                    result.push_back(blocks.at(j).at(i));
            }
        }
        assert(result.size() == static_cast<unsigned int>(rawCodewords));
        return result;
    }

    void QrCode::drawCodewords(const vector<int> &data) {
        if (data.size() != static_cast<unsigned int>(getNumRawDataModules(version) / 8))
            throw std::invalid_argument("Invalid argument");

        size_t i = 0;  // Bit index into the data
        // Do the funny zigzag scan
        for (int right = size - 1; right >= 1; right -= 2) {  // Index of right column in each column pair
            if (right == 6)
                right = 5;
            for (int vert = 0; vert < size; vert++) {  // Vertical counter
                for (int j = 0; j < 2; j++) {
                    size_t x = static_cast<size_t>(right - j);  // Actual x coordinate
                    bool upward = ((right + 1) & 2) == 0;
                    size_t y = static_cast<size_t>(upward ? size - 1 - vert : vert);  // Actual y coordinate
                    if (!isFunction.at(y).at(x) && i < data.size() * 8) {
                        modules.at(y).at(x) = getBit(data.at(i >> 3), 7 - static_cast<int>(i & 7));
                        i++;
                    }
                    // If this QR Code has any remainder bits (0 to 7), they were assigned as
                    // 0/false/light by the constructor and are left unchanged by this method
                }
            }
        }
        assert(i == data.size() * 8);
    }

    void QrCode::applyMask(int msk) {
        if (msk < 0 || msk > 7)
            throw std::domain_error("Mask value out of range");
        size_t sz = static_cast<size_t>(size);
        for (size_t y = 0; y < sz; y++) {
            for (size_t x = 0; x < sz; x++) {
                bool invert;
                switch (msk) {
                    case 0:  invert = (x + y) % 2 == 0;                    break;
                    case 1:  invert = y % 2 == 0;                          break;
                    case 2:  invert = x % 3 == 0;                          break;
                    case 3:  invert = (x + y) % 3 == 0;                    break;
                    case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
                    case 5:  invert = x * y % 2 + x * y % 3 == 0;          break;
                    case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
                    case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
                    default:  throw std::logic_error("Unreachable");
                }
                modules.at(y).at(x) = modules.at(y).at(x) ^ (invert & !isFunction.at(y).at(x));
            }
        }
    }

    long QrCode::getPenaltyScore() const {
        long result = 0;

        // Adjacent modules in row having same color, and finder-like patterns
        for (int y = 0; y < size; y++) {
            bool runColor = false;
            int runX = 0;
            std::array<int,7> runHistory = {};
            for (int x = 0; x < size; x++) {
                if (module(x, y) == runColor) {
                    runX++;
                    if (runX == 5)
                        result += PENALTY_N1;
                    else if (runX > 5)
                        result++;
                } else {
                    finderPenaltyAddHistory(runX, runHistory);
                    if (!runColor)
                        result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
                    runColor = module(x, y);
                    runX = 1;
                }
            }
            result += finderPenaltyTerminateAndCount(runColor, runX, runHistory) * PENALTY_N3;
        }
        // Adjacent modules in column having same color, and finder-like patterns
        for (int x = 0; x < size; x++) {
            bool runColor = false;
            int runY = 0;
            std::array<int,7> runHistory = {};
            for (int y = 0; y < size; y++) {
                if (module(x, y) == runColor) {
                    runY++;
                    if (runY == 5)
                        result += PENALTY_N1;
                    else if (runY > 5)
                        result++;
                } else {
                    finderPenaltyAddHistory(runY, runHistory);
                    if (!runColor)
                        result += finderPenaltyCountPatterns(runHistory) * PENALTY_N3;
                    runColor = module(x, y);
                    runY = 1;
                }
            }
            result += finderPenaltyTerminateAndCount(runColor, runY, runHistory) * PENALTY_N3;
        }

        // 2*2 blocks of modules having same color
        for (int y = 0; y < size - 1; y++) {
            for (int x = 0; x < size - 1; x++) {
                bool  color = module(x, y);
                if (  color == module(x + 1, y) &&
                color == module(x, y + 1) &&
                color == module(x + 1, y + 1))
                    result += PENALTY_N2;
            }
        }

        // Balance of dark and light modules
        int dark = 0;
        for (const vector<bool> &row : modules) {
            for (bool color : row) {
                if (color)
                    dark++;
            }
        }
        int total = size * size;  // Note that size is odd, so dark/total != 1/2
        // Compute the smallest integer k >= 0 such that (45-5k)% <= dark/total <= (55+5k)%
        int k = static_cast<int>((std::abs(dark * 20L - total * 10L) + total - 1) / total) - 1;
        assert(0 <= k && k <= 9);
        result += k * PENALTY_N4;
        assert(0 <= result && result <= 2568888L);  // Non-tight upper bound based on default values of PENALTY_N1, ..., N4
        return result;
    }

    vector<int> QrCode::getAlignmentPatternPositions() const {
        if (version == 1)
            return vector<int>();
        else {
            int numAlign = version / 7 + 2;
            int step = (version == 32) ? 26 :
                    (version * 4 + numAlign * 2 + 1) / (numAlign * 2 - 2) * 2;
            vector<int> result;
            for (int i = 0, pos = size - 7; i < numAlign - 1; i++, pos -= step)
                result.insert(result.begin(), pos);
            result.insert(result.begin(), 6);
            return result;
        }
    }

    int QrCode::getNumRawDataModules(int ver) {
        if (ver < MIN_VERSION || ver > MAX_VERSION)
            throw std::domain_error("Version number out of range");
        int result = (16 * ver + 128) * ver + 64;
        if (ver >= 2) {
            int numAlign = ver / 7 + 2;
            result -= (25 * numAlign - 10) * numAlign - 55;
            if (ver >= 7)
                result -= 36;
        }
        assert(208 <= result && result <= 29648);
        return result;
    }

    int QrCode::getNumDataCodewords(int ver, Ecc ecc) {
        return getNumRawDataModules(ver) / 8
        - getEccsPerBlock(ver, ecc)
        * getErrCorrectionPerBlock(ver, ecc);
    }

    vector<int> QrCode::reedSolomonComputeDivisor(int degree) {
        if (degree < 1 || degree > 255)
            throw std::domain_error("Degree out of range");
        // Polynomial coefficients are stored from highest to lowest power, excluding the leading term which is always 1.
        // For example the polynomial x^3 + 255x^2 + 8x + 93 is stored as the uint8 array {255, 8, 93}.
        vector<int> result(static_cast<size_t>(degree));
        result.at(result.size() - 1) = 1;  // Start off with the monomial x^0

        // Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
        // and drop the highest monomial term which is always 1x^degree.
        // Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
        int root = 1;
        for (int i = 0; i < degree; i++) {
            // Multiply the current product by (x - r^i)
            for (size_t j = 0; j < result.size(); j++) {
                result.at(j) = reedSolomonMultiply(result.at(j), root);
                if (j + 1 < result.size())
                    result.at(j) ^= result.at(j + 1);
            }
            root = reedSolomonMultiply(root, 0x02);
        }
        return result;
    }

    int QrCode::reedSolomonMultiply(int x, int y) {
        // Russian peasant multiplication
        int z = 0;
        for (int i = 7; i >= 0; i--) {
            z = (z << 1) ^ ((z >> 7) * 0x11D);
            z ^= ((y >> i) & 1) * x;
        }
        assert(z >> 8 == 0);
        return static_cast<int>(z);
    }

    int QrCode::finderPenaltyCountPatterns(const std::array<int, 7>& runHistory) const {
        int n = runHistory.at(1);
        assert(n <= size * 3);
        bool core = n > 0 && runHistory.at(2) == n && runHistory.at(3) == n * 3 && runHistory.at(4) == n && runHistory.at(5) == n;
        return (core && runHistory.at(0) >= n * 4 && runHistory.at(6) >= n ? 1 : 0)
        + (core && runHistory.at(6) >= n * 4 && runHistory.at(0) >= n ? 1 : 0);
    }

    int QrCode::finderPenaltyTerminateAndCount(bool currentRunColor, int currentRunLength, std::array<int,7> &runHistory) const {
        if (currentRunColor) {  // Terminate dark run
            finderPenaltyAddHistory(currentRunLength, runHistory);
            currentRunLength = 0;
        }
        currentRunLength += size;  // Add light border to final run
        finderPenaltyAddHistory(currentRunLength, runHistory);
        return finderPenaltyCountPatterns(runHistory);
    }

    void QrCode::finderPenaltyAddHistory(int currentRunLength, std::array<int,7> &runHistory) const {
        if (runHistory.at(0) == 0)
            currentRunLength += size;  // Add light border to initial run
            std::copy_backward(runHistory.cbegin(), runHistory.cend() - 1, runHistory.end());
            runHistory.at(0) = currentRunLength;
    }

    int QrCode::getNumDataCodewords(int v) const {
        return getNumDataCodewords(v, ecc_level);
    }

    vector<int> QrCode::reedSolomonComputeRemainder(const vector<int> &data, const vector<int> &divisor) {
        vector<int> result(divisor.size());
        for (int b : data) {  // Polynomial division
            int factor = b ^ result.at(0);
            result.erase(result.begin());
            result.push_back(0);
            for (size_t i = 0; i < result.size(); i++)
                result.at(i) ^= reedSolomonMultiply(divisor.at(i), factor);
        }
        return result;
    }
}

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

#include <deque>
#include <utility>
#include <stdexcept>
#include <vector>

#include "ErrorCorrectionEncoder.h"


namespace Qrio {
    using std::deque, std::move,
            std::vector, std::domain_error;

    ErrorCorrectionEncoder::ErrorCorrectionEncoder(Encoder& encoder):
    encoder{encoder} {
        assert(encoder.codewords.size() == encoder.getDataCodewordsCount());
        appendEccAndInterleave();
    }

    /*
     * Pre-Conditions:
     *      None
     *
     * Post-Conditions:
     *      Returns a Reed-Solomon ECC generator polynomial for the given degree.
     *
     * Implemented as a lookup table instead of an algorithm.
     */
    vector<int> ErrorCorrectionEncoder::reedSolomonDivisor() const {
        const auto degree{encoder.analyzer.getEccPerBlock()};

        if (degree < 1 or 255 < degree) {
            throw domain_error("Degree out of bounds");
        }

        /*
         * Polynomial coefficients are stored from highest to lowest power, excluding the leading term which is always 1.
         * For example the polynomial x^3 + 255x^2 + 8x + 93 is stored as the uint8 array {255, 8, 93}.
         */
        vector<int> result(degree);

        /* Start off with the monomial x^0 */
        result.back() = 1;

        /* Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
         * and drop the highest monomial term which is always 1x^degree.
         * Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
         */
        int root{1};

        for (int i{0}; i < degree; i++) {
            // Multiply the current product by (x - r^i)
            for (size_t j{0}; j < degree; j++) {
                result[j] = reedSolomonMultiply(result[j], root);

                if (j < degree - 1) {
                    result[j] ^= result[j + 1];
                }
            }

            root = reedSolomonMultiply(root, 0x02);
        }

        return result;
    }

    /*
     * Pre-Conditions:
     *      Two field elements.
     *
     * Post-Conditions:
     *      Returns the product of the two given field elements modulo GF(2^8/0x11D).
     */
    int ErrorCorrectionEncoder::reedSolomonMultiply(int n0, int n1) {
        /* Russian peasant multiplication */
        int result{0};

        for (int i{7}; 0 <= i; i--) {
            result = (result << 1) ^ (0x11D * (result >> 7));
            result ^= n0 * ((n1 >> i) & 1);
        }

        assert(result >> 8 == 0);
        return result;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns a new byte string representing the given data with the appropriate error correction
	 *      codewords appended to it, based on this object's version and error correction level.
     */
    void ErrorCorrectionEncoder::appendEccAndInterleave() {
        const int blocksCount{encoder.analyzer.getEccBlocksCount()},
                    eccPerBlock{encoder.analyzer.getEccPerBlock()},
                    bitCount{encoder.getVersionBitCount()};

        const int shortBlocksCount{blocksCount - bitCount % blocksCount},
                    shortBlocksLength{bitCount / blocksCount};

        /* Data codewords */
        const auto& data{encoder.codewords};

        vector<vector<int>> blocks{};
        vector<int> block{};
        deque<int> ecc;

        const auto& solomonDiv = reedSolomonDivisor();

        for (int i{0}, k{0}; i < blocksCount; i++) {
            block = vector<int>(data.cbegin() + k,
                                data.cbegin()
                                + (k + shortBlocksLength - eccPerBlock
                                    + (i < shortBlocksCount ? 0 : 1)));

            k += static_cast<int>(block.size());
            ecc = reedSolomonRemainder(block, solomonDiv);

            if (i < shortBlocksCount) {
                block.push_back(0);
            }

            block.insert(block.end(), ecc.cbegin(), ecc.cend());
            blocks.push_back(move(block));
        }

        /* Interleave the bytes from every block into a single bit sequence */
        const auto width{blocks.at(0).size()};

        for (size_t i{0}; i < width; i++) {
            for (size_t j{0}; j < blocksCount; j++) {
                /* Skip the padding byte in short blocks */
                if (i != static_cast<unsigned>(shortBlocksLength - eccPerBlock)
                    or static_cast<unsigned>(shortBlocksCount) <= j) {
                    push_back(blocks.at(j).at(i));
                }
            }
        }

        assert(size() == static_cast<unsigned>(bitCount));
    }

    /*
     * Pre-Conditions:
     *      Data,
     *      divisor polynomials.
     *
     * Post-Conditions:
     *      Returns the Reed-Solomon error correction codeword for the
     *      given data and divisor polynomials.
     */
    deque<int> ErrorCorrectionEncoder::reedSolomonRemainder(
            const vector<int> &data,
            const vector<int> &divisor) {
        deque<int> result(divisor.size());
        int factor;

        /* Perform polynomial division */
        for (auto b: data) {
            factor = b ^ result.front();

            result.pop_front();
            result.push_back(0);

            for (size_t i{0}; i < result.size(); i++) {
                result[i] ^= reedSolomonMultiply(divisor[i], factor);
            }
        }

        return result;
    }
}

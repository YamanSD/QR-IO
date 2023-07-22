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


#include <algorithm>
#include <array>
#include <deque>
#include <functional>
#include <utility>
#include <stdexcept>
#include <vector>

#include "Structurer.h"


namespace Qrio {
    using std::abs, std::array, std::copy_backward,
            std::deque, std::domain_error, std::function,
            std::max, std::vector;

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
        bool color;
        int dark_counter{0}, x_cycles, y_cycles;
        array<int, 7> run_history{};

        /* Adjacent modules in row having same color, and finder-like patterns */
        for (size_t y{0}; y < size(); y++) {
            color = false;
            x_cycles = 0;
            run_history = {};

            for (size_t x{0}; x < size(); x++) {
                if (module(x, y) == color) {
                    x_cycles++;

                    if (x_cycles == 5) {
                        result += penalties[0];
                    } else if (5 < x_cycles) {
                        result++;
                    }
                } else {
                    finderPenaltyAddHistory(x_cycles, run_history);

                    if (not color) {
                        result += penalties[2] * finderPenaltyCountPatterns(run_history);
                    }

                    color = module(x, y);
                    x_cycles = 1;
                }
            }

            result += penalties[2] * finderPenaltyTerminateAndCount(color, x_cycles, run_history);
        }

        /* Adjacent modules in column having same color, and finder-like patterns */
        for (size_t x{0}; x < size(); x++) {
            color = false;
            y_cycles = 0;
            run_history = {};

            for (size_t y{0}; y < size(); y++) {
                if (module(x, y) == color) {
                    y_cycles++;

                    if (y_cycles == 5) {
                        result += penalties[0];
                    } else if (5 < y_cycles) {
                        result++;
                    }
                } else {
                    finderPenaltyAddHistory(y_cycles, run_history);

                    if (not color) {
                        result += penalties[2] * finderPenaltyCountPatterns(run_history);
                    }

                    color = module(x, y);
                    y_cycles = 1;
                }
            }

            result += penalties[2] * finderPenaltyTerminateAndCount(color, y_cycles, run_history);
        }

        /* 2x2 blocks of modules having same color */
        for (size_t y{0}; y < size() - 1; y++) {
            for (size_t x{0}; x < size() - 1; x++) {
                color = module(x, y);

                if (color == module(x + 1, y)
                        and color == module(x, y + 1)
                        and color == module(x + 1, y + 1)) {
                    result += penalties[1];
                }
            }
        }

        /* Balance of dark and light modules */
        for (const auto& row: *this) {
            for (auto module: row) {
                if (module) {
                    dark_counter++;
                }
            }
        }

        const auto area{static_cast<long>(getArea())};

        /* Compute the smallest integer k >= 0 such that (45-5k)% <= dark/total <= (55+5k)% */
        int k{static_cast<int>(
                (abs(dark_counter * 20 - area * 10) + area - 1) / area) - 1};

        assert(0 <= k and k <= 9);
        result += k * penalties[3];

        // Non-tight upper bound based on the penalties.
        assert(0 <= result and result <= 2'568'888L);

        return result;
    }

    /*
     * Pre-Conditions:
     *      Reference to the ErrorCorrectionEncoder from the previous layer,
     *      optional final_mask.
     *
     * Post-Conditions:
     *      Fills the QR code matrix with the data bits & other information,
     *      a 0 represents a light module, while a 1 represents a black module.
     *      Applies the final_mask.
     *
     * Check 7.7 -> 7.10
     */
    Structurer::Structurer(ErrorCorrectionEncoder& ec_encoder, int mask):
            ec_encoder{ec_encoder},
            SquareMatrix(ec_encoder.getMatrixSize()),  // Initialize super class
            function_modules(ec_encoder.getMatrixSize()),
            final_mask{mask} {

        drawFunctionPatterns();
        drawCodewords();

        if (final_mask == -1) {
            final_mask = generateMask();
        }

        applyMask(final_mask);
        drawFormatBits(final_mask);

        /* Dispose, not needed anymore */
        function_modules.clear();
        function_modules.shrink_to_fit();
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Draws the 8-bit codewords (data and error correction) onto the matrix.
     *      Function modules need to be marked off before this is called.
     */
    void Structurer::drawVersion() {
        const long version{ec_encoder.encoder.analyzer.getVersion()};

        if (version < 7) {
            return;
        }

        /*
         * Calculate error correction code & pack bits.
         * Check Annex D.
         */
        long rem{version};

        for (int i{0}; i < 12; i++) {
            rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
        }

        long bits{version << 12 | rem};
        assert(bits >> 18 == 0);

        bool bit;
        size_t a, b;

        /* Draw 2 copies */
        for (int i{0}; i < 18; i++) {
            bit = getBit(bits, i);
            a = size() - 11 + i % 3;
            b = i / 3;

            setFunctionModule(a, b, bit);
            setFunctionModule(b, a, bit);
        }
    }

    /*
     * Pre-Conditions:
     *      An integer n,
     *      an integer i.
     *
     * Post-Conditions:
     *      Returns the i-th bit in n.
     */
    bool Structurer::getBit(long n, int i) {
        return (n >> i) & 1;
    }

    /*
     * Pre-Conditions:
     *      Center of the pattern (x, y).
     *
     * Post-Conditions:
     *      Draws a 5x5 alignment pattern.
     */
    void Structurer::drawAlignmentPattern(size_t x, size_t y) {
        for (int dy{-2}; dy <= 2; dy++) {
            for (int dx{-2}; dx <= 2; dx++) {
                setFunctionModule(x + dx, y + dy, max(abs(dx), abs(dy)) != 1);
            }
        }
    }

    /*
     * Pre-Conditions:
     *      Center of the pattern (x, y).
     *
     * Post-Conditions:
     *      Draws the 9x9 finder pattern without the border separator.
     */
    void Structurer::drawFinderPattern(size_t x, size_t y) {
        long distance, xx, yy;

        for (int dy{-4}; dy <= 4; dy++) {
            for (int dx{-4}; dx <= 4; dx++) {
                distance = max(abs(dx), abs(dy));
                xx = static_cast<long>(x) + dx;
                yy = static_cast<long>(y) + dy;

                if (0 <= xx and xx < size() and 0 <= yy and yy < size()) {
                    setFunctionModule(xx, yy, distance != 2 and distance != 4);
                }
            }
        }
    }

    /*
     * Pre-Conditions:
     *      Mask value.
     *
     * Post-Conditions:
     *      Draws two copies of the format bits (with its own error correction code)
     *      based on the given final_mask and the ECL.
     *
     * Check 7.8
     */
    void Structurer::drawFormatBits(int mask) {
        const auto ecl_bits{
            ec_encoder.encoder.analyzer.getEclBits()
        };

        /* Calculate error correction code & pack bits */
        int data{ecl_bits << 3 | mask};
        int rem{data};

        for (int i{0}; i < 10; i++) {
            rem = (rem << 1) ^ ((rem >> 9) * 0x537);
        }

        int bits{(data << 10 | rem) ^ 0x5412};
        assert(bits >> 15 == 0);

        /* Draw first copy */
        for (int i{0}; i <= 5; i++) {
            setFunctionModule(8, i, getBit(bits, i));
        }

        setFunctionModule(8, 7, getBit(bits, 6));
        setFunctionModule(8, 8, getBit(bits, 7));
        setFunctionModule(7, 8, getBit(bits, 8));

        for (int i{9}; i < 15; i++) {
            setFunctionModule(14 - i, 8, getBit(bits, i));
        }

        /* Draw second copy */
        for (int i{0}; i < 8; i++) {
            setFunctionModule(size() - i - 1, 8, getBit(bits, i));
        }

        for (int i{8}; i < 15; i++) {
            setFunctionModule(8, size() + i - 15, getBit(bits, i));
        }

        /* Always dark */
        setFunctionModule(8, size() - 8, true);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Reads this object's version field,
     *      and draws and marks all function modules.
     */
    void Structurer::drawFunctionPatterns() {
        /* Draw horizontal & vertical timing patterns */
        for (size_t i{0}; i < size(); i++) {
            setFunctionModule(6, i, i % 2 == 0);
            setFunctionModule(i, 6, i % 2 == 0);
        }

        /* Draw all three finder patterns */
        drawFinderPattern(3, 3);
        drawFinderPattern(size() - 4, 3);
        drawFinderPattern(3, size() - 4);

        /* Draw alignment patterns */
        const auto& alignment_centers{getAlignmentPatternPositions()};
        size_t aligns{alignment_centers.size()};

        for (size_t i{0}; i < aligns; i++) {
            for (size_t j{0}; j < aligns; j++) {
                /* Do not draw on the finder patterns */
                if (not ((i == 0 and j == 0) or (i == 0 and j == aligns - 1)
                    or (i == aligns - 1 and j == 0))) {
                    drawAlignmentPattern(alignment_centers.at(i),
                                         alignment_centers.at(j));
                }
            }
        }

        /*
         * Draw configuration data.
         * Dummy mask value, later overwritten.
         */
        drawFormatBits(0);
        drawVersion();
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Draws the 8-bit codewords (data and error correction) onto the matrix.
     *      Function modules need to be marked off before this is called.
     */
    void Structurer::drawCodewords() {
        assert(ec_encoder.size() == ec_encoder.encoder.getVersionBitCount() / 8);

        size_t bit_index{0}, x, y;
        bool is_upward;

        for (long right = static_cast<long>(size() - 1); 1 <= right; right -= 2) {
            if (right == 6) {
                right--;
            }

            for (size_t v{0}; v < size(); v++) {
                for (int j{0}; j < 2; j++) {
                    x = right - j;
                    is_upward = ((right + 1) & 2) == 0;
                    y = is_upward ? size() - v - 1 : v;

                    if (not function_modules.at(y, x) and bit_index < 8 * ec_encoder.size()) {
                        at(y, x) = getBit(ec_encoder.at(bit_index >> 3),
                                          static_cast<int>(7 - (bit_index & 7)));
                        bit_index++;
                    }
                }
            }
        }

        assert(bit_index == 8 * ec_encoder.size());
    }

    /*
     * Pre-Conditions:
     *      Coordinates of a module,
     *      color of a module.
     *
     * Post-Conditions:
     *      Sets the color of a module & marks it as a function module.
     */
    void Structurer::setFunctionModule(size_t x, size_t y, bool is_dark) {
        at(y, x) = is_dark;
        function_modules.at(y, x) = true;
    }

    /*
     * Pre-Conditions:
     *      current run length,
     *      run history.
     *
     * Post-Conditions:
     *      Pushes the given value to the front and drops the last value.
     *      A helper function for getPenalty().
     */
    void Structurer::finderPenaltyAddHistory(size_t length,
                                             array<int, 7>& history) const {
        if (not history.at(0)) {
            length += size();
        }

        copy_backward(history.cbegin(), history.cend() - 1, history.end());
        history.at(0) = static_cast<int>(length);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns an ascending list of positions of alignment patterns.
     *      Each position is in the range [0,176] and are used on both the x and y axes.
     */
    deque<size_t> Structurer::getAlignmentPatternPositions() const {
        const auto version{ec_encoder.encoder.analyzer.getVersion()};

        if (version == 1) {
            return {};
        }

        const int aligns{version / 7 + 2};
        const int steps{version == 32 ? 26 : 2 * (4 * version + 2 * aligns + 1) / (2 * aligns - 2)};

        deque<size_t> result{};

        for (size_t i{0}, pos{size() - 7}; i < aligns - 1; i++, pos -= steps) {
            result.push_front(pos);
        }

        result.push_front(6);
        return result;
    }

    /*
     * Pre-Conditions:
     *      Run history.
     *
     * Post-Conditions:
     *      Returns 0, 1, or 2.
     *      Helper for the getPenalty function.
     */
    int Structurer::finderPenaltyCountPatterns(const array<int, 7>& history) const {
        auto n{history.at(1)};
        assert(n <= 3 * size());

        const bool core{0 < n
                and history.at(2) == n
                and history.at(3) == 3 * n
                and history.at(4) == n
                and history.at(5) == n};

        return (core and 4 * n <= history.at(0) and history.at(6) >= n ? 1 : 0)
        + (core and 4 * n <= history.at(6) and history.at(0) >= n ? 1 : 0);
    }

    /*
     * Pre-Conditions:
     *      Current run color,
     *      current run length,
     *      run history.
     *
     * Post-Conditions:
     *      None.
     *
     * Must be called at the end of a line (row or column) of modules.
     * A helper function for getPenalty().
     */
    int Structurer::finderPenaltyTerminateAndCount(bool color,
                                                   size_t length,
                                                   array<int, 7>& history) const {
        /* Terminate dark run */
        if (color) {
            finderPenaltyAddHistory(length, history);
            length = 0;
        }

        /* Add light border to final run */
        length += size();
        finderPenaltyAddHistory(length, history);

        return finderPenaltyCountPatterns(history);
    }

    /*
     * Pre-Conditions:
     *      Mask value.
     *
     * Post-Conditions:
     *      Applies the given final_mask onto the matrix.
     *
     * Check 7.8
     */
    void Structurer::applyMask(int mask) {
        /* Mask functions array */
        const static function<bool(size_t, size_t)> mask_functions[8]{
           [](size_t x, size_t y) {
               return (x + y) % 2 == 0;
           },
           [](size_t x, size_t y) {
               return y % 2 == 0;
           },
           [](size_t x, size_t y) {
               return x % 3 == 0;
           },
           [](size_t x, size_t y) {
               return (x + y) % 3 == 0;
           },
           [](size_t x, size_t y) {
               return (x / 3 + y / 2) % 2 == 0;
           },
           [](size_t x, size_t y) {
               return x + y % 2 + x * y % 3 == 0;
           },
           [](size_t x, size_t y) {
               return (x * y % 2 + x * y % 3) % 2 == 0;
           },
           [](size_t x, size_t y) {
               return ((x + y) % 2 + x * y % 3) % 2 == 0;
           }
        };

        if (mask < 0 or 7 < mask) {
            throw domain_error("Mask out of range [0, 7]");
        }

        const auto& func{mask_functions[mask]};

        for (size_t y{0}; y < size(); y++) {
            for (size_t x{0}; x < size(); x++) {
                at(y, x) = at(y, x) ^ (func(x, y) and not function_modules.at(y, x));
            }
        }
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the best final_mask for the given data.
     *
     * Check 7.8.3
     */
    int Structurer::generateMask() {
        int result{-1};
        long min_penalty{LONG_MAX}, penalty;

        for (int i{0}; i < 8; i++) {
            applyMask(i);
            drawFormatBits(i);

            penalty = getPenalty();

            if (penalty < min_penalty) {
                min_penalty = penalty;
                result = i;
            }

            /* Undoes the mask due to XOR */
            applyMask(i);
        }

        return result;
    }
}

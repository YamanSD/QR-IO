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


#include <array>
#include <utility>

#include "Structurer.h"


namespace Qrio {
    using std::abs, std::array;

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
            run_history = array<int, 7>();

            for (size_t x{0}; x < size(); x++) {
                if (at(x, y) == color) {
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

                    color = at(x, y);
                    x_cycles = 1;
                }
            }

            result += finderPenaltyTerminateAndCount(color, x_cycles, run_history);
        }

        /* Adjacent modules in column having same color, and finder-like patterns */
        for (size_t x{0}; x < size(); x++) {
            color = false;
            y_cycles = 0;
            run_history = array<int, 7>();

            for (size_t y{0}; y < size(); y++) {
                if (at(x, y) == color) {
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

                    color = at(x, y);
                    y_cycles = 1;
                }
            }
        }

        /* 2x2 blocks of modules having same color */
        for (size_t y{0}; y < size() - 1; y++) {
            for (size_t x{0}; x < size() - 1; x++) {
                color = at(x, y);

                if (color == at(x + 1, y)
                        and color == at(x, y + 1)
                        and color == at(x + 1, y + 1)) {
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

        const auto area{getArea()};

        /* Compute the smallest integer k >= 0 such that (45-5k)% <= dark/total <= (55+5k)% */
        int k{static_cast<int>(
                (abs(static_cast<long>(dark_counter * 20 - area * 10)) + area - 1)
                / area) - 1};

        assert(0 <= k and k <= 9);
        result += k * penalties[3];

        // Non-tight upper bound based on the penalties.
        assert(0 <= result and result <= 2'568'888L);

        return result;
    }

    /*
     * Pre-Conditions:
     *      Reference to the ErrorCorrectionEncoder from the previous layer,
     *      optional mask.
     *
     * Post-Conditions:
     *      Fills the QR code matrix with the data bits & other information,
     *      a 0 represents a light module, while a 1 represents a black module.
     *      Applies the mask.
     *
     * Check 7.7 -> 7.10
     */
    Structurer::Structurer(ErrorCorrectionEncoder& ec_encoder, int mask):
    ec_encoder{ec_encoder},
    SquareMatrix(ec_encoder.getMatrixSize()),  // Initialize super class
    function_modules(ec_encoder.getMatrixSize()),
    mask{mask} {

    }
}

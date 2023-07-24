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

#ifndef QR_IO_STRUCTURER_H
#define QR_IO_STRUCTURER_H

#include <array>
#include <deque>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ErrorCorrectionEncoder.h"
#include "SquareMatrix.h"


namespace Qrio {
    /*
     * Structurer: 1.2
     *
     * Responsible for structuring the final message, place modules,
     * data final_mask, & place the format information.
     * Responsible for Steps 4, 5, 6, & 7 of the encoding procedure.
     */
    class Structurer final: public SquareMatrix {
    public:
        /* Error correction encoder from the previous layer */
        ErrorCorrectionEncoder ec_encoder;

        /* Final mask of the QR code */
        int final_mask;

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
        explicit Structurer(const ErrorCorrectionEncoder&, int mask = -1);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      None.
         *
         * Default constructor used temporarily by the QrCode class.
         */
        Structurer();
    private:
        /*
         * Matrix of function modules.
         * These modules are not included in the masking.
         * Used mainly during masking.
         */
        SquareMatrix function_modules;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Draws the 8-bit codewords (data and error correction) onto the matrix.
         *      Function modules need to be marked off before this is called.
         */
        void drawCodewords();

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the best final_mask for the given data.
         *
         * Check 7.8.3
         */
        [[nodiscard]] int generateMask();

        /*
         * Pre-Conditions:
         *      Mask value.
         *
         * Post-Conditions:
         *      Applies the given final_mask onto the matrix.
         *
         * Check 7.8
         */
        void applyMask(int);

        /*
         * Pre-Conditions:
         *      Mask value.
         *
         * Post-Conditions:
         *      Draws two copies of the format bits (with its own error correction code)
         *      based on the given final_mask and the ECL.
         */
        void drawFormatBits(int);

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
        [[nodiscard]] long getPenalty();

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Draws two copies of the version bits with its own ECC,
         *      iff 7 <= version <= 40 (check 7.7.2).
         */
        void drawVersion();

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Reads this object's version field,
         *      and draws and marks all function modules.
         */
        void drawFunctionPatterns();

        /*
         * Pre-Conditions:
         *      Center of the pattern (x, y).
         *
         * Post-Conditions:
         *      Draws the 9x9 finder pattern without the border separator.
         */
        void drawFinderPattern(size_t, size_t);

        /*
         * Pre-Conditions:
         *      Center of the pattern (x, y).
         *
         * Post-Conditions:
         *      Draws a 5x5 alignment pattern.
         */
        void drawAlignmentPattern(size_t, size_t);

        /*
         * Pre-Conditions:
         *      Coordinates of a module,
         *      color of a module.
         *
         * Post-Conditions:
         *      Sets the color of a module & marks it as a function module.
         */
        void setFunctionModule(size_t, size_t, bool);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns an ascending list of positions of alignment patterns.
         *      Each position is in the range [0,176] and are used on both the x and y axes.
         */
        [[nodiscard]] std::deque<size_t> getAlignmentPatternPositions() const;

        /*
         * Pre-Conditions:
         *      Run history.
         *
         * Post-Conditions:
         *      Returns 0, 1, or 2.
         *      Helper for the getPenalty function.
         */
        [[nodiscard]] int finderPenaltyCountPatterns(const std::array<int, 7>&) const;

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
         * A helper function for getPenaltyScore().
         */
        [[nodiscard]] int finderPenaltyTerminateAndCount(bool,
                                                         size_t,
                                                         std::array<int, 7>&) const;

        /*
         * Pre-Conditions:
         *      current run length,
         *      run history.
         *
         * Post-Conditions:
         *      Pushes the given value to the front and drops the last value.
         *      A helper function for getPenaltyScore().
         */
        void finderPenaltyAddHistory(size_t, std::array<int, 7>&) const;

        /*
         * Pre-Conditions:
         *      An integer n,
         *      an integer i.
         *
         * Post-Conditions:
         *      Returns the i-th bit in n.
         */
        [[nodiscard]] static bool getBit(long, int);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Rotates the matrix by 90 degrees anti-clockwise.
         *
         * Correction after creating the QR.
         */
        void rotate();

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Flips the matrix on its horizontal axis.
         *
         * Correction after creating the QR.
         */
        void flip();
    };
}


#endif //QR_IO_STRUCTURER_H

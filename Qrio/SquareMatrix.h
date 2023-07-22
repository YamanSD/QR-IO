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

#ifndef QR_IO_SQUAREMATRIX_H
#define QR_IO_SQUAREMATRIX_H

#include <vector>


namespace Qrio {
    /*
     * SquareMatrix: 1.1
     *
     * Used to store the bits of an N x N matrix.
     * Inherits from a std::vector<std::vector<bool>>.
     */
    class SquareMatrix: public std::vector<std::vector<bool>> {
    public:
        /*
         * Pre-Conditions:
         *      Length of one of the sides of the matrix.
         *
         * Post-Conditions:
         *      n equals the given side length,
         *      n empty bool vectors of size n are created
         *      with default value false.
         */
        explicit SquareMatrix(size_t);

        /*
         * Pre-Conditions:
         *      Row index, column index.
         *      Indices are valid (i.e. in [0, n))
         *
         * Post-Conditions:
         *      Reference to bool at (r, c) in the matrix.
         */
        [[nodiscard]] std::vector<bool>::reference at(size_t, size_t);

        /*
         * Pre-Conditions:
         *      Row index, column index.
         *      Indices are valid (i.e. in [0, n))
         *
         * Post-Conditions:
         *      Reference to bool at (y, x) in the matrix.
         */
        [[nodiscard]] std::vector<bool>::reference module(size_t, size_t);

        /*
         * Pre-Conditions:
         *      Row index, column index.
         *      Indices are valid (i.e. in [0, n))
         *
         * Post-Conditions:
         *      Constant reference to bool at (r, c) in the matrix.
         */
        [[nodiscard]] std::vector<bool>::const_reference at(size_t, size_t) const;

        /*
         * Pre-Conditions:
         *      Row index, column index.
         *      Indices are valid (i.e. in [0, n))
         *
         * Post-Conditions:
         *      Constant reference to bool at (y, x) in the matrix.
         */
        [[nodiscard]] std::vector<bool>::const_reference module(size_t, size_t) const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the area of the matrix.
         */
        size_t getArea() const;
    };
}

#endif //QR_IO_SQUAREMATRIX_H

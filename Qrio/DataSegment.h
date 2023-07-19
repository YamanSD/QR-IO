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

#ifndef QR_IO_DATASEGMENT_H
#define QR_IO_DATASEGMENT_H

#include <string>

#include "Designator.h"


namespace Qrio {
    /*
     * DataSegment: 1.0
     *
     * Used to store information about a segment of data and how it will
     * be later encoded.
     * Mainly used by the DataAnalyzer class.
     * Instances are immutable.
     */
    class DataSegment final {
    public:
        /*
         * Pre-Conditions:
         *      Reference to the data string,
         *      Start index of the segment in the data string,
         *      End index of the segment in the data string,
         *      Designator describing the data type.
         *
         * Post-Conditions:
         *      data refers to the given data string,
         *      start equals the given start index,
         *      end equals the given end index,
         *      Designator refers to the given designator.
         */
        explicit DataSegment(const std::wstring&, size_t,
                             size_t, Designator);

        /*
         * Pre-Conditions:
         *      Constant reference to a DataSegment
         *
         * Post-Conditions:
         *      data refers to the given data string,
         *      start equals the given start index,
         *      end equals the given end index,
         *      Designator refers to the given designator.
         */
        explicit DataSegment(const DataSegment&);

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a constant reference to the string data.
         */
        [[nodiscard]] const std::wstring& getData() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the start index.
         */
        [[nodiscard]] size_t getStart() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the end index.
         */
        [[nodiscard]] size_t getEnd() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a constant reference to the mode Designator.
         */
        [[nodiscard]] const Designator& getType() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns type bits.
         */
        [[nodiscard]] int getTypeBits() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a constant reference to the substring
         *      represented by [start, end[ in the data string.
         */
        [[nodiscard]] std::wstring getDataSegment() const;

        /*
         * Pre-Conditions:
         *      Index of the character in the substring from [start, end[.
         *
         * Post-Conditions:
         *      Returns a const reference to the wchar_t at index start + index.
         */
        [[nodiscard]] const wchar_t& operator[](size_t) const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a const iterator to the start of the substring.
         */
        [[nodiscard]] std::wstring::const_iterator begin() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns a const iterator to the end of the substring.
         */
        [[nodiscard]] std::wstring::const_iterator end() const;

        /*
         * Pre-Conditions:
         *      None.
         *
         * Post-Conditions:
         *      Returns the size of the data.
         */
        [[nodiscard]] size_t size() const;

        /*
         * Pre-Conditions:
         *      Start index,
         *      size of the substring.
         *
         * Post-Conditions:
         *      Returns a substring from [start, end[
         */
        [[nodiscard]] std::wstring substr(size_t, size_t) const;
    private:
        /* Data string */
        const std::wstring& data;

        /* Starting index of the segment in the data string */
        const size_t start_index;

        /* End index of the segment in the data string */
        const size_t end_index;

        /* Describes the data type */
        const Designator mode;
    };
}


#endif //QR_IO_DATASEGMENT_H

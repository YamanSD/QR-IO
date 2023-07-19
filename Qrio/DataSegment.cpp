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

#include <string>

#include "DataSegment.h"


namespace Qrio {
    using std::wstring;

    /*
     * Pre-Conditions:
     *      Reference to the data wstring,
     *      Start index of the segment in the data wstring,
     *      End index of the segment in the data wstring,
     *      Designator describing the data type.
     *
     * Post-Conditions:
     *      data refers to the given data wstring,
     *      start equals the given start index,
     *      end equals the given end index,
     *      Designator refers to the given designator.
     */
    DataSegment::DataSegment(const wstring& data,
                             size_t start,
                             size_t end,
                             Designator mode):
                             data{data},
                             start_index{start},
                             end_index{end},
                             mode{mode} {}

     /*
      * Pre-Conditions:
      *      None.
      *
      * Post-Conditions:
      *      Returns a constant reference to the wstring data.
      */
    const wstring& DataSegment::getData() const {
        return data;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the start index.
     */
    size_t DataSegment::getStart() const {
        return start_index;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the end index.
     */
    size_t DataSegment::getEnd() const {
        return end_index;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns a constant reference to the mode Designator.
     */
    const Designator& DataSegment::getType() const {
        return mode;
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns a constant reference to the subwstring
     *      represented by [start, end[ in the data wstring.
     */
    wstring DataSegment::getDataSegment() const {
        return data.substr(start_index, size());
    }

    /*
     * Pre-Conditions:
     *      Index of the character in the subwstring from [start, end[.
     *
     * Post-Conditions:
     *      Returns a const reference to the wchar_t at index start + index.
     */
    const wchar_t& DataSegment::operator[](size_t index) const {
        return data[start_index + index];
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns a const iterator to the start of the subwstring.
     */
    wstring::const_iterator DataSegment::begin() const {
        return data.begin() + static_cast<long>(start_index);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns a const iterator to the end of the subwstring.
     */
    wstring::const_iterator DataSegment::end() const {
        return data.begin() + static_cast<long>(end_index);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns the size of the data.
     */
    size_t DataSegment::size() const {
        return getEnd() - getStart();
    }

    /*
     * Pre-Conditions:
     *      Start index,
     *      end index.
     *
     * Post-Conditions:
     *      Returns a substring from [start, end[
     */
    std::wstring DataSegment::substr(size_t start, size_t size) const {
        return getData().substr(getStart() + start, size);
    }

    /*
     * Pre-Conditions:
     *      None.
     *
     * Post-Conditions:
     *      Returns type bits.
     */
    int DataSegment::getTypeBits() const {
        return static_cast<int>(mode);
    }

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
    DataSegment::DataSegment(const DataSegment& other):
    DataSegment(other.getData(), other.getStart(),
                other.getEnd(), other.getType()) {}
}

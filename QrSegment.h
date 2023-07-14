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

#ifndef QR_IO_QRSEGMENT_H
#define QR_IO_QRSEGMENT_H

#include <string>
#include <vector>

#include "BitBuffer.h"
#include "Mode.h"


namespace qr_io {
    /*
     * A segment of character/binary/control data in a QR Code symbol.
     * Instances of this class are immutable.
     * The mid-level way to create a segment is to take the payload data
     * and call a static factory function such as QrSegment::makeNumeric().
     * The low-level way to create a segment is to custom-make the bit buffer
     * and call the QrSegment() constructor with appropriate values.
     * This segment class imposes no length restrictions, but QR Codes have restrictions.
     * Even in the most favorable conditions, a QR Code can only hold 7089 characters of data.
     * Any segment longer than this is meaningless for the purpose of generating QR Codes.
     */
    class QrSegment final {
    public:
        // Used for Numeric & Alphanumeric
        QrSegment(ModeType, const std::string&);

        // Used for auto determination of mode between numeric, alpha, or bytes.
        explicit QrSegment(const std::string&);

        // Used for ECI
        explicit QrSegment(long);

        // Used for bytes
        explicit QrSegment(const std::vector<int>&);

        [[nodiscard]] size_t getDataSize() const;

        [[nodiscard]] static long getTotalBits(const std::vector<QrSegment>&, int) ;

        [[nodiscard]] const BitBuffer& getData() const;

        [[nodiscard]] size_t getSize() const;

        [[nodiscard]] const Mode& getMode() const;

    private:
        [[nodiscard]] static ModeType getDataType(const std::string&);

        [[nodiscard]] static int validAlphanumeric(char c);

        BitBuffer buffer;

        Mode mode;

        size_t size;
    };
}


#endif //QR_IO_QRSEGMENT_H

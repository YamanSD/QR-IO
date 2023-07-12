//
// Created by Yaman SirajAldeen on 7/12/23.
//

#ifndef QR_IO_QRSEGMENT_H
#define QR_IO_QRSEGMENT_H

#include <string>
#include <vector>

#include "BitBuffer.h"


namespace qr_io {
    // Values are subject to change
    enum class ModeType {
        NUMERIC = 0b0001,
        ALPHANUMERIC = 0b0010,
        BYTE = 0b0100,
        KANJI = 0b1000,
        ECI = 0b0111,
    };

    class Mode {
    public:
        Mode(int, int, int, int, ModeType);

        [[nodiscard]] int getBitCount() const;

        [[nodiscard]] ModeType getType() const;
    private:
        int bit_count;
        ModeType mode;
    };

    class QrSegment final {
    public:
        [[nodiscard]] int getTotalBits(const std::vector<QrSegment>&, int) const;

        [[nodiscard]] const BitBuffer& getData() const;

        [[nodiscard]] int getNumChars() const;

        [[nodiscard]] const Mode& getMode() const;

        QrSegment(const Mode&, int, const BitBuffer&);

        QrSegment(const Mode&, int, BitBuffer&&);

        QrSegment(ModeType, const std::string&);

        QrSegment(ModeType, long);

        QrSegment(ModeType, const std::vector<int>&);

        [[nodiscard]] static bool isAlphanumeric(const std::string&);

        [[nodiscard]] static bool isNumeric(const std::string&);


    private:
        BitBuffer data;
        int size;
        const Mode& mode;
    };
}


#endif //QR_IO_QRSEGMENT_H

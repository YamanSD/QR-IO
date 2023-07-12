/*
 * MIT LICENSE INSERT
 */

#ifndef QR_IO_BITBUFFER_H
#define QR_IO_BITBUFFER_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

namespace qr_io {
    /*
     * BitBuffer: 1.0
     *
     * Used to store bits dynamically.
     * Inherits from a std::vector<bool>.
     *
     * Public Methods:
     *      void appendBits(std::uint32_t value, int n);
     *          Appends the lower n bits of value into the bit buffer.
     *          The order of the bits must remain intact;
     *          (i.e. append from the most major, to the least major bit)
     *          Assertions: 0 <= n <= 31, value < 2 ^ n.
     *
     * Private Methods:
     *      static void checkInput(std::uint32_t value, int n) const;
     *          Checks if 0 <= n <= 31, value < 2 ^ n.
     *          If true continue.
     *          Otherwise, std::domain_error is thrown.
     */
    class BitBuffer final: public std::vector<bool>  {
    public:
        /*
         * Pre-Conditions:
         *      0 <= n <= 31,
         *      value < 2 ^ n
         * Post-Conditions:
         *      Lower n bits of value appended into the buffer.
         *      Their order remains intact.
         */
        void appendBits(std::uint32_t, int);
    private:
        /*
         * Checks if 0 <= n <= 31, value < 2 ^ n.
         *          If true continue.
         *          Otherwise, std::domain_error is thrown
         */
        static void checkInput(std::uint32_t, int);
    };

    /*
     * Pre-Conditions:
     *      None
     * Post-Conditions:
     *      Displays the bits in the buffer, followed by a newline character.
     *
     * Mainly used for debugging purposes.
     */
    std::ostream& operator<<(std::ostream&, const BitBuffer&);
}



#endif //QR_IO_BITBUFFER_H

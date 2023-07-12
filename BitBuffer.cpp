/*
 * MIT LICENSE INSERT
 */

#include "BitBuffer.h"


namespace qr_io {
    using std::domain_error, std::endl,
            std::ostream, std::to_string,
            std::uint32_t;

    /*
     * Pre-Conditions:
     *      0 <= n <= 31,
     *      value < 2 ^ n
     * Post-Conditions:
     *      Lower n bits of value appended into the buffer.
     *      Their order remains intact.
     *
     * Assertion is performed by the checkInput function.
     */
    void BitBuffer::appendBits(uint32_t value, int n) {
        /* Assert 0 <= n <= 31, value < 2 ^ n */
        checkInput(value, n);

        /* Append bit by bit from n-th bit to the 0-th bit */
        for (int i{0}; i < n; i++) {
            push_back(value & (1 << (n - i)));
        }
    }

    /*
     * Checks if 0 <= n <= 31, value < 2 ^ n.
     *          If true continue.
     *          Otherwise, std::domain_error is thrown
     */
     void BitBuffer::checkInput(uint32_t value, int n) {
        /*
         * Checks if n is less than 0,
         * then if n is greater than 31,
         * then if 2 ^ n <= value (i.e. if value has a set 32nd bit)
         */
        if (n < 0 or 31 < n or value >> n) {
            throw domain_error(
                    "\n0 <= n <= 31, value < 2 ^ n;\n"
                    "but n = (" + to_string(n) + ") "
                    "& value = (" + to_string(value) + ")\n");
        }
    }

    /*
     * Pre-Conditions:
     *      None
     * Post-Conditions:
     *      Displays the bits in the buffer, followed by a newline character.
     *
     * Mainly used for debugging purposes.
     */
    ostream& operator<<(ostream& out, const BitBuffer& buffer) {
         for (auto b: buffer) {
             out << b;
         }

         return out << endl;
     }
}

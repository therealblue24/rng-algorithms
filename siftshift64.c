/* All this code is published to public domain. See the
 * license file in the root folder for more info.
 */

#include <stdint.h>

typedef uint64_t rand_t;

/*
 * "Sifts" a number into a different state.
 *
 * Used in siftshift64().
 */

rand_t sift(rand_t in) {
    rand_t out = (in * 0x2717);
    out ^= 0x1abea61d2f;

    out *= out;
    out ^= ((out << 23) | (out >> 16));
    out ^= ((out << 16) | (out >> 16));

    return out;
}

/*
 * Main siftshift64 RNG function. If using a seed, do
 * siftshift64(seed). When needing to generate another
 * number do: siftshift64([insert previous siftshift64 result here])
 */

rand_t siftshift64(rand_t prev) {
    rand_t out = sift(prev);
    out >>= 1;
    out ^= sift(prev);
    for(int i=0;i<64;i++) {
        int last_bit = out & 1;
        int second_last_bit = out & 2;
        out >>= 2;
        out ^= sift(prev);
        out |= last_bit;
        out |= second_last_bit;
        out >>= 1;
        out *= out;
    }
    return out;
}

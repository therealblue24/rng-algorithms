/* All this code is published to public domain. See the
 * license file in the root folder for more info.
 */
#include <stdint.h>
typedef uint64_t rand_t;
typedef uint16_t shortrand_t;

/*
 * Compress a 64-bit number into a 16-bit
 * number. Uses the apply-mask algorithim
 * of compression on a number, put the number
 * cannot be restored. Thus, we see an example
 * of number loseless compression.
 */
shortrand_t compress6416(rand_t in) {
    shortrand_t out = 0;
    shortrand_t k[4] = {0};
    k[0] = (shortrand_t)(in >> 48);
    k[1] = (shortrand_t)(in >> 32);
    k[2] = (shortrand_t)(in >> 16);
    k[3] = (shortrand_t)(in >> 0);
    out = (k[0] ^ k[1] ^ k[2] ^ k[3]);
    return out;
}

rand_t replace64_helper_squared_curve25519(rand_t i) {
    return (i*i*i) + (486662*i*i) + (i);
}
rand_t replace64_helper_null(rand_t i) {
    return i;
}

/*
 * Main RNG function. Use replace64(seed, ...)
 * to return a random number and
 * replace64([insert previous replace64 result here], ...)
 * to continue generating numbers.
 * The 2nd argument is the helper function to reduce
 * deactivated numbers. Pass NULL for no default,
 * or pass another function which returns rand_t and
 * has 1 input: rand_t. replace64_helper_squared_curve25519
 * is the best function for this job.
 */
#include <stdlib.h>
rand_t replace64(rand_t seed, rand_t(*helper_fun)(rand_t)) {
    rand_t out = seed;
    for(int i=0;i<4;i++) {
        shortrand_t sout = compress6416(out);
        if(sout < UINT8_MAX) {
            if(helper_fun == NULL) {
                out = replace64_helper_null(out);
            } else {
                out = helper_fun(out);
            }
            sout = compress6416(out);
        }
        out >>= 16;
        out += ((rand_t)(sout) << 48);
    }
    return out;
}

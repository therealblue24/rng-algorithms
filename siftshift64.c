#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

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

/* Splitmix64 + Siftshift64 */

rand_t siftshift64_drng(rand_t prev) {
    rand_t st = prev + 1;
    st ^= 5;
    st += 0x9e3779b97f4a7c15;
    rand_t z = st;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return siftshift64(z ^ (z >> 31));
}

/* For debugging.  Easier to read and type. */
#define opt1_rand siftshift64
int main() {
    printf("%ld\t\t\t%ld\n", opt1_rand(time(NULL)), opt1_rand(opt1_rand(time(NULL))));
}

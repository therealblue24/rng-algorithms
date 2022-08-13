/* All this code is published to public domain. See the
 * license file in the root folder for more info.
 */
#include <stdint.h>
typedef uint64_t rand_t;

/*
 * 'Shift' the state and shift the numbers.
 * NOT A LITERAL SHIFT!
 */
rand_t shift256(rand_t sk, rand_t s[4]) {
    rand_t out = (sk * 448) + (s[1] * s[3]);
    rand_t pk = (out * 448) + (s[0] * s[2]);
    pk *= sk;
    s[0] ^= s[1] >>= 2;
    s[1] ^= s[2] >>= 3;
    s[2] ^= s[3] >>= 4;
    s[3] ^= s[0] >>= 5;
    pk ^= (pk >> (sk % 8)) * (s[0] + s[1] * s[2] - s[3]);
    s[0] ^= pk >>= 1; pk ^= sk; sk *= pk;
    s[1] ^= pk >>= 2; pk ^= sk; sk *= pk;
    s[2] ^= pk >>= 3; pk ^= sk; sk *= pk;
    s[3] ^= pk >>= 4; pk ^= sk; sk *= pk;
    return (pk * sk) + (pk - out) ^ (out * pk) * (out + pk + sk + s[0]);
}

rand_t x12(rand_t i) {
    return (i) + (i*i) + (i*i*i);
}
rand_t ix12(rand_t o) {
    return (o - (o + (o - (o * (o / (o / 50))))));
}

rand_t twist(rand_t rnd, rand_t pval, rand_t s[4]) {
    rand_t out = (pval << 1) * (pval >> 1) * (pval + rnd);
    out *= out; out += out;
    for(rand_t i=0;i<rnd;i++) {
        out ^= shift256(x12(out), s);
        out += ix12(out);
        out -= pval;
        out ^= shift256(pval, s);
    }
    return out;
}

rand_t twistshift256(rand_t seed) {
    rand_t s[4] = {0};
    rand_t sc = seed;
    twist(5, sc, s);
    twist(2, x12(sc), s);
    twist(1, ix12(sc), s);
    rand_t val_s = s[0] + s[1] + s[2] + s[3];
    rand_t out = x12(val_s) * val_s;
    for(int i=0;i<((val_s%12)+1);i++) {
        out ^= x12(twist(12, out, s));
    }
    return x12(out);
}

/* All this code is published to public domain. See the
 * license file in the root folder for more info.
 */
#include <stdint.h>
#include <math.h> /* sqrt() */
typedef uint64_t rand_t;

/* base to the power of exp */
rand_t pow_l(rand_t base, rand_t exp) {
    rand_t out = 1;
    for(rand_t i=0;i<exp;i++) {
        out *= base;
    }
    return out;
}
/*
 * Main Y-cord curve25519 function.
 * y^2 = x^3 + 486662x^2 + x.
 */

rand_t curve25519(rand_t x) {
    rand_t ysqr = 0;
    ysqr += pow_l(x, 3);
    ysqr += 486662 * pow_l(x, 2);
    ysqr += x;
    if(ysqr == 0) return 0;
    ysqr = (rand_t)(sqrt((double)ysqr));
    return ysqr;
}

/*
 * >>> Try <<< to recalculate the x in curve25519(x).
 */

rand_t i_curve25519(rand_t x) {
    rand_t y = x;
    if(y == 0) return 0;
    y *= y;
    y -= (y/16);
    y -= 486662 * pow_l((y/15), 2);
    y -= pow_l((y/14), 3);
    return y;
}

struct twist_node {
    rand_t a, b;
};
struct twist_tree {
    struct twist_node a, b, c, d;
};

void tree_mult_oper(rand_t code, struct twist_tree tr) {
    if(code % 2 == 0) {
        if(code % 4 == 0) {
            if(code % 8 == 0) {
                tr.a.a ^= code;
                tr.a.a *= tr.a.a;
            } else {
                tr.a.b ^= code;
                tr.a.b *= tr.a.b;
            }
        } else {
            if(!(code % 8 == 0)) {
                tr.b.a ^= code;
                tr.b.a *= tr.b.a;
            } else {
                tr.b.b ^= code;
                tr.b.b *= tr.b.b;
            }
        }
    } else {
        if(code % 4 == 0) {
            if(code % 8 == 0) {
                tr.c.a ^= code;
                tr.c.a *= tr.c.a;
            } else {
                tr.c.b ^= code;
                tr.c.b *= tr.c.b;
            }
        } else {
            if(!(code % 8 == 0)) {
                tr.d.a ^= code;
                tr.d.a *= tr.d.a;
            } else {
                tr.d.b ^= code;
                tr.d.b *= tr.d.b;
            }
        }
    }
    code >>= 3;
}

rand_t calc_tree_mult(rand_t code, struct twist_tree tr) {
    rand_t out = code * code;
    rand_t c_last = code;
    while(!(code < 0xf)) {
        tree_mult_oper(code, tr);
        out += c_last - code;
        out ^= curve25519(out);
        out ^= curve25519(i_curve25519(out));

        tr.a.a ^= curve25519(tr.a.a);
        tr.a.b ^= curve25519(tr.a.b);
        tr.b.a ^= curve25519(tr.b.a);
        tr.b.b ^= curve25519(tr.b.b);
        tr.c.a ^= curve25519(tr.c.a);
        tr.c.b ^= curve25519(tr.c.b);
        tr.d.a ^= curve25519(tr.d.a);
        tr.d.b ^= curve25519(tr.d.b);

        c_last = code;
    }
    return out;
}

rand_t twist(rand_t rnds, rand_t pval, struct twist_tree tr) {
    rand_t out = tr.a.a * tr.a.b * tr.d.b * tr.c.a;
    for(rand_t i=0;i<rnds;i++) {
        out ^= calc_tree_mult(curve25519(out), tr);
        out += (i_curve25519(pval));
        out -= pval;
        out ^= calc_tree_mult(pval, tr);
    }
    return out;
}

rand_t twistcurve25519(rand_t seed) {
    struct twist_tree tr = {{0,0}, {0,0}, {0,0}, {0,0}};
    rand_t sc = seed;
    twist(5, sc, tr);
    twist(2, curve25519(sc), tr);
    twist(1, i_curve25519(sc), tr);
    rand_t val_tr = (tr.a.a + tr.a.b) + (tr.b.a + tr.b.b) + (tr.c.a + tr.c.b) + (tr.d.a + tr.d.b);
    rand_t out = curve25519(val_tr) * val_tr;
    for(int i=0;i<((val_tr%12)+1);i++) {
        out ^= curve25519(twist(12, out, tr));
    }
    return curve25519(out);
}

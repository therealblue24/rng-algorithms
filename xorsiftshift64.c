/* All this code is published to public domain. See the
 * license file in the root folder for more info.
 */

#include <stdint.h>

typedef uint64_t rand_t;

/*
 * "Sifts" a number into a different state.
 *  + does a few xor rotines.
 * Used in siftshift64().
 */

rand_t xorsift(rand_t in)
{
	rand_t out = (in * 0x2717);
	out ^= 0x1abea61d2f;

	out *= out;
	out ^= ((out << 23) | (out >> 16));
	out ^= ((out << 16) | (out >> 16));

	rand_t var = out ^ (out >> ((out ^ (out / 5)) % 32));
	var ^= var << 31;
	var ^= var >> 11;
	var ^= out ^ (out >> 30);
	var *= 1181783497276652981ULL;
	out *= var;
	out += 121731903912801ULL;

	return out;
}

/*
 * Main xorsiftshift64 RNG function. If using a seed, do
 * xorsiftshift64(seed). When needing to generate another
 * number do: xorsiftshift64([insert previous xorsiftshift64 result here])
 */

rand_t xorsiftshift64(rand_t prev)
{
	rand_t out = xorsift(prev);
	out >>= 1;
	out ^= xorsift(prev);
	for(int i = 0; i < 64; i++) {
		int last_bit = out & 1;
		int second_last_bit = out & 2;
		out >>= 2;
		out ^= xorsift(prev);
		out |= last_bit;
		out |= second_last_bit;
		out >>= 1;
		out *= out;
	}
	return out;
}

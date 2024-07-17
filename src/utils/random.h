//
// Created by josue on 2024-06-12.
//

#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

void random_seed(uint64_t seed);

float random_float(void);

float random_float_range(float min, float max);

uint32_t random_u32(void);

uint32_t random_u32_range(uint32_t min, uint32_t max);

#endif //RANDOM_H

// Implementation
#ifdef RANDOM_IMPLEMENTATION
// PRNG
#ifndef RANDOM_PRNG
#define RANDOM_PRNG
/*
 * Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

 * To the extent possible under law, the author has dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide.

 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This is xoshiro128+ 1.0, our best and fastest 32-bit generator for 32-bit
 * floating-point numbers. We suggest to use its upper bits for
 * floating-point generation, as it is slightly faster than xoshiro128**.
 * It passes all tests we are aware of except for
 * linearity tests, as the lowest four bits have low linear complexity, so
 * if low linear complexity is not considered an issue (as it is usually
 * the case) it can be used to generate 32-bit outputs, too.

 * We suggest to use a sign test to extract a random Boolean value, and
 * right shifts to extract subsets of bits.

 * The state must be seeded so that it is not everywhere zero.
 */


static inline uint32_t rotl(const uint32_t x, int k) {
	return (x << k) | (x >> (32 - k));
}

static uint32_t s[4];

uint32_t next(void) {
	const uint32_t result = s[0] + s[3];

	const uint32_t t = s[1] << 9;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 11);

	return result;
}


/// This is the jump function for the generator. It is equivalent
/// to 2^64 calls to next(); it can be used to generate 2^64
/// non-overlapping subsequences for parallel computations.
void jump(void) {
	static const uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

	uint32_t s0 = 0;
	uint32_t s1 = 0;
	uint32_t s2 = 0;
	uint32_t s3 = 0;
	for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 32; b++) {
			if (JUMP[i] & UINT32_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}


/// This is the long-jump function for the generator. It is equivalent to
/// 2^96 calls to next(); it can be used to generate 2^32 starting points,
/// from each of which jump() will generate 2^32 non-overlapping
/// subsequences for parallel distributed computations.
void long_jump(void) {
	static const uint32_t LONG_JUMP[] = { 0xb523952e, 0x0b6f099f, 0xccf5a0ef, 0x1c580662 };

	uint32_t s0 = 0;
	uint32_t s1 = 0;
	uint32_t s2 = 0;
	uint32_t s3 = 0;
	for(int i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++)
		for(int b = 0; b < 32; b++) {
			if (LONG_JUMP[i] & UINT32_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			next();
		}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;
}

#define RANDOM_SEED s
#define RANDOM_NEXT next
#define RANDOM_JUMP jump
#define RANDOM_LONG_JUMP long_jump

#endif //RANDOM_PRNG

static inline float u32_to_float(uint32_t n) {
    union {uint32_t u32; float f;} u = {.u32 = n >> 9 | 0x3f800000};
    return u.f - 1.0f;
}

void random_seed(uint64_t seed) {
    RANDOM_SEED[0] = seed;
    RANDOM_SEED[1] = 448390217;
}

float random_float(void) {
    return u32_to_float(RANDOM_NEXT());
}

float random_float_range(float min, float max) {
    return random_float() * (max - min) + min;
}

uint32_t random_u32(void) {
    return RANDOM_NEXT();
}

uint32_t random_u32_range(uint32_t min, uint32_t max){
    return random_u32() % (max - min + 1) + min;
}
#endif //RANDOM_IMPLEMENTATION

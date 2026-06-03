/*
 * random.c - Cryptographically secure random number generation
 * MeowPassword - Cat Name Based Secure Password Generator
 *
 * Copyright (c) 2025 Jeffrey Kunzelman
 * MIT License
 */

#if defined(__APPLE__)
#  define _DARWIN_C_SOURCE
#elif defined(__linux__)
#  define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meowpass.h"

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#  define MEOW_HAVE_ARC4RANDOM 1
#elif defined(__linux__)
#  include <sys/random.h>
#  include <sys/types.h>
#  include <errno.h>
#  define MEOW_HAVE_GETRANDOM 1
#endif

#ifdef MEOW_TEST_RNG
/* Deterministic xorshift64* PRNG used ONLY when the binary is built
 * with -DMEOW_TEST_RNG. The mutation-testing harness sets this flag
 * so test outputs are reproducible across mutant runs; production
 * binaries never define it and continue to use the OS CSPRNG below.
 * Not cryptographically secure — do not enable in shipped builds. */

static uint64_t meow_test_state = 0xDEADCA7FEEDBEEFULL;

void meow_test_seed(uint64_t seed) {
    meow_test_state = seed ? seed : 0xCAFEBABEDEADBEEFULL;
}

static uint64_t meow_test_next(void) {
    uint64_t x = meow_test_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    meow_test_state = x;
    return x * 0x2545F4914F6CDD1DULL;
}
#endif /* MEOW_TEST_RNG */

void meow_random_bytes(void *buf, size_t len) {
#ifdef MEOW_TEST_RNG
    unsigned char *p = (unsigned char *)buf;
    size_t got = 0;
    while (got < len) {
        uint64_t r = meow_test_next();
        size_t take = (len - got) > 8 ? 8 : (len - got);
        memcpy(p + got, &r, take);
        got += take;
    }
#elif defined(MEOW_HAVE_ARC4RANDOM)
    arc4random_buf(buf, len);
#else
    unsigned char *p = (unsigned char *)buf;
    size_t got = 0;
#  if defined(MEOW_HAVE_GETRANDOM)
    while (got < len) {
        ssize_t n = getrandom(p + got, len - got, 0);
        if (n < 0) {
            if (errno == EINTR) continue;
            break;
        }
        got += (size_t)n;
    }
#  endif
    if (got < len) {
        FILE *f = fopen("/dev/urandom", "rb");
        if (f) {
            got += fread(p + got, 1, len - got, f);
            fclose(f);
        }
    }
    if (got != len) {
        fprintf(stderr, "FATAL: secure random source unavailable\n");
        abort();
    }
#endif
}

uint32_t meow_random_u32(void) {
    uint32_t x;
    meow_random_bytes(&x, sizeof(x));
    return x;
}

uint32_t meow_random_uniform(uint32_t bound) {
    if (bound == 0) return 0;
#if defined(MEOW_HAVE_ARC4RANDOM) && !defined(MEOW_TEST_RNG)
    return arc4random_uniform(bound);
#else
    /* Unbiased rejection sampling: discard the partial range above the
       largest multiple of bound that fits in 2^32. Under MEOW_TEST_RNG
       this path is used so the seeded PRNG drives uniform() too. */
    uint32_t threshold = (uint32_t)(-bound) % bound;
    for (;;) {
        uint32_t r = meow_random_u32();
        if (r >= threshold) return r % bound;
    }
#endif
}

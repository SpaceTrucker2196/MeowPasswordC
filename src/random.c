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
#include "meowpass.h"

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#  define MEOW_HAVE_ARC4RANDOM 1
#elif defined(__linux__)
#  include <sys/random.h>
#  include <sys/types.h>
#  include <errno.h>
#  define MEOW_HAVE_GETRANDOM 1
#endif

void meow_random_bytes(void *buf, size_t len) {
#if defined(MEOW_HAVE_ARC4RANDOM)
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
#if defined(MEOW_HAVE_ARC4RANDOM)
    return arc4random_uniform(bound);
#else
    /* Unbiased rejection sampling: discard the partial range above the
       largest multiple of bound that fits in 2^32. */
    uint32_t threshold = (uint32_t)(-bound) % bound;
    for (;;) {
        uint32_t r = meow_random_u32();
        if (r >= threshold) return r % bound;
    }
#endif
}

// These hashers have been extracted from stb_ds.h

#ifndef BENCHMARKS_STB_DS_HASH_H
#define BENCHMARKS_STB_DS_HASH_H

#include <stddef.h>

#define STBDS_SIZE_T_BITS ((sizeof(size_t)) * 8)

#define STBDS_ROTATE_LEFT(val, n)  (((val) << (n)) | ((val) >> (STBDS_SIZE_T_BITS - (n))))
#define STBDS_ROTATE_RIGHT(val, n) (((val) >> (n)) | ((val) << (STBDS_SIZE_T_BITS - (n))))

#define STBDS_SIPHASH_2_4_C_ROUNDS 2
#define STBDS_SIPHASH_2_4_D_ROUNDS 4
typedef int STBDS_SIPHASH_2_4_can_only_be_used_in_64_bit_builds[sizeof(size_t) == 8 ? 1 : -1];

#define STBDS_SIPHASH_C_ROUNDS 1
#define STBDS_SIPHASH_D_ROUNDS 1

size_t stbds_siphash_bytes(void* p, size_t len, size_t seed)
{
    unsigned char* d = (unsigned char*)p;
    size_t i, j;
    size_t v0, v1, v2, v3, data;

    // hash that works on 32- or 64-bit registers without knowing which we have
    // (computes different results on 32-bit and 64-bit platform)
    // derived from siphash, but on 32-bit platforms very different as it uses 4 32-bit state not 4 64-bit
    v0 = ((((size_t)0x736f6d65 << 16) << 16) + 0x70736575) ^ seed;
    v1 = ((((size_t)0x646f7261 << 16) << 16) + 0x6e646f6d) ^ ~seed;
    v2 = ((((size_t)0x6c796765 << 16) << 16) + 0x6e657261) ^ seed;
    v3 = ((((size_t)0x74656462 << 16) << 16) + 0x79746573) ^ ~seed;

#define STBDS_SIPROUND()                                   \
    do {                                                   \
        v0 += v1;                                          \
        v1 = STBDS_ROTATE_LEFT(v1, 13);                    \
        v1 ^= v0;                                          \
        v0 = STBDS_ROTATE_LEFT(v0, STBDS_SIZE_T_BITS / 2); \
        v2 += v3;                                          \
        v3 = STBDS_ROTATE_LEFT(v3, 16);                    \
        v3 ^= v2;                                          \
        v2 += v1;                                          \
        v1 = STBDS_ROTATE_LEFT(v1, 17);                    \
        v1 ^= v2;                                          \
        v2 = STBDS_ROTATE_LEFT(v2, STBDS_SIZE_T_BITS / 2); \
        v0 += v3;                                          \
        v3 = STBDS_ROTATE_LEFT(v3, 21);                    \
        v3 ^= v0;                                          \
    } while (0)

    for (i = 0; i + sizeof(size_t) <= len; i += sizeof(size_t), d += sizeof(size_t)) {
        data = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
        data |= (size_t)(d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // discarded if size_t == 4

        v3 ^= data;
        for (j = 0; j < STBDS_SIPHASH_C_ROUNDS; ++j) {
            STBDS_SIPROUND();
        }
        v0 ^= data;
    }
    data = len << (STBDS_SIZE_T_BITS - 8);
    switch (len - i) {
        case 7:
            data |= ((size_t)d[6] << 24) << 24; // fall through
        case 6:
            data |= ((size_t)d[5] << 20) << 20; // fall through
        case 5:
            data |= ((size_t)d[4] << 16) << 16; // fall through
        case 4:
            data |= (d[3] << 24); // fall through
        case 3:
            data |= (d[2] << 16); // fall through
        case 2:
            data |= (d[1] << 8); // fall through
        case 1:
            data |= d[0]; // fall through
        case 0:
            break;
    }
    v3 ^= data;
    for (j = 0; j < STBDS_SIPHASH_C_ROUNDS; ++j) {
        STBDS_SIPROUND();
    }
    v0 ^= data;
    v2 ^= 0xff;
    for (j = 0; j < STBDS_SIPHASH_D_ROUNDS; ++j) {
        STBDS_SIPROUND();
    }

#undef STBDS_SIPROUND
    return v1 ^ v2 ^ v3; // slightly stronger since v0^v3 in above cancels out final round operation? I tweeted at the
                         // authors of SipHash about this but they didn't reply
}

size_t stbds_siphash_2_4_bytes(void* p, size_t len, size_t seed)
{
    unsigned char* d = (unsigned char*)p;
    size_t i, j;
    size_t v0, v1, v2, v3, data;

    // hash that works on 32- or 64-bit registers without knowing which we have
    // (computes different results on 32-bit and 64-bit platform)
    // derived from siphash, but on 32-bit platforms very different as it uses 4 32-bit state not 4 64-bit
    v0 = ((((size_t)0x736f6d65 << 16) << 16) + 0x70736575) ^ seed;
    v1 = ((((size_t)0x646f7261 << 16) << 16) + 0x6e646f6d) ^ ~seed;
    v2 = ((((size_t)0x6c796765 << 16) << 16) + 0x6e657261) ^ seed;
    v3 = ((((size_t)0x74656462 << 16) << 16) + 0x79746573) ^ ~seed;

    // hardcoded with key material in the siphash test vectors
    v0 ^= 0x0706050403020100ull ^ seed;
    v1 ^= 0x0f0e0d0c0b0a0908ull ^ ~seed;
    v2 ^= 0x0706050403020100ull ^ seed;
    v3 ^= 0x0f0e0d0c0b0a0908ull ^ ~seed;

#define STBDS_SIPROUND()                                   \
    do {                                                   \
        v0 += v1;                                          \
        v1 = STBDS_ROTATE_LEFT(v1, 13);                    \
        v1 ^= v0;                                          \
        v0 = STBDS_ROTATE_LEFT(v0, STBDS_SIZE_T_BITS / 2); \
        v2 += v3;                                          \
        v3 = STBDS_ROTATE_LEFT(v3, 16);                    \
        v3 ^= v2;                                          \
        v2 += v1;                                          \
        v1 = STBDS_ROTATE_LEFT(v1, 17);                    \
        v1 ^= v2;                                          \
        v2 = STBDS_ROTATE_LEFT(v2, STBDS_SIZE_T_BITS / 2); \
        v0 += v3;                                          \
        v3 = STBDS_ROTATE_LEFT(v3, 21);                    \
        v3 ^= v0;                                          \
    } while (0)

    for (i = 0; i + sizeof(size_t) <= len; i += sizeof(size_t), d += sizeof(size_t)) {
        data = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
        data |= (size_t)(d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // discarded if size_t == 4

        v3 ^= data;
        for (j = 0; j < STBDS_SIPHASH_2_4_C_ROUNDS; ++j) {
            STBDS_SIPROUND();
        }
        v0 ^= data;
    }
    data = len << (STBDS_SIZE_T_BITS - 8);
    switch (len - i) {
        case 7:
            data |= ((size_t)d[6] << 24) << 24; // fall through
        case 6:
            data |= ((size_t)d[5] << 20) << 20; // fall through
        case 5:
            data |= ((size_t)d[4] << 16) << 16; // fall through
        case 4:
            data |= (d[3] << 24); // fall through
        case 3:
            data |= (d[2] << 16); // fall through
        case 2:
            data |= (d[1] << 8); // fall through
        case 1:
            data |= d[0]; // fall through
        case 0:
            break;
    }
    v3 ^= data;
    for (j = 0; j < STBDS_SIPHASH_2_4_C_ROUNDS; ++j) {
        STBDS_SIPROUND();
    }
    v0 ^= data;
    v2 ^= 0xff;
    for (j = 0; j < STBDS_SIPHASH_2_4_D_ROUNDS; ++j) {
        STBDS_SIPROUND();
    }

#undef STBDS_SIPROUND
    return v0 ^ v1 ^ v2 ^ v3;
}

// Note: 32 bytes only!
size_t stbds_hash_bytes(void* p, size_t len, size_t seed)
{
    assert(len == 4);
    unsigned char* d = (unsigned char*)p;

    unsigned int hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);

    // HASH32-BB  Bob Jenkin's presumably-accidental version of Thomas Wang hash with rotates turned into shifts.
    // Note that converting these back to rotates makes it run a lot slower, presumably due to collisions, so I'm
    // not really sure what's going on.
    hash ^= seed;
    hash = (hash ^ 61) ^ (hash >> 16);
    hash = hash + (hash << 3);
    hash = hash ^ (hash >> 4);
    hash = hash * 0x27d4eb2d;
    hash ^= seed;
    hash = hash ^ (hash >> 15);

    return (((size_t)hash << 16 << 16) | hash) ^ seed;
}

// Note: 32 bytes only!
size_t stbds_hash_bytes_accidental(void* p, size_t len, size_t seed)
{
    assert(len == 4);
    unsigned char* d = (unsigned char*)p;

    unsigned int hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);

    // HASH32-A  Bob Jenkin's hash function w/o large constants
    hash ^= seed;
    hash -= (hash << 6);
    hash ^= (hash >> 17);
    hash -= (hash << 9);
    hash ^= seed;
    hash ^= (hash << 4);
    hash -= (hash << 3);
    hash ^= (hash << 10);
    hash ^= (hash >> 15);

    return (((size_t)hash << 16 << 16) | hash) ^ seed;
}

// Note: 32 bytes only!
size_t stbds_hash_bytes_murmur(void* p, size_t len, size_t seed)
{
    assert(len == 4);
    unsigned char* d = (unsigned char*)p;

    unsigned int hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);

    // HASH32-C   -  Murmur3
    hash ^= seed;
    hash *= 0xcc9e2d51;
    hash = (hash << 17) | (hash >> 15);
    hash *= 0x1b873593;
    hash ^= seed;
    hash = (hash << 19) | (hash >> 13);
    hash = hash * 5 + 0xe6546b64;
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= seed;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;

    return (((size_t)hash << 16 << 16) | hash) ^ seed;
}

// Note: 64 bytes only!
size_t stbds_hash_bytes_64(void* p, size_t len, size_t seed)
{
    assert(len == 8);
    unsigned char* d = (unsigned char*)p;

    size_t hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    hash |= (size_t)(d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // avoid warning if size_t == 4
    hash ^= seed;
    hash = (~hash) + (hash << 21);
    hash ^= STBDS_ROTATE_RIGHT(hash, 24);
    hash *= 265;
    hash ^= STBDS_ROTATE_RIGHT(hash, 14);
    hash ^= seed;
    hash *= 21;
    hash ^= STBDS_ROTATE_RIGHT(hash, 28);
    hash += (hash << 31);
    hash = (~hash) + (hash << 18);
    return hash;
}

#endif // BENCHMARKS_STB_DS_HASH_H

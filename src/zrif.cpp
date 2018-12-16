#include "zrif.hpp"

extern "C"
{
#include "puff.h"
#include "utils.h"
}

#include <cstring>

#define ADLER32_MOD 65521

#define ZLIB_DEFLATE_METHOD 8
#define ZLIB_DICTIONARY_ID_ZRIF 0x627d1d5d

static const uint8_t zrif_dict[] = {
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,
        0,   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  48,  48,  48,  48,
        57,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  48, 48,  48,  48,  54,
        48,  48, 48, 48, 55, 48, 48, 48, 48, 56, 0,  48, 48, 48,  48,  51,  48,
        48,  48, 48, 52, 48, 48, 48, 48, 53, 48, 95, 48, 48, 45,  65,  68,  68,
        67,  79, 78, 84, 48, 48, 48, 48, 50, 45, 80, 67, 83, 71,  48,  48,  48,
        48,  48, 48, 48, 48, 48, 48, 49, 45, 80, 67, 83, 69, 48,  48,  48,  45,
        80,  67, 83, 70, 48, 48, 48, 45, 80, 67, 83, 67, 48, 48,  48,  45,  80,
        67,  83, 68, 48, 48, 48, 45, 80, 67, 83, 65, 48, 48, 48,  45,  80,  67,
        83,  66, 48, 48, 48, 0,  1,  0,  1,  0,  1,  0,  2,  239, 205, 171, 137,
        103, 69, 35, 1,
};

static const uint8_t b64d[] = {
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57,
        58, 59, 60, 61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,
        7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
        25, 64, 64, 64, 64, 64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
        37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
        64, 64, 64, 64,
};

static uint32_t adler32(const uint8_t* data, size_t size)
{
    uint32_t a = 1;
    uint32_t b = 0;

    for (size_t i = 0; i < size; i++)
    {
        a = (a + data[i]) % ADLER32_MOD;
        b = (b + a) % ADLER32_MOD;
    }

    return (b << 16) | a;
}

static uint32_t base64_decode(const char* in, uint8_t* out)
{
    const uint8_t* out0 = out;
    const uint8_t* in8 = (uint8_t*)in;

    size_t len = strlen(in);
    if (in[len - 1] == '=')
    {
        len--;
    }
    if (in[len - 1] == '=')
    {
        len--;
    }

    for (size_t i = 0; i < len / 4; i++)
    {
        *out++ = (b64d[in8[0]] << 2) + ((b64d[in8[1]] & 0x30) >> 4);
        *out++ = (b64d[in8[1]] << 4) + (b64d[in8[2]] >> 2);
        *out++ = (b64d[in8[2]] << 6) + b64d[in8[3]];
        in8 += 4;
    }

    size_t left = len % 4;
    if (left == 2)
    {
        *out++ = (b64d[in8[0]] << 2) + ((b64d[in8[1]] & 0x30) >> 4);
        *out++ = (b64d[in8[1]] << 4);
    }
    else if (left == 3)
    {
        *out++ = (b64d[in8[0]] << 2) + ((b64d[in8[1]] & 0x30) >> 4);
        *out++ = (b64d[in8[1]] << 4) + (b64d[in8[2]] >> 2);
        *out++ = b64d[in8[2]] << 6;
    }

    return (uint32_t)(out - out0);
}

// https://www.ietf.org/rfc/rfc1950.txt
static uint32_t zlib_inflate(
        const uint8_t* in,
        uint32_t inlen,
        uint8_t* out,
        uint32_t outlen,
        char* error,
        uint32_t error_size)
{
    if (inlen < 2 + 4)
    {
        strncpy(error, "zRIF is too short", error_size);
        return 0;
    }

    if (((in[0] << 8) + in[1]) % 31 != 0)
    {
        strncpy(error, "zRIF header is corrupted", error_size);
        return 0;
    }

    if ((in[0] & 0xf) != ZLIB_DEFLATE_METHOD)
    {
        strncpy(error, "only deflate method supported in zRIF", error_size);
        return 0;
    }

    unsigned long slen = inlen - 4;
    unsigned long dlen = outlen;
    unsigned long dictlen = 0;

    if (in[1] & (1 << 5))
    {
        memcpy(out, zrif_dict, sizeof(zrif_dict));
        dictlen = sizeof(zrif_dict);

        if (get32be(in + 2) != ZLIB_DICTIONARY_ID_ZRIF)
        {
            strncpy(error, "zRIF uses unknown dictionary", error_size);
            return 0;
        }

        in += 6;
        slen -= 6;
    }
    else
    {
        in += 2;
        slen -= 2;
    }

    if (puff(dictlen, out, &dlen, in, &slen) != 0)
    {
        strncpy(error, "failed to uncompress zRIF", error_size);
        return 0;
    }
    memmove(out, out + dictlen, dlen);

    if (adler32(out, dlen) != get32be(in + slen))
    {
        strncpy(error, "zRIF is corrupted, wrong checksum", error_size);
        return 0;
    }

    return dlen;
}

int pkgi_zrif_decode(
        const char* str, uint8_t* rif, char* error, uint32_t error_size)
{
    uint8_t raw[1024];
    uint32_t len = base64_decode(str, raw);

    uint8_t out[1024 + sizeof(zrif_dict)];
    len = zlib_inflate(raw, len, out, sizeof(out), error, error_size);
    if (len != 512 && len != 1024)
    {
        strncpy(error, "wrong size of zRIF, is it corrupted?", error_size);
        return 0;
    }

    memcpy(rif, out, 1024);
    return 1;
}
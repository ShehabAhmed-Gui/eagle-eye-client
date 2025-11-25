﻿/* Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <cstdlib>
#include <cstdint>

#include <iostream>
#include <vector>
#include <array>
#include <bitset>
#include <iomanip>

#include "hash.h"


//Checks if the architecture is little endian in runtime
static bool is_little_endian();

static uint64_t to_big_endian_u64(uint64_t value);
static uint32_t to_big_endian_u32(uint32_t value);

static uint32_t circular_right_shift(uint32_t value, int shift_by);

static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);

static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);


//Pad to multiple of 512 bits
// input_len : the length of the input buffer in bytes, will be used to determine padding
//
// msg_len : the length of the message to be put at the end of the padded buffer
//          it is seperate from input_len for cases where pad_message()
//          is used on the last chunk of a big multiple chunk message
//          processeed by the sha256_NAME api.
//          when using a no state sha256 hash though with the entire message
//          processed at once, msg_len is the same as input_len.
static std::vector<char> pad_message(const char* input, int input_len, uint64_t msg_len);

static std::array<uint32_t, 64> prepare_schedule(const char* block);

static void process_schedule(const std::array<uint32_t, 64>& schedule, uint32_t* hash);


static const int CHUNK_SIZE_BITS = 512;
static const int CHUNK_SIZE_BYTES = CHUNK_SIZE_BITS / 8;
static const int BLOCK_SIZE = 64;
static const std::string HMAC_KEY = "4q72JHgX89z3BkFMt6cwQxL1rD28jpN5UfVhIZYPbCSeuGovRaWmA0sD9ECtX7Jf";

//initial hash values
static const std::array<uint32_t, 8> h =
    {
        0x6a09e667, // H0
        0xbb67ae85, // H1
        0x3c6ef372, // H2
        0xa54ff53a, // H3
        0x510e527f, // H4
        0x9b05688c, // H5
        0x1f83d9ab, // H6
        0x5be0cd19  // H7
};

static const std::array<uint32_t, 64> k =
    {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void hmac_sha256_init(hmac_sha256_state* state, uint64_t message_len)
{
    //msg length + the length of the ipad we will feed first
    sha256_init(&(state->inner_hash), message_len + BLOCK_SIZE);

    std::array<char, BLOCK_SIZE> block_sized_key;
    std::array<char, BLOCK_SIZE> ipad;

    ipad.fill(0x36);
    state->opad.fill(0x5c);

    memset(block_sized_key.data(), 0, block_sized_key.size());

    if (HMAC_KEY.size() > BLOCK_SIZE) {
        std::array<uint32_t, 8> key_hash = sha256(HMAC_KEY.data(), HMAC_KEY.size());

        for (int i = 0; i < key_hash.size(); i++)
        {
            key_hash[i] = to_big_endian_u32(key_hash[i]);
        }

        memcpy(block_sized_key.data(), key_hash.data(),
               key_hash.size() * sizeof(uint32_t));
    }
    else if (HMAC_KEY.size() < BLOCK_SIZE) {
        memcpy(block_sized_key.data(), HMAC_KEY.data(),
               HMAC_KEY.size());
    }
    else {
        memcpy(block_sized_key.data(), HMAC_KEY.data(),
               HMAC_KEY.size());
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        state->opad[i] = block_sized_key[i] ^ state->opad[i];
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        ipad[i] = block_sized_key[i] ^ ipad[i];
    }

    sha256_update(&(state->inner_hash), ipad.data(), BLOCK_SIZE);
}


void hmac_sha256_update(hmac_sha256_state* state, const char* block, int block_len)
{
    sha256_update(&(state->inner_hash), block, block_len);
}


void hmac_sha256_finalize(hmac_sha256_state* state)
{
    sha256_finalize(&(state->inner_hash));

    std::array<uint32_t, 8> inner_hash = state->inner_hash.hash;
    for (int i = 0; i < inner_hash.size(); i++)
    {
        inner_hash[i] = to_big_endian_u32(inner_hash[i]);
    }

    std::array<char, BLOCK_SIZE + (sizeof(uint32_t)*8)> outer;

    memcpy(outer.data(), state->opad.data(), state->opad.size());
    memcpy(outer.data() + state->opad.size(),
           inner_hash.data(),
           inner_hash.size() * sizeof(uint32_t));

    state->hash = sha256(outer.data(), outer.size());
}


std::array<uint32_t, 8> hmac_sha256(const char* msg, int msg_len)
{
    std::array<char, BLOCK_SIZE> block_sized_key;
    std::array<char, BLOCK_SIZE> ipad;
    std::array<char, BLOCK_SIZE> opad;

    ipad.fill(0x36);
    opad.fill(0x5c);

    memset(block_sized_key.data(), 0, block_sized_key.size());

    if (HMAC_KEY.size() > BLOCK_SIZE) {
        std::array<uint32_t, 8> key_hash = sha256(HMAC_KEY.data(), HMAC_KEY.size());

        for (int i = 0; i < key_hash.size(); i++)
        {
            key_hash[i] = to_big_endian_u32(key_hash[i]);
        }

        memcpy(block_sized_key.data(), key_hash.data(),
               key_hash.size() * sizeof(uint32_t));
    }
    else if (HMAC_KEY.size() < BLOCK_SIZE) {
        memcpy(block_sized_key.data(), HMAC_KEY.data(),
               HMAC_KEY.size());
    }
    else {
        memcpy(block_sized_key.data(), HMAC_KEY.data(),
               HMAC_KEY.size());
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        opad[i] = block_sized_key[i] ^ opad[i];
    }

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        ipad[i] = block_sized_key[i] ^ ipad[i];
    }

    std::vector<char> inner(ipad.size() + msg_len);
    std::array<char, opad.size() + (sizeof(uint32_t)*8)> outer;

    memcpy(inner.data(), ipad.data(), ipad.size());
    memcpy(inner.data() + ipad.size(), msg, msg_len);

    std::array<uint32_t, 8> inner_hash = sha256(inner.data(), inner.size());
    for (int i = 0; i < inner_hash.size(); i++)
    {
        inner_hash[i] = to_big_endian_u32(inner_hash[i]);
    }

    memcpy(outer.data(), opad.data(), opad.size());
    memcpy(outer.data() + opad.size(),
           inner_hash.data(),
           inner_hash.size() * sizeof(uint32_t));

    std::array<uint32_t, 8> hash = sha256(outer.data(), outer.size());

    return hash;
}


void sha256_init(sha256_state* state, uint64_t message_len)
{
    state->hash = h;
    state->buffer_len = 0;
    state->message_len = message_len;
}


void sha256_update(sha256_state* state, const char* block, int block_len)
{
    if (state->buffer_len > 0)
    {
        int needed = 64 - state->buffer_len;
        if (needed > block_len) {
            needed = block_len;
        }
        memcpy(state->buffer.data() + state->buffer_len, block, needed);

        if (state->buffer_len == 64) {
            state->schedule = prepare_schedule(state->buffer.data());
            process_schedule(state->schedule, state->hash.data());

            state->buffer_len = 0;
            block += needed;
            block_len -= needed;
        }
        else {
            return;
        }
    }

    while (block_len >= 64)
    {
        state->schedule = prepare_schedule(block);
        process_schedule(state->schedule, state->hash.data());

        block_len -= 64;
        block += 64;
    }

    if (block_len > 0)
    {
        memcpy(state->buffer.data(), block, block_len);
        state->buffer_len = block_len;
    }
}


void sha256_finalize(sha256_state* state)
{
    std::vector<char> padded_chunk = pad_message(state->buffer.data(), state->buffer_len,
                                                 state->message_len);
    int chunk_count = (padded_chunk.size() + CHUNK_SIZE_BYTES - 1) / CHUNK_SIZE_BYTES;

    for (int i = 0; i < chunk_count; i++)
    {
        std::array<uint32_t, 64> schedule = prepare_schedule(padded_chunk.data() + (i * CHUNK_SIZE_BYTES));
        process_schedule(schedule, state->hash.data());
    }
}


std::array<uint32_t, 8> sha256(const char* input, int input_len)
{
    std::vector<char> padded_message = pad_message(input, input_len, input_len);
    int chunk_count = (padded_message.size() + CHUNK_SIZE_BYTES - 1) / CHUNK_SIZE_BYTES;

    std::array<uint32_t, 8> hash = h;

    for (int i = 0; i < chunk_count; i++)
    {
        std::array<uint32_t, 64> schedule = prepare_schedule(padded_message.data() + (i * CHUNK_SIZE_BYTES));
        process_schedule(schedule, hash.data());
    }

    return hash;
}


void print_hash(const std::array<uint32_t, 8>& hash)
{
    for (int i = 0; i < hash.size(); i++)
    {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << (hash[i]);
    }
}


static void process_schedule(const std::array<uint32_t, 64>& schedule, uint32_t* hash)
{
    uint32_t a = hash[0];
    uint32_t b = hash[1];
    uint32_t c = hash[2];
    uint32_t d = hash[3];
    uint32_t e = hash[4];
    uint32_t f = hash[5];
    uint32_t g = hash[6];
    uint32_t h = hash[7];

    for (int i = 0; i < schedule.size(); i++)
    {
        uint32_t w = schedule[i];

        uint32_t S0 = circular_right_shift(a, 2) ^ circular_right_shift(a, 13) ^ circular_right_shift(a, 22);
        uint32_t S1 = circular_right_shift(e, 6) ^ circular_right_shift(e, 11) ^ circular_right_shift(e, 25);

        uint32_t T1 = h + S1 + ch(e, f, g) + k[i] + w;
        uint32_t T2 = S0 + maj(a, b, c);

        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
    hash[5] += f;
    hash[6] += g;
    hash[7] += h;
}


static std::array<uint32_t, 64> prepare_schedule(const char* block)
{
    const uint32_t* block_words = reinterpret_cast<const uint32_t*>(block);
    std::array<uint32_t, 64> schedule;

    for (int i = 0; i < 16; i++)
    {
        schedule[i] = to_big_endian_u32(block_words[i]);
    }

    for (int i = 16; i < 64; i++)
    {
        uint32_t word1 = schedule[i-15];
        uint32_t word2 = schedule[i-2];

        uint32_t s0 = circular_right_shift(word1, 7) ^ circular_right_shift(word1, 18) ^ (word1 >> 3);

        uint32_t s1 = circular_right_shift(word2, 17) ^ circular_right_shift(word2, 19) ^ (word2 >> 10);

        schedule[i] = s1 + schedule[i-7] + s0 + schedule[i-16];
    }

    return schedule;
}


static std::vector<char> pad_message(const char* input, int input_len, uint64_t msg_len)
{
    //Size of input in bits
    int l = input_len * 8;
    //Size of zero bit padding needed in bytes
    int plen = (CHUNK_SIZE_BITS - (l + 8 + 64) % CHUNK_SIZE_BITS) / 8;

    //Copy of the message with padding
    //and 8 bytes for the message length and 1 byte for the 1 bit
    int mlen = input_len + plen + 8 + 1;
    std::vector<char> m(mlen);

    //Copy the message
    memcpy(m.data(), input, input_len);

    //Single 1 bit
    m[input_len] = 0x80;

    //Padding
    memset(m.data() + input_len + 1, 0, plen);

    //Message length in bits
    uint64_t input_len_long = to_big_endian_u64(msg_len * 8);
    memcpy(m.data() + mlen - 8, &input_len_long, 8);

    return m;
}


static uint32_t to_big_endian_u32(uint32_t value)
{
    if (is_little_endian())
    {
        return ((value>>24)&0xff) | ((value>>8)&0xff00) | ((value<<8)&0xff0000) | ((value<<24)&0xff000000);
    }

    return value;
}


static uint64_t to_big_endian_u64(uint64_t value)
{
    if (is_little_endian())
    {
        return ((value & 0x00000000000000FFULL) << 56) |
               ((value & 0x000000000000FF00ULL) << 40) |
               ((value & 0x0000000000FF0000ULL) << 24) |
               ((value & 0x00000000FF000000ULL) << 8)  |
               ((value & 0x000000FF00000000ULL) >> 8)  |
               ((value & 0x0000FF0000000000ULL) >> 24) |
               ((value & 0x00FF000000000000ULL) >> 40) |
               ((value & 0xFF00000000000000ULL) >> 56);
    }

    return value;
}


static bool is_little_endian()
{
    uint16_t x = 1;
    return *(uint8_t*)&x == 1;
}


static uint32_t circular_right_shift(uint32_t value, int shift_by)
{
    return (value >> shift_by) | (value << (32 - shift_by));
}


static uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ ((~x) & z);
}


static uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

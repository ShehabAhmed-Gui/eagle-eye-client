/* Licensed under the Apache License, Version 2.0 (the "License");
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

#ifndef HASH_H
#define HASH_H

#include <cstdint>
#include <array>


/*
* USAGE
*
* sha256_FUNCTIONNAME(): Use this by default for consistency
*
* sha256_init(): Call once to initialize the sha256_state struct
* sha256_update(): Call for every chunk in the message
* sha256_finalize(): Call after going through all chunks
*
*
* sha256(): Takes in the entire message at once, doesn't support chunk streaming.
*           requires no state.
*/


struct sha256_state
{
    std::array<uint32_t, 8> hash;
    std::array<char, 64> buffer;
    int buffer_len;
    uint64_t message_len; //the length of the entire data, in bytes.
                          //if you are processing a 5000 kilobyte file
                          //in chunks. message_len = 5000 * 1000
};


//Used when the message will be hashed in chunks, for example when loading files.
void sha256_init(sha256_state* state, uint64_t message_len);
void sha256_update(sha256_state* state, const char* block, int block_len);
void sha256_finalize(sha256_state* state);


//Used when the entire message to be hashed will be passed in at once
std::array<uint32_t, 8> sha256(const char* input, int input_len);

//Prints out the 8 word SHA256 hash in hexadecimal.
void print_hash(const std::array<uint32_t, 8>& hash);

#endif

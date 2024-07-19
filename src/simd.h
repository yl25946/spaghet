#pragma once

#include "defs.h"

#if defined(USE_SIMD)
#include <immintrin.h>
#endif

#if defined(USE_AVX512)
using vepi16 = __m512i;
using vepi32 = __m512i;

inline vepi16 zero_epi16() { return *_mm512_setzero_si512(); }
inline vepi32 zero_epi32() { return *_mm512_setzero_si512(); }
inline vepi16 load_epi16(const int16_t *memory_address) { return _mm512_load_si512(reinterpret_cast<const __m512i *>(memory_address)); }
inline vepi32 load_epi32(const int32_t *memory_address) { return _mm512_load_si512(reinterpret_cast<const __m512i *>(memory_address)); }
// this function puts num as every value in the vector
inline vepi16 load_epi16(int num) { return _mm512_set1_epi16(num); };
inline vepi32 load_epi32(int num) { return _mm512_set1_epi32(num); };
// inline void store_epi16(void *memory_address, vepi16 vector) { _mm512_store_si512(memory_address, vector); }

inline vepi16 clip(vepi16 vector, int L1Q) { return _mm512_min_epi16(_mm512_max_epi16(vector, zero_epi16()), load_epi16(L1Q)); }

#elif defined(USE_AVX2)
using vepi16 = __m256i;
using vepi32 = __m256i;

inline vepi16 zero_epi16() { return _mm256_setzero_si256(); }
inline vepi32 zero_epi32() { return _mm256_setzero_si256(); }
inline vepi16 load_epi16(const int16_t *memory_address) { return _mm256_load_si256(reinterpret_cast<const __m256i *>(memory_address)); }
inline vepi32 load_epi32(const int32_t *memory_address) { return _mm256_load_si256(reinterpret_cast<const __m256i *>(memory_address)); }
// this function puts num as every value in the vector
inline vepi16 load_epi16(int num) { return _mm256_set1_epi16(num); };
inline vepi32 load_epi32(int num) { return _mm256_set1_epi32(num); };
// inline void store_epi16(void *memory_address, vepi16 vector) { _mm256_store_si256(memory_address, vector); }

inline vepi16 clip(vepi16 vector, int L1Q) { return _mm256_min_epi16(_mm256_max_epi16(vector, zero_epi16()), load_epi16(L1Q)); }

// implementation from Stormphrax
inline int _mm256_reduce_add_epi32(vepi32 vector)
{
    auto high128 = _mm256_extracti128_si256(vector, 1);
    auto low128 = _mm256_castsi256_si128(vector);

    auto sum128 = _mm_add_epi32(high128, low128);
    // copies the highest 64 bits twice into a 128 vector
    auto high64 = _mm_unpackhi_epi64(sum128, sum128);

    // we sum together the 64 least significant bits because those are the ones that matter
    // bottom 64 bits store the 2 important numbers
    auto sum64 = _mm_add_epi32(sum128, high64);

    // take the highest 32 bits and places thin into the least significant 32 bits in a 128 vector
    // the shuffle operation does this
    const auto high32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));

    // adds the two important numbers up
    const auto sum32 = _mm_add_epi32(sum64, high32);

    // extracts the sum
    return _mm_cvtsi128_si32(sum32);
}

// screlus everything and then reduces it to a single int
inline int screlu_reduce(const int16_t *accumulator_address, const int16_t *net_weight_address, int L1Q)
{
    vepi16 accumulator_values256 = load_epi16(accumulator_address);
    vepi16 net_weights256 = load_epi16(net_weight_address);

    vepi16 clipped256 = clip(accumulator_values256, L1Q);

    // extract high and low 128 bits
    const auto low128 = _mm256_castsi256_si128(clipped256);
    const auto high128 = _mm256_extracti128_si256(clipped256, 1);
    const auto low_net_weights128 = _mm256_castsi256_si128(net_weights256);
    const auto high_net_weights128 = _mm256_extracti128_si256(net_weights256, 1);

    // cast to int32
    vepi32 converted_low256 = _mm256_cvtepi16_epi32(low128);
    vepi32 converted_high256 = _mm256_cvtepi16_epi32(high128);
    vepi32 converted_low_net_weights256 = _mm256_cvtepi16_epi32(low_net_weights128);
    vepi32 converted_high_net_weights256 = _mm256_cvtepi16_epi32(high_net_weights128);

    // squaring
    vepi32 squared_low256 = _mm256_mullo_epi32(converted_low256, converted_low256);
    vepi32 squared_high256 = _mm256_mullo_epi32(converted_high256, converted_high256);

    // multiply by net weight

    vepi32 final_low256 = _mm256_mullo_epi32(squared_low256, converted_low_net_weights256);
    vepi32 final_high256 = _mm256_mullo_epi32(squared_high256, converted_high_net_weights256);

    // reduce into a single value
    int reduced_value = _mm256_reduce_add_epi32(final_low256);
    reduced_value += _mm256_reduce_add_epi32(final_high256);

    return reduced_value;
}

#endif
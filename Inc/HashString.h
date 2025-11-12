// ========================================================================
// File: HashString.h
//
// Author: winterknife
//
// Description: This header file contains inline routines to hash short
// strings using non-cryptographic hash functions
//
// Modifications:
//  2025-08-24	Created
//  2025-09-22	Updated
// ========================================================================

// ========================================================================
// Header guards
// ========================================================================

#pragma once

// ========================================================================
// Includes
// ========================================================================

#include <stdint.h>
#include <sal.h>

// ========================================================================
// Macros
// ========================================================================

#pragma region MACROS

// 64-bit magic prime for Fowler/Noll/Vo (FNV) hash
// May not be modified
#define FNV_PRIME_64 0x100000001B3ULL

// 64-bit non-zero initial basis for FNV-1 hash
// May be modified as long as it is non-zero
#define FNV_OFFSET_BASIS_64 0xCBF29CE484222325ULL

// Macro to hash a short string at compile time
#define HASH_STRING_COMPILE_TIME(String) hash_string_fnv1a_64_as_constant(String, 0x95DD92DBA3D9E736ULL)

// Macro to hash a short string at run time
#define HASH_STRING_RUN_TIME(String, Length) hash_string_fnv1a_64(String, Length, 0x95DD92DBA3D9E736ULL)

#pragma endregion

// ========================================================================
// Inline routines
// ========================================================================

#pragma region INLINES

// Original implementation: http://www.isthe.com/chongo/tech/comp/fnv/index.html
// Authors: Phong Vo, Glenn Fowler, Landon Curt Noll

// Online hash lookup:
// curl -s -D "/dev/stderr" https://hashdb.openanalysis.net/hash/fnv1a_64/<hash value in decimal> | jq

/// @brief Hash a short string using 64-bit Fowler/Noll/Vo (FNV-1a) hash function
/// @tparam Type Type template parameter that acts as a placeholder for the type of character literals that make up the string
/// @param pctyString Pointer to a constant null-terminated string of ANSI/UNICODE characters
/// @param dwptrSize Number of characters in the string, excluding the terminal null
/// @param qwHash FNV offset basis (initial hash value)
/// @return 64-bit unsigned integer representing the hash value
template <typename Type>
__attribute__((always_inline)) inline constexpr unsigned long long hash_string_fnv1a_64(
    _In_z_ const Type*        pctyString,
    _In_   size_t             dwptrSize,
    _In_   unsigned long long qwHash = FNV_OFFSET_BASIS_64
) noexcept {
    // Init local variables
    unsigned char byCurrent = 0;

    // Hash each byte of the buffer
    while (dwptrSize--) {
        // Get current byte
        byCurrent = static_cast<unsigned char>(*pctyString++);

        // XOR the lower 8-bits of the hash value with the current byte
        qwHash ^= byCurrent;

        // Multiply the hash value by the 64-bit FNV magic prime mod 2^64
        qwHash *= FNV_PRIME_64;
    }

    return qwHash;
}

#pragma endregion

// ========================================================================
// Immediate routines
// ========================================================================

#pragma region IMMEDIATES

/// @brief Hash a compile-time string constant at compile time using 64-bit Fowler/Noll/Vo (FNV-1a) hash function
/// @tparam Type Type template parameter that acts as a placeholder for the type of character literals that make up the string
/// @tparam Size Number of characters in the string, including the terminal null
/// @param tyarrInputData Reference to an array of constant ANSI/UNICODE characters
/// @param qwHash FNV offset basis (initial hash value)
/// @return 64-bit unsigned integer representing the hash value
template <typename Type, size_t Size>
consteval auto hash_string_fnv1a_64_as_constant(
    _In_z_ const Type (&tyarrInputData)[Size],
    _In_   unsigned long long qwHash = FNV_OFFSET_BASIS_64
) {
    return hash_string_fnv1a_64(tyarrInputData, (Size - 1), qwHash);
}

#pragma endregion
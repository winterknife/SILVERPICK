// ========================================================================
// File: StackString.h
//
// Author: winterknife
//
// Description: This header file contains inline routines to create
// string literals that are stored on the program stack
// using template metaprogramming
//
// Modifications:
//  2025-08-14	Created
//  2025-08-26	Updated
// ========================================================================

// ========================================================================
// Header guards
// ========================================================================

#pragma once

// ========================================================================
// Includes
// ========================================================================

#include <array>
#include <sal.h>

// ========================================================================
// Macros
// ========================================================================

#pragma region MACROS

// Macro to build a stack string
#define STACK_STRING(Name, String) auto Name = String##_stackstring

#pragma endregion

// ========================================================================
// Inline routines
// ========================================================================

#pragma region INLINES

// Original implementation: https://godbolt.org/z/5hrKvsY85
// Author: Can Bölük (@_can1357)

/// @brief Construct a stack string character-by-character using recursion to unpack the template parameter pack
/// @tparam Type Type template parameter that acts as a placeholder for the type of character literals
/// @tparam First Non-type template parameter representing the leading character in the sequence
/// @tparam ...Rest Non-type template parameter pack consisting of a sequence of characters
/// @param ptyString Pointer to a character in the std::array representing the stack string
template <typename Type, Type First, Type... Rest>
__attribute__((always_inline)) inline void store_character(
    _In_ volatile Type* ptyString
) {
    *ptyString++ = First;
    if constexpr(sizeof...(Rest) != 0) {
        store_character<Type, Rest...>(ptyString);
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
/// @brief Literal operator template to create a compile-time user-defined string literal that is stored on the program stack
/// @tparam Type Type template parameter that acts as a placeholder for the type of character literals
/// @tparam ...Rest Non-type template parameter pack consisting of a sequence of characters
/// @return Returns a std::array representing the stack string
template <typename Type, Type... Rest>
__attribute__((always_inline)) inline constexpr auto operator""_stackstring(
    void
) {
    std::array<Type, sizeof...(Rest)+1> array = {};
    store_character<Type, Rest...>(array.data());
    return array;
}
#pragma GCC diagnostic pop

#pragma endregion
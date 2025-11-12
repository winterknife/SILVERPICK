// ========================================================================
// File: Common.h
//
// Author: winterknife
//
// Description: Contains the common stuff for this project
//
// Modifications:
//  2021-08-21	Created
//  2025-11-02	Updated
// ========================================================================

// ========================================================================
// Header guards
// ========================================================================

#pragma once

// ========================================================================
// Predefined macros
// ========================================================================

// References:
// 1: https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
// 2: https://github.com/cpredef/predef

// Emit error message at compile time if build target is not 64-bit x86 Windows
#if not defined(__x86_64__) || not defined(_WIN64)
#error Unsupported build target!
#endif

// Emit error message at compile time if compiler is not GCC 15.2
#if not defined(__GNUC__) || (__GNUC__ != 15) || (__GNUC_MINOR__ != 2)
#error Unsupported compiler!
#endif

// Emit error message at compile time if C++ standard is not C++20
#if __cplusplus != 202002L
#error Unsupported C++ standard!
#endif

// ========================================================================
// Includes
// ========================================================================

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winternl.h>
#include <winnt.h>
#include <intrin.h>
#include <bit>
#include <string>
#include "BaseDataTypes.h"

// ========================================================================
// Macros
// ========================================================================

#pragma region MACROS

// Major version number
#define MAJOR_VERSION 1

// Minor version number
#define MINOR_VERSION 0

// Macro to specify C linkage (prevent name mangling)
#define EXTERN_C extern "C"

// References:
// 1: https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
// 2: https://gcc.gnu.org/onlinedocs/gcc/x86-Function-Attributes.html

// Macro to prevent a function from being considered for inlining
#define NO_INLINE __attribute__((noinline))

// Macro to inline a function independent of any restrictions that otherwise apply to inlining
#define FORCE_INLINE __attribute__((always_inline)) inline

// Macro to place a function at the beginning of the .text section
#define CODE_BEGIN __attribute__((section(".init")))

// Macro to generate an alternate prologue and epilogue to realign the stack
#define ALIGN_STACK __attribute__((force_align_arg_pointer))

// Macro to disable compiler optimizations for a specific function
#define DISABLE_OPTIMIZATION __attribute__((optimize("O0")))

// Convert a macro argument into a string constant
#define STRINGIZE_EX(x) #x
#define STRINGIZE(x) STRINGIZE_EX(x)

// Merge two tokens into one while expanding macros
#define CONCATENATE_EX(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_EX(x, y)

// Pragma macro
#define PRAGMA(x) _Pragma(#x)

// Macros to suppress compiler warnings
#define SUPPRESS_WARNING_PUSH     PRAGMA(GCC diagnostic push)
#define SUPPRESS_WARNING(warning) PRAGMA(GCC diagnostic ignored warning)
#define SUPPRESS_WARNING_POP      PRAGMA(GCC diagnostic pop)

// Macro to get the type of an expression
#define TYPEOF(x) decltype(x)

// Macros for C++ type casting operators
// Reference: https://stackoverflow.com/questions/332030/when-should-static-cast-dynamic-cast-const-cast-and-reinterpret-cast-be-used
#define STATIC_CAST(Type, Expression)      static_cast<Type>(Expression)
#define CONST_CAST(Type, Expression)       const_cast<Type>(Expression)
#define DYNAMIC_CAST(Type, Expression)     dynamic_cast<Type>(Expression)
#define REINTERPRET_CAST(Type, Expression) reinterpret_cast<Type>(Expression)
#define BIT_CAST(Type, Expression)         std::bit_cast<Type>(Expression)

// References:
// 1: https://devblogs.microsoft.com/oldnewthing/20190129-00/?p=100825
// 2: https://medium.com/@ophirharpaz/a-summary-of-x86-string-instructions-87566a28c20c

// Macro to fill a block of memory with zeroes given its address and length in bytes by generating the store string instruction (REP STOSB)
// Enhanced REP MOVSB and STOSB operation (ERMSB) is only available since Ivy Bridge Intel microarchitecture (2012/2013)
// Processors that provide support for enhanced MOVSB/STOSB operations are enumerated by the CPUID feature flag: CPUID:(EAX=7H, ECX=0H):EBX.ERMSB[bit 9] = 1
#define ZERO_MEMORY(Destination, Length) __stosb(reinterpret_cast<unsigned char*>(Destination), 0, Length)

// Macro to copy the contents of a source memory block to a destination memory block given its source address, destination address, and the number of bytes to copy by generating the move string instruction (REP MOVSB)
#define COPY_MEMORY(Destination, Source, Count) __movsb(reinterpret_cast<unsigned char*>(Destination), reinterpret_cast<const unsigned char*>(Source), Count)

#pragma endregion

// ========================================================================
// Inline routines
// ========================================================================

#pragma region INLINES

/// @brief Compare the contents of two memory blocks byte by byte using the compare string instruction (REPE CMPSB)
/// @param pcSource1 Pointer to the first block of memory
/// @param pcSource2 Pointer to the second block of memory
/// @param dwptrLength Number of bytes to compare
/// @return The return value is one if all bytes match up to the specified length value, or zero otherwise
__attribute__((always_inline)) inline bool compare_memory(
    _In_ const void* pcSource1,
    _In_ const void* pcSource2,
    _In_ size_t      dwptrLength
) {
    bool bResult;
    void* pRSI;
    void* pRDI;
    void* pRCX;

    asm volatile(
        "test rcx, rcx;"
        "repz cmpsb;"
        : "=S" (pRSI), "=D" (pRDI), "=c" (pRCX), "=@ccz" (bResult)
        : "0" (pcSource1), "1" (pcSource2), "2" (dwptrLength)
        : "memory"
    );

    return bResult;
}

/// @brief Scan the contents of a memory block for a byte using the scan string instruction (REPNE SCASB)
/// @param pcSource Pointer to the block of memory
/// @param byTarget Byte to look for
/// @param dwptrLength Number of bytes to check
/// @return Returns a pointer to the first matching byte if successful, or NULL otherwise
__attribute__((always_inline)) inline void* scan_memory(
    _In_ const void*   pcSource,
    _In_ unsigned char byTarget,
    _In_ size_t        dwptrLength
) {
    void* pRDI;
    void* pRCX;

    asm volatile(
        "jrcxz notfound;"
        "repnz scasb;"
        "jz found;"
        "notfound: mov %0, 1;"
        "found: dec %0;"
        : "=D" (pRDI), "=c" (pRCX)
        : "a" (byTarget), "0" (pcSource), "1" (dwptrLength)
        : "memory"
    );

    return pRDI;
}

#pragma endregion

// ========================================================================
// Immediate routines
// ========================================================================

#pragma region IMMEDIATES

/// @brief Force compile-time only evaluation of a function
/// @param value Input constant-expression function
/// @return Returns a compile-time constant
consteval auto as_constant(
    auto value
) {
    // References:
    // 1: https://andreasfertig.com/blog/2021/07/cpp20-a-neat-trick-with-consteval/

    return value;
}

/// @brief Get the length of a compile-time string constant at compile time
/// @tparam Type Type template parameter that acts as a placeholder for the type of character literals that make up the string
/// @param pctyString Pointer to a constant null-terminated string of ANSI/UNICODE characters
/// @return Returns the number of characters in the string, excluding the terminal null
template <typename Type>
consteval size_t get_string_length_as_constant(
    const Type* pctyString
) {
    // References:
    // 1: https://devblogs.microsoft.com/oldnewthing/20221114-00/?p=107393

    return std::char_traits<Type>::length(pctyString);
}

#pragma endregion
// ========================================================================
// File: BaseDataTypes.h
//
// Author: winterknife
//
// Description: Type definitions for common Windows base data types made
// for maintaining uniformity across all User Mode and Kernel Mode code
//
// Modifications:
//  2021-07-31	Created
//  2025-08-06	Updated
// ========================================================================

// ========================================================================
// Pragmas
// ========================================================================

#pragma once

// ========================================================================
// Type definitions
// ========================================================================

#pragma region TYPEDEFS

// 8-bit unsigned integer, range = 0 - 255, unsigned char
typedef unsigned char BYTE, *PBYTE;

// 16-bit unsigned integer, range = 0 - 65535, unsigned short
typedef unsigned short WORD, *PWORD;

// 32-bit unsigned integer, range = 0 - 4294967295, unsigned int or unsigned long
typedef unsigned long int DWORD, *PDWORD;

// 64-bit unsigned integer, range = 0 - 18446744073709551615, unsigned long long
typedef unsigned long long QWORD, *PQWORD;

// Should be TRUE (1) or FALSE (0), unsigned char or bool
typedef unsigned char BOOLEAN, *PBOOLEAN;

// 8-bit UTF-8/Multibyte/ANSI character, char
typedef char CHAR, *PCHAR;

// 16-bit UTF-16/Wide/UNICODE character, unsigned short
typedef wchar_t WCHAR, *PWCHAR;

// Pointer to any type, size = 4 bytes or 8 bytes depending on code bitness
typedef void* PVOID;

// Pointer to a constant of any type
typedef const void* PCVOID;

// Pointer to constant null-terminated string of ANSI characters
typedef const char* PCSTR;

// Pointer to constant null-terminated string of UNICODE characters
typedef const wchar_t* PCWSTR;

// Pointer to constant null-terminated string of ANSI or UNICODE characters depending on character encoding scheme
#if defined(UNICODE)
typedef const wchar_t* PCTSTR;
#else
typedef const char* PCTSTR;
#endif

// 32-bit or 64-bit unsigned integer, range = 0 - 4294967295 or 18446744073709551615 depending on compiler bitness
// Used for casting pointers
#if defined(_WIN64)
typedef unsigned long long DWORD_PTR, *PDWORD_PTR;
#else
typedef unsigned long int DWORD_PTR, *PDWORD_PTR;
#endif

#pragma endregion
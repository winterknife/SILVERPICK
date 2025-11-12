// ========================================================================
// File: PEParse.h
//
// Author: winterknife
//
// Description: Header file for PEParse.cpp source file
//
// Modifications:
//  2025-09-25  Created
//  2025-11-06  Updated
// ========================================================================

// ========================================================================
// Header guards
// ========================================================================

#pragma once

// ========================================================================
// Includes
// ========================================================================

#include "Common.h"
#include "HashString.h"

// ========================================================================
// Macros
// ========================================================================

#pragma region MACROS

// Hardcoded value for the exported symbol name maximum length
#define MAX_EXPORTED_SYMBOL_NAME_LEN 64ULL

// Macro to get the exported symbol address from a loaded user-mode module
#define GET_EXPORTED_SYMBOL_ADDRESS(ModuleBase, Name) get_exported_symbol_address_by_hash(ModuleBase, HASH_STRING_COMPILE_TIME(Name))

// Macro to declare and initialize a function pointer for run-time dynamic linking, use on a local scope
#define INITIALIZE_FUNCTION_POINTER(Function) typedef TYPEOF(Function) CONCATENATE(__type_, Function); CONCATENATE(__type_, Function*) Function = nullptr;

// Macro to resolve a function pointer using run-time dynamic linking, use on a local scope
#define RESOLVE_FUNCTION_POINTER(ModuleBase, Function) Function = BIT_CAST(CONCATENATE(__type_, Function*), GET_EXPORTED_SYMBOL_ADDRESS(ModuleBase, STRINGIZE(Function)));

#pragma endregion

// ========================================================================
// Inline routines
// ========================================================================

#pragma region INLINES

/// @brief Convert a Relative Virtual Address (RVA) to a Virtual Address (VA)
/// @tparam Type Type template parameter that acts as a placeholder for the type of the pointer
/// @param ptyBase The base address of an image that is mapped into memory
/// @param dwptrOffset The relative virtual address to be converted
/// @return The return value is the virtual address in the mapped image, performs type safe math
template <typename Type>
FORCE_INLINE Type* convert_rva_to_va(
    _In_ Type*     ptyBase,
    _In_ DWORD_PTR dwptrOffset
) noexcept {
    return BIT_CAST(Type*, (BIT_CAST(PBYTE, ptyBase) + dwptrOffset));
}

#pragma endregion

// ========================================================================
// C routine declarations
// ========================================================================

#pragma region DECLARATIONS

/// @brief Get the address of an exported symbol from the export table of the specified loaded PE module, custom implementation of kernel32!GetProcAddress routine
/// @param pImageBase Image base address of the loaded module
/// @param qwTargetExportHash 64-bit Fowler/Noll/Vo (FNV-1a) hash of the exported symbol name, can also be the exported symbol ordinal for nameless exports
/// @return Pointer to the target export, returns nullptr if the symbol is not found in the PE file's export table or if it is forwarded to an external module
EXTERN_C NO_INLINE PVOID __stdcall get_exported_symbol_address_by_hash(
    _In_ PVOID pImageBase,
    _In_ QWORD qwTargetExportHash
);

#pragma endregion
// ========================================================================
// File: UserModuleBase.h
//
// Author: winterknife
//
// Description: Header file for UserModuleBase.cpp source file
//
// Modifications:
//  2025-09-23  Created
//  2025-09-25  Updated
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

// Hardcoded ntdll!_LDR_DATA_TABLE_ENTRY.BaseDllName.MaximumLength value
#define MAX_BASE_DLL_NAME_LEN 64U

// Macro to get the image base address of a loaded user-mode module
#define GET_USER_MODULE_BASE(Name) get_user_module_base_by_hash(HASH_STRING_COMPILE_TIME(Name))

#pragma endregion

// ========================================================================
// Inline routines
// ========================================================================

#pragma region INLINES

/// @brief Get a pointer to the Thread Environment Block (TEB) of the current thread
/// @return A pointer to the TEB of the current thread
FORCE_INLINE PTEB get_current_teb(
    VOID
) {
    // GS segment base address points to the base address of the current thread's Thread Environment Block (TEB) in user-mode
    return BIT_CAST(TEB*, __readgsqword(FIELD_OFFSET(NT_TIB, Self)));
}

#pragma endregion

// ========================================================================
// C routine declarations
// ========================================================================

#pragma region DECLARATIONS

/// @brief To retrieve the base address of a loaded user-mode module (EXE/DLL) from user-mode, does not increment module reference count, custom implementation of kernel32!GetModuleHandle routine
/// @param qwTargetModuleHash 64-bit Fowler/Noll/Vo (FNV-1a) hash of the target module name in lowercase with the file name extension, can also be nullptr to get the base address of the calling module
/// @return Returns the base address of the loaded module if successful, or nullptr otherwise
EXTERN_C NO_INLINE PVOID __stdcall get_user_module_base_by_hash(
    _In_opt_ QWORD qwTargetModuleHash
);

#pragma endregion
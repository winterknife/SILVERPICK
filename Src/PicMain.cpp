// ========================================================================
// File: PicMain.cpp
//
// Author: winterknife
//
// Description: Source file that contains the PIC entry point
//
// Modifications:
//  2025-09-29  Created
//  2025-09-29  Updated
// ========================================================================

// ========================================================================
// Includes
// ========================================================================

#include "StackString.h"
#include "UserModuleBase.h"
#include "PEParse.h"

// ========================================================================
// Routines
// ========================================================================

#pragma region ROUTINES

/// @brief PIC start function
/// @param None
/// @return None
EXTERN_C NO_INLINE VOID __stdcall payload(
    VOID
) {
    // Init local variables
    PVOID pKernel32 = nullptr;
    INITIALIZE_FUNCTION_POINTER(LoadLibraryA);
    HMODULE hUser32 = nullptr;
    STACK_STRING(sstrUser32, "user32.dll");
    INITIALIZE_FUNCTION_POINTER(MessageBoxA);
    STACK_STRING(sstrText, "an extra long hello world!");
    STACK_STRING(sstrCaption, "Demo");

    // Get the image base address of kernel32.dll
    pKernel32 = GET_USER_MODULE_BASE("kernel32.dll");
    if (pKernel32 == nullptr)
        goto cleanup;

    // Resolve kernel32!LoadLibraryA
    RESOLVE_FUNCTION_POINTER(pKernel32, LoadLibraryA);
    if (LoadLibraryA == nullptr)
        goto cleanup;

    // Load User32.dll into the process VAS
    hUser32 = LoadLibraryA(sstrUser32.data());
    if (hUser32 == nullptr)
        goto cleanup;

    // Resolve user32!MessageBoxA
    RESOLVE_FUNCTION_POINTER(hUser32, MessageBoxA);
    if (MessageBoxA == nullptr)
        goto cleanup;

    // Display a message box
    MessageBoxA(nullptr, sstrText.data(), sstrCaption.data(), MB_OK);

    // Cleanup
cleanup:
    return;
}

/// @brief PIC entry point
/// @param None
/// @return None
EXTERN_C NO_INLINE CODE_BEGIN ALIGN_STACK VOID __stdcall PicEntry(
    VOID
) {
    payload();
}

#pragma endregion
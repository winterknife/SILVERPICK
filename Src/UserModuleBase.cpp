// ========================================================================
// File: UserModuleBase.cpp
//
// Author: winterknife
//
// Description: This source file contains routine(s) to query the
// image base address of user modules mapped in UVAS without any
// external dependencies including CRT, Win32, or Native API routines
//
// Modifications:
//  2025-09-23  Created
//  2025-09-25  Updated
// ========================================================================

// ========================================================================
// Includes
// ========================================================================

#include "UserModuleBase.h"
#include <algorithm>

// ========================================================================
// Routines
// ========================================================================

#pragma region ROUTINES

_Use_decl_annotations_
PVOID __stdcall get_user_module_base_by_hash(
    QWORD qwTargetModuleHash
) {
    // Init local variables
    PTEB pTeb = nullptr;
    PPEB pPeb = nullptr;
    PVOID pImageBase = nullptr;
    PPEB_LDR_DATA pPebLdrData = nullptr;
    PLIST_ENTRY pListHead = nullptr;
    PLIST_ENTRY pNextEntry = nullptr;
    PLDR_DATA_TABLE_ENTRY pLdrDataTableEntry = nullptr;
    PUNICODE_STRING pUnicodeString = nullptr;
    CHAR strModuleName[MAX_BASE_DLL_NAME_LEN]; ZERO_MEMORY(strModuleName, sizeof(strModuleName));
    DWORD_PTR dwptrModuleNameLength = 0;
    DWORD dwIndex = 0;
    WCHAR wchLetter = 0;
    QWORD qwModuleHash = 0;

    // Get the base address of the current thread's Thread Environment Block (TEB)
    pTeb = get_current_teb();
    if (pTeb == nullptr)
        return nullptr;

    // Get the base address of the current process's Process Environment Block (PEB)
	pPeb = pTeb->ProcessEnvironmentBlock;
	if (pPeb == nullptr)
        return nullptr;

    // Check if the hash of the target module name is 0, if true then return the base address of the calling module
	if (qwTargetModuleHash == 0) {
        pImageBase = pPeb->Reserved3[1]; // ntdll!_PEB.ImageBaseAddress (0x10)
        return pImageBase;
    }

    // Get the address of the PEB loader data - ntdll!_PEB_LDR_DATA structure (0x18)
    // Caution: race condition below since we cannot acquire the loader lock here before walking the loaded module list
    pPebLdrData = pPeb->Ldr;

    // Get the address of the circular doubly linked list head - ntdll!_LIST_ENTRY structure (0x20)
    pListHead = &(pPebLdrData->InMemoryOrderModuleList);

    // Get the address of the first entry - ntdll!_LIST_ENTRY structure (0x00)
    pNextEntry = pListHead->Flink;

    // Loop through all the list entries/loaded modules till we reach the head again
    while (pNextEntry != pListHead) {
        // Get the address of the current loader module entry - ntdll!_LDR_DATA_TABLE_ENTRY structure (0x10)
        pLdrDataTableEntry = CONTAINING_RECORD(pNextEntry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        // Get the address of the Unicode string representing the current module name - ntdll!_UNICODE_STRING structure
        pUnicodeString = BIT_CAST(PUNICODE_STRING, &pLdrDataTableEntry->Reserved4); // ntdll!_LDR_DATA_TABLE_ENTRY.BaseDllName (0x58)

        // Reset the ANSI string buffer for the current module
        ZERO_MEMORY(strModuleName, sizeof(strModuleName));

        // Get the length of the Unicode string representing the current module name
        dwptrModuleNameLength = std::min(pUnicodeString->Length / sizeof(WCHAR), sizeof(strModuleName));

        // Convert the current module name from Unicode string to ANSI string
        for (dwIndex = 0; dwIndex < dwptrModuleNameLength; dwIndex++) {
            // Extract each wide character letter
            wchLetter = *(pUnicodeString->Buffer + dwIndex);

            // Convert each wide character letter to lowercase
            if (wchLetter != L'_')
                wchLetter = wchLetter | 0x20U;

            // Check if each wide character letter is a valid US-ASCII character, else replace with "?"
            if (wchLetter > 0x7FU)
                wchLetter = '?';

            // Put each wide character letter into the ANSI string buffer
            strModuleName[dwIndex] = STATIC_CAST(CHAR, wchLetter);
        }

        // Get the 64-bit FNV-1a hash of the current module name
        qwModuleHash = HASH_STRING_RUN_TIME(strModuleName, dwptrModuleNameLength);

        // Check if the hash of the current module name matches the hash of the target module name
        if (qwModuleHash == qwTargetModuleHash) {
            // Return the base address of the current module
            pImageBase = pLdrDataTableEntry->DllBase;
            return pImageBase;
        }

        // Get the address of the next entry
        pNextEntry = pNextEntry->Flink;
    }

    return nullptr;
}

#pragma endregion
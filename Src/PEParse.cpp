// ========================================================================
// File: PEParse.cpp
//
// Author: winterknife
//
// Description: This source file contains routine(s) for parsing Windows
// Portable Executable (PE) files mapped in UVAS without any external
// dependencies including CRT, Win32, or Native API routines
//
// Modifications:
//  2025-09-25  Created
//  2025-11-12  Updated
// ========================================================================

// ========================================================================
// Includes
// ========================================================================

#include "PEParse.h"
#include <algorithm>

// ========================================================================
// Routines
// ========================================================================

#pragma region ROUTINES

_Use_decl_annotations_
PVOID __stdcall get_exported_symbol_address_by_hash(
    PVOID pImageBase,
    QWORD qwTargetExportHash
) {
    // Init local variables
    PIMAGE_DOS_HEADER pImageDosHeader = nullptr;
    DWORD dwElfanew = 0;
    PIMAGE_NT_HEADERS pImageNtHeaders = nullptr;
    PIMAGE_OPTIONAL_HEADER64 pImageOptionalHeader64 = nullptr;
    PIMAGE_DATA_DIRECTORY pImageDataDirectory = nullptr;
    DWORD dwExportDirectoryRva = 0;
    DWORD dwExportDirectorySize = 0;
    PIMAGE_EXPORT_DIRECTORY pImageExportDirectory = nullptr;
    DWORD dwTotalExportCount = 0;
    DWORD dwNamedExportCount = 0;
    PDWORD pdwAddressTable = nullptr;
    PDWORD pdwNameTable = nullptr;
    PWORD pwoNameOrdinalTable = nullptr;
    WORD woOrdinalBase = 0;
    WORD woExportOrdinal = 0;
    WORD woExportEatIndex = 0;
    DWORD dwIndex = 0;
    PCSTR strExportName = nullptr;
    DWORD_PTR dwptrExportNameLength = 0;
    QWORD qwExportHash = 0;
    BOOLEAN bFlag = false;
    PVOID pExportSymbol = nullptr;

    // Sanity check the function parameters
    if (pImageBase == nullptr || pImageBase == INVALID_HANDLE_VALUE)
        return nullptr;

    // Get the address of the DOS header - nt!_IMAGE_DOS_HEADER structure
    pImageDosHeader = STATIC_CAST(PIMAGE_DOS_HEADER, pImageBase);
    if (pImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return nullptr; // not a valid PE

    // Get the offset to the NT headers
    dwElfanew = pImageDosHeader->e_lfanew;
    if (dwElfanew >= (256U * 1024U * 1024U))
        return nullptr; // offset must not be larger than 256 MB

    // Get the address of the NT headers - nt!_IMAGE_NT_HEADERS32/64 structure
    pImageNtHeaders = STATIC_CAST(PIMAGE_NT_HEADERS, convert_rva_to_va(pImageBase, dwElfanew));
    if (pImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
        return nullptr; // not a valid PE

    // Check if the module is a 64-bit binary (PE32+)
    if (pImageNtHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        return nullptr; // PE32

    // Get the address of the Optional header - nt!_IMAGE_OPTIONAL_HEADER64 structure
    pImageOptionalHeader64 = &(pImageNtHeaders->OptionalHeader);

    // Get the address of the first data directory - nt!_IMAGE_DATA_DIRECTORY structure
    pImageDataDirectory = pImageOptionalHeader64->DataDirectory;

    // Get the RVA of the data for the export directory
    dwExportDirectoryRva = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (dwExportDirectoryRva == 0)
        return nullptr;

    // Get the size of the data for the export directory
    dwExportDirectorySize = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

    // Get the AVA of the data for the export directory - nt!_IMAGE_EXPORT_DIRECTORY structure
    pImageExportDirectory = STATIC_CAST(PIMAGE_EXPORT_DIRECTORY, convert_rva_to_va(pImageBase, dwExportDirectoryRva));

    // Get the total number of exports
    // WARNING: If EAF/EAF+ exploit mitigation is enabled, then the following line of code will crash the host process
    dwTotalExportCount = pImageExportDirectory->NumberOfFunctions;
    if (dwTotalExportCount == 0)
        return nullptr; // no exports

    // Get the number of named exports
    dwNamedExportCount = pImageExportDirectory->NumberOfNames;

    // Get the address of the array of RVAs of exported symbols - Export Address Table (EAT)
    pdwAddressTable = STATIC_CAST(PDWORD, convert_rva_to_va(pImageBase, pImageExportDirectory->AddressOfFunctions));

    // Get the address of the array of RVAs of exported symbol names as ANSI strings - Export Name Table (ENT)
    pdwNameTable = STATIC_CAST(PDWORD, convert_rva_to_va(pImageBase, pImageExportDirectory->AddressOfNames));

    // Get the address of the array of exported symbol name ordinals/zero-indexed indices for export names into the EAT - Export Name Ordinal Table (ENOT)
    pwoNameOrdinalTable = STATIC_CAST(PWORD, convert_rva_to_va(pImageBase, pImageExportDirectory->AddressOfNameOrdinals));

    // Get the starting ordinal number
    woOrdinalBase = LOWORD(pImageExportDirectory->Base); // default = 1

    // Check if we received an ordinal number
    if (HIWORD(qwTargetExportHash) == 0) {
        // Get the target ordinal value
        woExportOrdinal = LOWORD(qwTargetExportHash);
        if (woExportOrdinal < woOrdinalBase)
            return nullptr;

        // Get the zero-indexed index for the export ordinal into the EAT
        woExportEatIndex = woExportOrdinal - woOrdinalBase;
    }
    // Else we received a hash of the export name
    else if (qwTargetExportHash) {
        // Lookup the desired name in the name table using linear search
        for (dwIndex = 0; dwIndex < dwNamedExportCount; dwIndex++) {
            // Get the name of the current exported symbol
            strExportName = STATIC_CAST(PCSTR, convert_rva_to_va(pImageBase, pdwNameTable[dwIndex]));

            // Get the length of the null-terminated ANSI string representing the name of the current exported symbol
            dwptrExportNameLength = std::min(BIT_CAST(DWORD_PTR, scan_memory(strExportName, 0x00, MAX_EXPORTED_SYMBOL_NAME_LEN)) - BIT_CAST(DWORD_PTR, strExportName), MAX_EXPORTED_SYMBOL_NAME_LEN);

            // Get the 64-bit FNV-1a hash of the current exported symbol name
            qwExportHash = HASH_STRING_RUN_TIME(strExportName, dwptrExportNameLength);

            // Check if the hash of the current exported symbol name matches the hash of the target exported symbol name
            if (qwExportHash == qwTargetExportHash) {
                bFlag = true;
                break;
            }
        }

        // If the flag is not set, then a matching table entry was not found
        if (bFlag == false)
            return nullptr;

        // Get the zero-indexed index for the export name into the EAT
        woExportEatIndex = pwoNameOrdinalTable[dwIndex];
    }

    // Validate the ordinal by checking if the index is within the bounds of the EAT
    if (woExportEatIndex >= dwTotalExportCount)
        return nullptr;

    // Get the address of the exported symbol
    pExportSymbol = convert_rva_to_va(pImageBase, pdwAddressTable[woExportEatIndex]);

    // Check if the exported symbol RVA is a forwarder RVA by checking if the function pointer lies within the target module's export directory range
    if ((pExportSymbol > pImageExportDirectory) && (pExportSymbol < convert_rva_to_va(pImageExportDirectory, dwExportDirectorySize)))
        return nullptr;

    return pExportSymbol;
}

#pragma endregion
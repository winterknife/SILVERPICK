# SILVERPICK

## VERSION

- `v1.0`

## BRIEF

Project `SILVERPICK` is a `Windows User-Mode Shellcode Development Framework (WUMSDF)` whose sole purpose is to empower capability developers to build `Position Independent Code (PIC)` blobs for `Windows` `x64` using `C/C++` in an easy manner so as to reduce the development costs of such an endeavour.

It derives from project [WILDBEAST](https://github.com/winterknife/WILDBEAST) and, as such, leverages:
1. `Visual Studio Code` as the code editor
2. `MinGW-w64` toolchain as the compiler toolchain
3. `GNU Make` as the build system

## SETUP

You may find the setup instructions from here: [GCC-Clang-Setup-Windows](https://gist.github.com/winterknife/0b177a75a55bad895b19aad64cffa14f)

Please note that this project is using [MSYS2](https://www.msys2.org/).

## FEATURES

Writing shellcode in higher-level programming languages is nothing new, and countless blog posts and research papers have been published on the same since 2010. So, what is new in `SILVERPICK`?

Well, I am glad that you asked.

`SILVERPICK` has a nice little bag of tricks hidden up its sleeve, but most of all, this is my take on the subject.

So, without further ado, I present to you my first trick.

### TRICK 01

Ever since Matt Graeber popularised writing shellcode in `C`, most people have been using his [16-byte stack alignment stub written in `Assembly` language](https://github.com/mattifestation/PIC_Bindshell/blob/master/PIC_Bindshell/AdjustStack.asm).

While this is not a problem, seeing as how we aren't `IKEA`, assembly should not be necessary, and indeed it is not.

There exists a [`GCC` Function Attribute](https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html) that will emit the stack alignment stub for you.

Meet the `force_align_arg_pointer` function attribute in the form of a helpful [ALIGN_STACK](/Inc/Common.h#L84) macro, which generates the following assembly:
```asm
Disassembly of section .init:

<PicEntry>:
	push   rbp
	mov    rbp,rsp
	and    rsp,0xfffffffffffffff0
	sub    rsp,0x20
	call   <PicEntry+0x11>	IMAGE_REL_AMD64_REL32	.text$payload
	leave
	ret
```

What's the `.init` section, you ask? Well, that serves as a nice segue into my second trick.

### TRICK 02

Matt Graeber might have popularised writing shellcode in `C` at some point, but really, it was Paul Ungur who revived this black art with [Stardust](https://github.com/Cracked5pider/Stardust).

Now, `Stardust` uses a [Binutils linker script](https://sourceware.org/binutils/docs/ld/Scripts.html) to control the placement of functions and data into the appropriate `PE` section in the correct order. This technique itself is derived from Austin Hudson's work, and many people use a variant of his linker scripts.

While linker scripts are great for linker section ordering, if all you need is to place a certain function at the beginning of the code section, they are unnecessary.

Enter the `section` function attribute with a special section name called `.init`, which indicates to the linker that the function contains pre-`main()` runtime initialization code and must be _first_ in the link order.

To this effect, the [CODE_BEGIN](/Inc/Common.h#L81) macro has been created.

### TRICK 03

For my third trick, I present to you the [STACK_STRING](/Inc/StackString.h#L35) macro.

In `C`, you can create a stack string (a string that is dynamically built on the stack) by declaring the string literal as an array of `ANSI` characters:
```c
char charrHelloKitty[] = { 'H', 'e', 'l', 'l', 'o', 'K', 'i', 't', 't', 'y', '\0' };
```

In `C++`, you can create a stack string by simply marking a `char` array as `constexpr`:
```cpp
constexpr char charrHelloKitty[]{ "HelloKitty" };
```

However, both of these techniques are rendered useless in the face of compiler optimizations _if_ the string literals are sufficiently large, unlike our solution, which will work regardless of the string length and the level of compiler optimizations, thanks to some clever `C++` template metaprogramming hack courtesy of Can Bölük.

Using this macro is pretty straightforward:
```c
STACK_STRING(sstrText, "an extra long hello world!");
STACK_STRING(sstrCaption, "Demo");

MessageBoxA(nullptr, sstrText.data(), sstrCaption.data(), MB_OK);
```

This will generate the following assembly:
```asm
mov     [rsp+58h+var_23], 61h ; 'a'
mov     [rsp+58h+var_22], 6Eh ; 'n'
mov     [rsp+58h+var_21], 20h ; ' '
mov     [rsp+58h+var_20], 65h ; 'e'
mov     [rsp+58h+var_1F], 78h ; 'x'
mov     [rsp+58h+var_1E], 74h ; 't'
mov     [rsp+58h+var_1D], 72h ; 'r'
mov     [rsp+58h+var_1C], 61h ; 'a'
mov     [rsp+58h+var_1B], 20h ; ' '
mov     [rsp+58h+var_1A], 6Ch ; 'l'
mov     [rsp+58h+var_19], 6Fh ; 'o'
mov     [rsp+58h+var_18], 6Eh ; 'n'
mov     [rsp+58h+var_17], 67h ; 'g'
mov     [rsp+58h+var_16], 20h ; ' '
mov     [rsp+58h+var_15], 68h ; 'h'
mov     [rsp+58h+var_14], 65h ; 'e'
mov     [rsp+58h+var_13], 6Ch ; 'l'
mov     [rsp+58h+var_12], 6Ch ; 'l'
mov     [rsp+58h+var_11], 6Fh ; 'o'
mov     [rsp+58h+var_10], 20h ; ' '
mov     [rsp+58h+var_2F], 0
mov     [rsp+58h+var_F], 77h ; 'w'
mov     [rsp+58h+var_E], 6Fh ; 'o'
mov     [rsp+58h+var_D], 72h ; 'r'
mov     [rsp+58h+var_C], 6Ch ; 'l'
mov     [rsp+58h+var_B], 64h ; 'd'
mov     [rsp+58h+var_A], 21h ; '!'
mov     [rsp+58h+var_33], 44h ; 'D'
mov     [rsp+58h+var_32], 65h ; 'e'
mov     [rsp+58h+var_31], 6Dh ; 'm'
mov     [rsp+58h+var_30], 6Fh ; 'o'
```

### TRICK 04

Speaking of `C++`, I present to you compile-time string hashing for my fourth trick.

While this is not a novel concept, `SILVERPICK` offers some improvements over existing public implementations.

Firstly, we use the 64-bit variant of the popular `FNV-1a` non-cryptographic hash function in order to reduce the probability of a successful hash collision attack.

Secondly, we use a modified parameter for the hash function to defend against [precalculated hash table lookups](https://cloud.google.com/blog/topics/threat-intelligence/precalculated-string-hashes-reverse-engineering-shellcode/), such as [HashDB](https://github.com/OALabs/hashdb). Crucially, this does _not_ change the properties of the hash function.

To hash a short string at run time, simply use the [HASH_STRING_RUN_TIME](/Inc/HashString.h#L45) macro.

To hash a short string literal at compile time, simply use the [HASH_STRING_COMPILE_TIME](/Inc/HashString.h#L42) macro. Compile-time only evaluation is guaranteed via `consteval`.

### TRICK 05

It turns out that you can implement quite a handful of `C Runtime Library (CRT)` functions using `x86` string instructions. So, of course, I had to implement them using a mix of compiler intrinsics and inline assembly.

Do you want to use the `msvcrt!memset` function in your code? Use the [ZERO_MEMORY](/Inc/Common.h#L123) macro instead, which uses the `rep stosb` instruction emitted via a compiler intrinsic.

How about the `msvcrt!memcpy` function or the `msvcrt!memmove` function, you ask? Meet the [COPY_MEMORY](/Inc/Common.h#L126) macro as a replacement, which uses the `rep movsb` instruction emitted via a compiler intrinsic.

But what about an alternative to the `msvcrt!memcmp` function? It turns out that there isn't exactly a compiler intrinsic available to emit the `repe cmpsb` instruction. So, we write a [compare_memory](/Inc/Common.h#L141) function using inline assembly instead.

Finally, if you seek a replacement for the `msvcrt!memchr` function, meet the [scan_memory](/Inc/Common.h#L167) function, which once again uses inline assembly since there is no compiler intrinsic available to emit the `repne scasb` instruction.

Oh, and did I forget to mention that you could write your own safer version of the `msvcrt!strlen` function using the `scan_memory` routine like so:
```cpp
DWORD_PTR dwptrExportNameLength = std::min(BIT_CAST(DWORD_PTR, scan_memory(strExportName, 0x00, MAX_EXPORTED_SYMBOL_NAME_LEN)) - BIT_CAST(DWORD_PTR, strExportName), MAX_EXPORTED_SYMBOL_NAME_LEN);
```

Please note that these implementations may not produce the most performant code, depending on the target `CPU` microarchitecture. However, they are guaranteed to get the job done.

### TRICK 06

Interested in more parlour tricks?

There are a lot of other small macros in [Common.h](/Inc/Common.h) that exist to abstract away some of the complexities of massaging the compiler.

A dependency-less implementation of the [GetModuleHandle](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea) function is provided in [UserModuleBase.cpp](/Src/UserModuleBase.cpp). To simplify ease-of-use, a helpful macro named [GET_USER_MODULE_BASE](/Inc/UserModuleBase.h#L36) has been created.

Similarly, a dependency-less implementation of the [GetProcAddress](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getprocaddress) function is provided in [PEParse.cpp](/Src/PEParse.cpp), which is then wrapped in a handy macro aptly named [GET_EXPORTED_SYMBOL_ADDRESS](/Inc/PEParse.h#L36). Furthermore, two more macros have been provided to aid in run-time dynamic linking - [INITIALIZE_FUNCTION_POINTER](/Inc/PEParse.h#L39) to declare and initialize a function pointer to 0, and [RESOLVE_FUNCTION_POINTER](/Inc/PEParse.h#L42) to resolve said function pointer.

[Visual Studio Code integration](/.vscode/) is built into the project so that developers may use the `Ctrl+Shift+B` keyboard shortcut for a no-fuss build process.

[GitHub Actions integration](/.github/workflows/build.yml) is also built into the project to enable `CI` builds.

The project takes some pride in its well-organized structure, as well as the thoroughly commented and relatively clean code.

Finally, take a gander at the project [Makefile](/Makefile), which contains the choicest selection of compiler and linker flags that will generate small, secure, and `OPSEC`-friendly code. Meanwhile, verbose logging and a generated linker map file will provide visibility into the build process to facilitate a deeper understanding of the toolchain. Additionally, each translation unit also produces a disassembly file that, upon inspection, will often make you say things like "the compiler did what now?", etc.

## USAGE

If you are sold on the framework, this section describes how you would make use of it.

The following is an excerpt taken from [PicMain.cpp](/Src/PicMain.cpp):
```cpp
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
```

Seems easy enough, no?

While writing `PIC` in `C/C++` using the `SILVERPICK` framework, you have to take into consideration the following rules:
1. Treat the `payload` function as you would the `main` function of a traditional program, i.e., as the _(pseudo) entry point_.
2. All string literals must be declared as _stack strings_.
3. _Global variables_ may not be used anywhere in the code.
4. `Windows API` or `Native API` functions may be used only via _run-time dynamic linking_ after ensuring that the function prototype is available in the corresponding header file.

## FUTURE ENHANCEMENTS

The section contains a non-exhaustive list of planned enhancements that will be integrated into a future project.

- [ ] Switch to `Clang/LLVM` toolchain.
- [ ] Switch to a different build system.
- [ ] Compile-time string obfuscation that works with stack strings and is resistant to [FLOSS](https://github.com/mandiant/flare-floss).
- [ ] Compile-time string hashing using seeded non-cryptographic custom hash function.
- [ ] Alternate method to retrieve `GS` segment base address.
- [ ] Capability to bypass `Export Address Filtering (EAF)` exploit mitigation.

## REFERENCES

The following is a list of references arranged in chronological order that proved invaluable to me during my research and were extensively used as inspiration for this project:

1. [Writing Shellcode with a C Compiler](https://nickharbour.wordpress.com/2010/07/01/writing-shellcode-with-a-c-compiler/) by Nick Harbour (2010)

2. [Shellcode with a C-compiler](https://blog.didierstevens.com/programs/shellcode/) by Didier Stevens (2010)

3. [Writing Optimized Windows Shellcode in C](https://web.archive.org/web/20201202085848/http://www.exploit-monday.com/2013/08/writing-optimized-windows-shellcode-in-c.html) by Matt Graeber (2013)

4. [Shellcode the better way, or how to just use your compiler](https://phrack.org/issues/69/4) by Justin Fisher (2016)

5. [ShellcodeStdio](https://winternl.com/shellcodestdio/) by Jack Ullrich (2016)

6. [Shellcode: A Windows PIC using RSA-2048 key exchange, AES-256, SHA-3](https://web.archive.org/web/20240316160314/https://modexp.wordpress.com/2016/12/26/windows-pic/) by Odzhan (2016)

7. [Writing Optimized Windows Shellcode](https://dimitrifourny.github.io/2017/04/28/optimized-shellcode.html) by Dimitri Fourny (2017)

8. [Writing and Compiling Shellcode in C](https://www.ired.team/offensive-security/code-injection-process-injection/writing-and-compiling-shellcode-in-c) by Aleksandra Doniec and Mantvydas Baranauskas (2021)

9. [Creating Shellcode from any Code Using Visual Studio and C++](https://www.codeproject.com/articles/Creating-Shellcode-from-any-Code-Using-Visual-Stud#comments-section) by Hamid Memar (2021)

10. [Writing Optimized Windows Shellcode in C](https://phasetw0.com/malware/writing-optimized-windows-shellcode-in-c/) by Philip Woldhek (2021)

11. [From C, with inline assembly, to shellcode](https://steve-s.gitbook.io/0xtriboulet/archive/notice/just-malicious/from-c-with-inline-assembly-to-shellcode) by Steve Salinas (2023)

12. [How To Craft Your Own Windows x86/64 Shellcode with Visual Studio](https://xacone.github.io/custom_shellcode.html) by Yazid Benjamaa (2023)

13. [Modern implant design: position independent malware development](https://5pider.net/blog/2024/01/27/modern-shellcode-implant-design) by Paul Ungur (2024)

14. [From C to shellcode (simple way)](https://print3m.github.io/blog/from-c-to-shellcode) by Print3M (2024)

15. [relocatable](https://github.com/tijme/relocatable) by Tijme Gommers (2025)

16. [PIC Development Crash Course](https://player.vimeo.com/video/1100089433) by Raphael Mudge (2025)
# References
# 01. https://makefiletutorial.com/
# 02. https://web.stanford.edu/class/archive/cs/cs107/cs107.1174/guide_make.html
# 03. https://www.gnu.org/software/make/manual/make.html
# 04. https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/Option-Index.html
# 05. https://www3.ntu.edu.sg/home/ehchua/programming/cpp/gcc_make.html
# 06. https://nullprogram.com/blog/2023/02/15/
# 07. https://github.com/pts/pts-tinype
# 08. https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html
# 09. https://www.man7.org/linux/man-pages/man1/ld.1.html
# 10. https://gcc.gnu.org/onlinedocs/gcc-15.1.0/gcc/Cygwin-and-MinGW-Options.html
# 11. https://www.renenyffenegger.ch/notes/development/languages/C-C-plus-plus/GCC/options/
# 12. https://gist.github.com/tomdaley92/190c68e8a84038cc91a5459409e007df
# 13. https://keasigmadelta.com/blog/cmake-vs-make-a-real-life-comparison-with-actual-code/
# 14. https://www.man7.org/linux/man-pages/man1/windres.1.html
# 15. https://airbus-seclab.github.io/c-compiler-security/
# 16. https://nullprogram.com/blog/2023/04/29/
# 17. https://interrupt.memfault.com/blog/code-size-optimization-gcc-flags
# 18. https://interrupt.memfault.com/blog/best-and-worst-gcc-clang-compiler-flags
# 19. https://www.incredibuild.com/blog/compiling-with-clang-optimization-flags
# 20. https://clang.llvm.org/docs/genindex.html
# 21. https://sourceware.org/binutils/docs/ld/WIN32.html
# 22. https://www.man7.org/linux/man-pages/man1/objdump.1.html
# 23. https://www.man7.org/linux/man-pages/man1/objcopy.1.html
# 24. https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run/14061796#14061796
# 25. https://dev.to/deciduously/how-to-make-a-makefile-1dep/

# Project name
PROJECT := SilverPick

# The flags given to make
MAKEFLAGS += --no-builtin-rules     # eliminate use of the built-in implicit rules
MAKEFLAGS += --no-builtin-variables # eliminate use of the built-in rule-specific variables

# Program for compiling C++ programs
CXX = gcc

# Extra flags to give to the C preprocessor and programs that use it
CPPFLAGS := $(addprefix -I,Inc) # Specify the directory to be searched for header files during preprocessing

# Extra flags to give to the C++ compiler
CXXFLAGS := -c                              # compile the source files, but do not link
CXXFLAGS += -Wall                           # enable all the warnings
CXXFLAGS += -Wextra                         # enable some extra warnings
CXXFLAGS += -Werror                         # turn all warnings into errors
CXXFLAGS += -Wshadow                        # issue a warning when a variable declaration hides a previous declaration
CXXFLAGS += -pedantic-errors                # disable compiler extensions
CXXFLAGS += -Wconversion                    # warn for implicit conversions that may alter a value
CXXFLAGS += -Wundef                         # warn if an undefined identifier is evaluated in an #if directive
CXXFLAGS += -Wpadded                        # warn if padding is added to a structure due to alignment requirements
CXXFLAGS += -fdiagnostics-color=always      # use color in diagnostics
CXXFLAGS += -std=c++20                      # use C++20 standard
CXXFLAGS += -Os                             # optimize for size
CXXFLAGS += -flto                           # enable whole program Link Time Optimization (LTO)
CXXFLAGS += -ffat-lto-objects               # generate a fat LTO object that has both a true object and a discardable IR section
CXXFLAGS += -m64                            # generate code for the x86-64 architecture
CXXFLAGS += -fno-stack-protector            # disable stack protection
CXXFLAGS += -mno-stack-arg-probe            # disable stack probing
CXXFLAGS += -fno-ident                      # do not emit metadata containing compiler name and version
CXXFLAGS += -fno-exceptions                 # disable exception handling
CXXFLAGS += -fno-unwind-tables              # suppress generation of static unwind tables
CXXFLAGS += -fno-asynchronous-unwind-tables # prevent generation of the .pdata section that contains the unwind tables
CXXFLAGS += -fno-builtin                    # don't recognize built-in functions that do not begin with __builtin_ as prefix
CXXFLAGS += -ffunction-sections             # place each function in a separate section in the resulting object file
CXXFLAGS += -fdata-sections                 # place each data item in a separate section in the resulting object file
CXXFLAGS += -fno-jump-tables                # do not use jump tables for switch statements
CXXFLAGS += -fno-toplevel-reorder           # do not reorder top-level functions, variables, and asm statements
CXXFLAGS += -fpack-struct=8                 # specify maximum alignment for structure member packing
CXXFLAGS += -fms-extensions                 # accept some non-standard constructs used in Microsoft header files
CXXFLAGS += -fno-rtti                       # disable run-time type information
CXXFLAGS += -fomit-frame-pointer            # omit the frame pointer in functions that don't need one
CXXFLAGS += -masm=intel                     # output assembly instructions using selected dialect
CXXFLAGS += -v                              # enable verbose mode

# Extra flags to give to the compiler when they are supposed to invoke the linker
LDFLAGS := -fuse-ld=bfd                     # use the bfd linker instead of the default linker
LDFLAGS += -nostdlib                        # do not use the standard system startup files or libraries when linking
ifeq (pic,$(firstword $(MAKECMDGOALS)))
LDFLAGS += -Wl,--entry=PicEntry             # specify the program entry point
else ifeq (exe,$(firstword $(MAKECMDGOALS)))
LDFLAGS += -Wl,--entry=ExeEntry             # specify the program entry point
else ifeq (dll,$(firstword $(MAKECMDGOALS)))
LDFLAGS += -Wl,--entry=DllEntry             # specify the program entry point
LDFLAGS += -shared                          # create a DLL instead of a regular executable
LDFLAGS += -Wl,--dll                        # create a DLL instead of a regular executable
LDFLAGS += -Wl,--exclude-all-symbols        # specify that no symbols should be automatically exported
endif
LDFLAGS += -Wl,--subsystem=windows:6.1      # specify the subsystem and the subsystem version targeted by the executable
LDFLAGS += -Wl,--dynamicbase                # use ASLR
LDFLAGS += -Wl,--high-entropy-va            # use HEASLR
ifeq (dll,$(firstword $(MAKECMDGOALS)))
LDFLAGS += -Wl,--image-base=0x180000000     # set the base address for the program
else
LDFLAGS += -Wl,--image-base=0x140000000     # set the base address for the program
endif
LDFLAGS += -Wl,--nxcompat                   # indicate that the executable is compatible with DEP
ifeq (pic,$(firstword $(MAKECMDGOALS)))
LDFLAGS += -Wl,--no-seh                     # the image does not use SEH
else
LDFLAGS += -Wl,--disable-no-seh             # the image uses SEH
endif
ifeq (dll,$(firstword $(MAKECMDGOALS)))
LDFLAGS += -Wl,--disable-tsaware            # the image isn't Terminal Server aware
else
LDFLAGS += -Wl,--tsaware                    # the image is Terminal Server aware
endif
LDFLAGS += -Wl,--no-insert-timestamp        # suppress generation of the PE file header timestamp
LDFLAGS += -Wl,--strip-all                  # remove all symbol table and relocation information from the executable
LDFLAGS += -Wl,--gc-sections                # enable garbage collection of unused input sections
LDFLAGS += -Wl,--oformat=pei-x86-64         # specify the binary format for the output object file
LDFLAGS += -Wl,--major-os-version=6         # set the major number of the OS version
LDFLAGS += -Wl,--minor-os-version=1         # set the minor number of the OS version
LDFLAGS += -Wl,--cref                       # output a cross reference table
LDFLAGS += -Wl,-Map=Temp/$(PROJECT)_x64.map # generate linker map file
LDFLAGS += -Wl,--disable-auto-import        # don't automatically import data symbols from other DLLs without dllimport
LDFLAGS += -Wl,-static                      # do not link against shared libraries
LDFLAGS += -Wl,--verbose                    # enable verbose mode

# Library flags or names given to the compiler when they are supposed to invoke the linker
ifneq (pic,$(firstword $(MAKECMDGOALS)))
LDLIBS := -lkernel32 # search kernel32 import library when linking
LDLIBS += -luser32   # search user32 import library when linking
LDLIBS += -lmsvcrt   # search msvcrt import library when linking
endif

# Source files
SOURCES := $(wildcard Src/*.cpp)

# Resource script files
RESOURCES := $(wildcard Src/*.rc)

# Object files
OBJECTS64 := $(patsubst Src/%.cpp, Temp/%_x64.obj, ${SOURCES})
OBJECTS64 += $(patsubst Src/%.rc, Temp/%_x64.obj, ${RESOURCES})

# Default target
.DEFAULT_GOAL := help

# Display help
.PHONY: help
help:
	@ echo "[*] usage: make {exe|dll|pic|clean|help}"

# PIC target
.PHONY: pic
pic: Bin/$(PROJECT)_x64.bin

# DLL target
.PHONY: dll
dll: Bin/$(PROJECT)_x64.dll

# EXE target
.PHONY: exe
exe: Bin/$(PROJECT)_x64.exe

# Build PIC (x64)
Bin/$(PROJECT)_x64.bin: Bin/$(PROJECT)_x64.exe
	@ echo "[*] extracting .text section from PE file..."
	@ objcopy --target=pei-x86-64 --dump-section .text=$@ $<
	@ xxd -include -u $@ $(dir $@)/Shellcode.h
	@ echo "[+] done."

# Build DLL (x64)
Bin/$(PROJECT)_x64.dll: $(OBJECTS64)
	@ echo "[*] linking COFF file(s) into PE file..."
	@ mkdir -p $(dir $@)
	@ $(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
	@ echo "[+] done."

# Build EXE (x64)
Bin/$(PROJECT)_x64.exe: $(OBJECTS64)
	@ echo "[*] linking COFF file(s) into PE file..."
	@ mkdir -p $(dir $@)
	@ $(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
	@ echo "[+] done."

# Compile (x64)
Temp/%_x64.obj: Src/%.cpp
	@ echo "[*] compiling C/C++ code into COFF file..."
	@ mkdir -p $(dir $@)
	@ $(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@ objdump --disassemble --disassembler-options=intel --reloc --wide --no-addresses --no-show-raw-insn $@ > $@.disasm
	@ echo "[+] done."

Temp/%_x64.obj: Src/%.rc
	@ echo "[*] compiling resource file into COFF file..."
	@ mkdir -p $(dir $@)
	@ windres --input=$< --output=$@ --output-format=coff --target=pe-x86-64 --include-dir=Inc --verbose
	@ echo "[+] done."

# Cleanup
.PHONY: clean
clean:
	@ echo "[*] cleaning up..."
	@ find . -name "*.obj" -type f -printf "removed '%f'\n" -delete
	@ find . -name "*.exe" -type f -printf "removed '%f'\n" -delete
	@ find . -name "*.dll" -type f -printf "removed '%f'\n" -delete
	@ find . -name "*.bin" -type f -printf "removed '%f'\n" -delete
	@ find . -name "*.map" -type f -printf "removed '%f'\n" -delete
	@ find . -name "*.disasm" -type f -printf "removed '%f'\n" -delete
	@ echo "[+] done."

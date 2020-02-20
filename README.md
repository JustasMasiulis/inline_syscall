# inline_syscall [![](https://img.shields.io/badge/OS-windows-green.svg)]() [![](https://img.shields.io/badge/compiler-clang-green.svg)]() [![](https://img.shields.io/badge/arch-x64-green.svg)]()
Header only library that allows you to generate direct syscall instructions in an optimized, inlineable and easy to use manner.

## How to use
All you have to do is copy over the header files and call the initialization function `init_syscalls_list` before using the `INLINE_SYSCALL(function_pointer)` and `INLINE_SYSCALL_T(function_type)` macros.

```cpp
// This header contains the initialization function.
// If you already initialized, inline_syscall.hpp contains all you need.
#include "inline_syscall/include/in_memory_init.hpp"

// Needs to be called once at startup before INLINE_SYSCALL is used.
jm::init_syscalls_list();

// Usage of the main macro INLINE_SYSCALL
void* allocation = nullptr;
SIZE_T size      = 0x1000;
NTSTATUS status  = INLINE_SYSCALL(NtAllocateVirtualMemory)((HANDLE)-1, &allocation, 0, &size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
```

## What code does it generate
As one of the main goals of this library is to be as optimized as possible here is the output of an optimized build.
```asm
mov qword ptr [rsp+30h], 0                  ; void* allocation = nullptr
mov qword ptr [rsp+28h], 1000h              ; SIZE_T size      = 0x1000;
mov eax, dword ptr [entry (07FF683157004h)] ; syscall id is loaded
lea rdx, [rsp+30h]                          ; BaseAddress     = &allocation
lea r9, [rsp+28h]                           ; RegionSize      = &size
mov r10, 0FFFFFFFFFFFFFFFFh                 ; ProcessHandle   = -1
xor r8d,r8d                                 ; ZeroBits        = 0
sub rsp,40h                                 ; preparing stack
mov qword ptr [type],3000h                  ; AllocationType  = MEM_RESERVE | MEM_COMMIT
mov qword ptr [protect], 4                  ; Protect         = PAGE_READWRITE
syscall                                     ; syscall instruction itself
add rsp,40h                                 ; restoring stack
```

## FAQ
* Q: What are the main uses of this? A: Obfuscation and hook avoidance.
* Q: Why would I use this over some other library? A: The code this generates can be inlined and it is optimized for every single parameter count as much as possible.
* Q: Why can't this work on MSVC? A: MSVC doesn't support GCC style inline assembly which can be properly optimized and worked on by compiler.
* Q: Why can't this work on GCC? A: Contrary to MSVC GCC is too good at optimizing inline assembly and as such breaks my code that tries to be somewhat generic.

## Creating your own initialization function
This library enables you to create your own custom initialization routines that are more resilent against missing syscalls or acquire syscall ids in some other way.

`JM_INLINE_SYSCALL_ENTRY_TYPE` can be defined with your own syscall entry type that needs to have `hash` field. By default `syscall_entry_small` is used, but `syscall_entry_full` is also shipped.

If you want to use the provided `INLINE_SYSCALL` macro you will need to use the provided `jm::hash` function.

To acquire the start of syscall entries you need to call `jm::syscall_entries()` and iterate untill you hit a zero entry.
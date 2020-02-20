/*
 * Copyright 2018-2020 Justas Masiulis
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JM_INLINE_SYSCALL_IN_MEMORY_INIT_HPP
#define JM_INLINE_SYSCALL_IN_MEMORY_INIT_HPP

#include "inline_syscall.hpp"
#include <intrin.h>

namespace jm {

    /// \brief Initializes syscalls list.
    inline void init_syscalls_list();

    namespace detail {

        struct IMAGE_DOS_HEADER { // DOS .EXE header
            unsigned short e_magic; // Magic number
            unsigned short e_cblp; // Bytes on last page of file
            unsigned short e_cp; // Pages in file
            unsigned short e_crlc; // Relocations
            unsigned short e_cparhdr; // Size of header in paragraphs
            unsigned short e_minalloc; // Minimum extra paragraphs needed
            unsigned short e_maxalloc; // Maximum extra paragraphs needed
            unsigned short e_ss; // Initial (relative) SS value
            unsigned short e_sp; // Initial SP value
            unsigned short e_csum; // Checksum
            unsigned short e_ip; // Initial IP value
            unsigned short e_cs; // Initial (relative) CS value
            unsigned short e_lfarlc; // File address of relocation table
            unsigned short e_ovno; // Overlay number
            unsigned short e_res[4]; // Reserved words
            unsigned short e_oemid; // OEM identifier (for e_oeminfo)
            unsigned short e_oeminfo; // OEM information; e_oemid specific
            unsigned short e_res2[10]; // Reserved words
            long           e_lfanew; // File address of new exe header
        };

        struct IMAGE_FILE_HEADER {
            unsigned short Machine;
            unsigned short NumberOfSections;
            unsigned long  TimeDateStamp;
            unsigned long  PointerToSymbolTable;
            unsigned long  NumberOfSymbols;
            unsigned short SizeOfOptionalHeader;
            unsigned short Characteristics;
        };

        struct IMAGE_EXPORT_DIRECTORY {
            unsigned long  Characteristics;
            unsigned long  TimeDateStamp;
            unsigned short MajorVersion;
            unsigned short MinorVersion;
            unsigned long  Name;
            unsigned long  Base;
            unsigned long  NumberOfFunctions;
            unsigned long  NumberOfNames;
            unsigned long  AddressOfFunctions; // RVA from base of image
            unsigned long  AddressOfNames; // RVA from base of image
            unsigned long  AddressOfNameOrdinals; // RVA from base of image
        };

        struct IMAGE_DATA_DIRECTORY {
            unsigned long VirtualAddress;
            unsigned long Size;
        };

        struct IMAGE_OPTIONAL_HEADER64 {
            unsigned short       Magic;
            unsigned char        MajorLinkerVersion;
            unsigned char        MinorLinkerVersion;
            unsigned long        SizeOfCode;
            unsigned long        SizeOfInitializedData;
            unsigned long        SizeOfUninitializedData;
            unsigned long        AddressOfEntryPoint;
            unsigned long        BaseOfCode;
            unsigned long long   ImageBase;
            unsigned long        SectionAlignment;
            unsigned long        FileAlignment;
            unsigned short       MajorOperatingSystemVersion;
            unsigned short       MinorOperatingSystemVersion;
            unsigned short       MajorImageVersion;
            unsigned short       MinorImageVersion;
            unsigned short       MajorSubsystemVersion;
            unsigned short       MinorSubsystemVersion;
            unsigned long        Win32VersionValue;
            unsigned long        SizeOfImage;
            unsigned long        SizeOfHeaders;
            unsigned long        CheckSum;
            unsigned short       Subsystem;
            unsigned short       DllCharacteristics;
            unsigned long long   SizeOfStackReserve;
            unsigned long long   SizeOfStackCommit;
            unsigned long long   SizeOfHeapReserve;
            unsigned long long   SizeOfHeapCommit;
            unsigned long        LoaderFlags;
            unsigned long        NumberOfRvaAndSizes;
            IMAGE_DATA_DIRECTORY DataDirectory[16];
        };

        struct IMAGE_NT_HEADERS {
            unsigned long           Signature;
            IMAGE_FILE_HEADER       FileHeader;
            IMAGE_OPTIONAL_HEADER64 OptionalHeader;
        };

        JM_INLINE_SYSCALL_FORCEINLINE const IMAGE_NT_HEADERS* nt_headers(
            const char* base) noexcept
        {
            return reinterpret_cast<const IMAGE_NT_HEADERS*>(
                base + reinterpret_cast<const IMAGE_DOS_HEADER*>(base)->e_lfanew);
        }

        struct exports_directory {
            const char*                   _base;
            const IMAGE_EXPORT_DIRECTORY* _ied;

        public:
            using size_type = unsigned long;

            JM_INLINE_SYSCALL_FORCEINLINE exports_directory(const char* base) noexcept
                : _base(base)
            {
                const auto ied_data_dir =
                    nt_headers(base)->OptionalHeader.DataDirectory[0];
                _ied = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(
                    base + ied_data_dir.VirtualAddress);
            }

            JM_INLINE_SYSCALL_FORCEINLINE size_type size() const noexcept
            {
                return _ied->NumberOfNames;
            }

            JM_INLINE_SYSCALL_FORCEINLINE const char* name(size_type index) const noexcept
            {
                return reinterpret_cast<const char*>(
                    _base + reinterpret_cast<const unsigned long*>(
                                _base + _ied->AddressOfNames)[index]);
            }

            JM_INLINE_SYSCALL_FORCEINLINE const char* address(size_type index) const
                noexcept
            {
                const auto* const rva_table = reinterpret_cast<const unsigned long*>(
                    _base + _ied->AddressOfFunctions);

                const auto* const ord_table = reinterpret_cast<const unsigned short*>(
                    _base + _ied->AddressOfNameOrdinals);

                return _base + rva_table[ord_table[index]];
            }
        };

        JM_INLINE_SYSCALL_FORCEINLINE const void* ntdll_base() noexcept
        {
            struct ldr_entry_t {
                ldr_entry_t* Flink;
                void*        _[5];
                char*        DllBase;
            };

            // CBA to copy over 4 structures just for this
            // TEB->ProcessEnvironmentBlock
            const auto peb = *reinterpret_cast<const char**>(__readgsqword(0x30) + 0x60);
            // PEB->Ldr
            const auto ldr_data = *reinterpret_cast<const char* const*>(peb + 0x18);
            // LDR->InLoadOrderModuleList
            const auto ldr_entry =
                *reinterpret_cast<const ldr_entry_t* const*>(ldr_data + 0x10);

            return ldr_entry->Flink->DllBase;
        }

    } // namespace detail

    /// \brief Initializes syscall ids with information from ntdll.dll loaded in current
    ///        process.
    /// \warning THIS DOES NOT INITIALIZE SYSCALLS FROM USER32.DLL / NtUser*
    JM_INLINE_SYSCALL_FORCEINLINE void init_syscalls_list()
    {
        detail::exports_directory exports(static_cast<const char*>(detail::ntdll_base()));
        for(auto i = exports.size();; --i) {
            auto entry = jm::syscall_entries();
            while(entry->hash != 0) {
                if(jm::hash(exports.name(i)) == entry->hash) {
                    entry->id =
                        *reinterpret_cast<const std::int32_t*>(exports.address(i) + 4);
                    break;
                }
                ++entry;
            }

            if(i == 0)
                break;
        }
    }

} // namespace jm

#endif // JM_INLINE_SYSCALL_IN_MEMORY_INIT_HPP
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

#ifndef JM_INLINE_SYSCALL_INL
#define JM_INLINE_SYSCALL_INL

#include "inline_syscall.hpp"

#if defined(_MSC_VER)
#define JM_INLINE_SYSCALL_FORCEINLINE __forceinline
#else
#define JM_INLINE_SYSCALL_FORCEINLINE __attribute__((always_inline))
#endif

// helper macro to reduce the typing a bit
#define JM_INLINE_SYSCALL_STUB(...) \
    JM_INLINE_SYSCALL_FORCEINLINE std::int32_t syscall(__VA_ARGS__) noexcept

namespace jm {

    // hash used for the syscall names
    inline constexpr std::uint32_t hash(const char* str) noexcept
    {
        std::uint32_t value = 2166136261;

        str += 2;
        for(;;) {
            const char c = *str++;
            if(!c)
                return value;

            value = static_cast<std::uint32_t>((value ^ c) * 16777619ull);
        }
    }

    constexpr syscall_entry_full::syscall_entry_full(std::uint32_t hash_) noexcept
        : hash(hash_)
    {}

    namespace detail {

        // stores syscall info in a section that we create
        // Because we store it in its own section we can initialize all values like an
        // array
        template<std::uint32_t Hash>
        struct syscall_holder {
            [[gnu::section(
                "_sysc")]] inline static JM_INLINE_SYSCALL_ENTRY_TYPE entry{ Hash };
        };

        // we instantiate the first entry with 0 hash to be able to get a pointer
        template struct syscall_holder<0>;

        // disables register keyword deprecation warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wregister"

        /* syscall stubs begin here.
         *
         * They all seem more or less the same and that's true, however
         * we need them like this for best possible code generation.
         */

        JM_INLINE_SYSCALL_STUB(std::uint32_t id)
        {
            register void* a1 asm("r10");
            void*          a2;
            register void* a3 asm("r8");
            register void* a4 asm("r9");

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("syscall\n"
                         : "=a"(status),
                           "=r"(a1),
                           "=d"(a2),
                           "=r"(a3),
                           "=r"(a4),
                           "=c"(unused_output),
                           "=r"(unused_output2)
                         : "a"(id)
                         : "memory", "cc");
            return status;
        }

        template<class T1>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1)
        {
            register auto  a1 asm("r10") = _1;
            void*          a2;
            register void* a3 asm("r8");
            register void* a4 asm("r9");

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("syscall\n"
                         : "=a"(status),
                           "=r"(a1),
                           "=d"(a2),
                           "=r"(a3),
                           "=r"(a4),
                           "=c"(unused_output),
                           "=r"(unused_output2)
                         : "a"(id), "r"(a1)
                         : "memory", "cc");
            return status;
        }

        template<class T1, class T2>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2)
        {
            register auto  a1 asm("r10") = _1;
            register void* a3 asm("r8");
            register void* a4 asm("r9");

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("syscall\n"
                         : "=a"(status),
                           "=r"(a1),
                           "=d"(_2),
                           "=r"(a3),
                           "=r"(a4),
                           "=c"(unused_output),
                           "=r"(unused_output2)
                         : "a"(id), "r"(a1), "d"(_2)
                         : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3)
        {
            register auto  a1 asm("r10") = _1;
            register auto  a3 asm("r8")  = _3;
            register void* a4 asm("r9");

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("syscall\n"
                         : "=a"(status),
                           "=r"(a1),
                           "=d"(_2),
                           "=r"(a3),
                           "=r"(a4),
                           "=c"(unused_output),
                           "=r"(unused_output2)
                         : "a"(id), "r"(a1), "d"(_2), "r"(a3)
                         : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("syscall\n"
                         : "=a"(status),
                           "=r"(a1),
                           "=d"(_2),
                           "=r"(a3),
                           "=r"(a4),
                           "=c"(unused_output),
                           "=r"(unused_output2)
                         : "a"(id), "r"(a1), "d"(_2), "r"(a3), "r"(a4)
                         : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $48, %%rsp\n"
                         "movq %[a5], 40(%%rsp)\n"
                         "syscall\n"
                         "add $48, %%rsp"
                         : "=a"(status),
                           "=r"(a1),
                           "=d"(_2),
                           "=r"(a3),
                           "=r"(a4),
                           "=c"(unused_output),
                           "=r"(unused_output2)
                         : "a"(id),
                           "r"(a1),
                           "d"(_2),
                           "r"(a3),
                           "r"(a4),
                           [ a5 ] "re"(reinterpret_cast<void*>(_5))
                         : "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $64, %%rsp\n"
                         "movq %[a5], 40(%%rsp)\n"
                         "movq %[a6], 48(%%rsp)\n"
                         "syscall\n"
                         "add $64, %%rsp"
                         : "=a"(status),
                           "=r"(a1),
                           "=d"(_2),
                           "=r"(a3),
                           "=r"(a4),
                           "=c"(unused_output),
                           "=r"(unused_output2)
                         : "a"(id),
                           "r"(a1),
                           "d"(_2),
                           "r"(a3),
                           "r"(a4),
                           [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                           [ a6 ] "re"(reinterpret_cast<void*>(_6))
                         : "memory", "cc");
            return status;
        }

        // clang-format off
    
        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $64, %%rsp\n"
                        "movq %[a5], 40(%%rsp)\n"
                        "movq %[a6], 48(%%rsp)\n"
                        "movq %[a7], 56(%%rsp)\n"
                        "syscall\n"
                        "add $64, %%rsp"
                        : "=a"(status),
                        "=r"(a1),
                        "=d"(_2),
                        "=r"(a3),
                        "=r"(a4),
                        "=c"(unused_output),
                        "=r"(unused_output2)
                        : "a"(id),
                           "r"(a1),
                           "d"(_2),
                           "r"(a3),
                           "r"(a4),
                           [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                           [ a6 ] "re"(reinterpret_cast<void*>(_6)),
                           [ a7 ] "re"(reinterpret_cast<void*>(_7))
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $80, %%rsp\n"
                        "movq %[a5], 40(%%rsp)\n"
                        "movq %[a6], 48(%%rsp)\n"
                        "movq %[a7], 56(%%rsp)\n"
                        "movq %[a8], 64(%%rsp)\n"
                        "syscall\n"
                        "add $80, %%rsp"
                        : "=a"(status),
                        "=r"(a1),
                        "=d"(_2),
                        "=r"(a3),
                        "=r"(a4),
                        "=c"(unused_output),
                        "=r"(unused_output2)
                        : "a"(id),
                        "r"(a1),
                        "d"(_2),
                        "r"(a3),
                        "r"(a4),
                        [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                        [ a6 ] "re"(reinterpret_cast<void*>(_6)),
                        [ a7 ] "re"(reinterpret_cast<void*>(_7)),
                        [ a8 ] "re"(reinterpret_cast<void*>(_8))
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9 _9)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $80, %%rsp\n"
                        "movq %[a5], 40(%%rsp)\n"
                        "movq %[a6], 48(%%rsp)\n"
                        "movq %[a7], 56(%%rsp)\n"
                        "movq %[a8], 64(%%rsp)\n"
                        "movq %[a9], 72(%%rsp)\n"
                        "syscall\n"
                        "add $80, %%rsp"
                        : "=a"(status),
                        "=r"(a1),
                        "=d"(_2),
                        "=r"(a3),
                        "=r"(a4),
                        "=c"(unused_output),
                        "=r"(unused_output2)
                        : "a"(id),
                        "r"(a1),
                        "d"(_2),
                        "r"(a3),
                        "r"(a4),
                        [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                        [ a6 ] "re"(reinterpret_cast<void*>(_6)),
                        [ a7 ] "re"(reinterpret_cast<void*>(_7)),
                        [ a8 ] "re"(reinterpret_cast<void*>(_8)),
                        [ a9 ] "re"(reinterpret_cast<void*>(_9))
                        : "memory", "cc");
            return status;
        }


        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2,T3 _3,T4 _4,T5 _5,T6 _6,T7 _7,T8 _8, T9 _9, T10 _10)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $96, %%rsp\n"
                        "movq %[a5], 40(%%rsp)\n"
                        "movq %[a6], 48(%%rsp)\n"
                        "movq %[a7], 56(%%rsp)\n"
                        "movq %[a8], 64(%%rsp)\n"
                        "movq %[a9], 72(%%rsp)\n"
                        "movq %[a10], 80(%%rsp)\n"
                        "syscall\n"
                        "add $96, %%rsp"
                        : "=a"(status),
                        "=r"(a1),
                        "=d"(_2),
                        "=r"(a3),
                        "=r"(a4),
                        "=c"(unused_output),
                        "=r"(unused_output2)
                        : "a"(id),
                        "r"(a1),
                        "d"(_2),
                        "r"(a3),
                        "r"(a4),
                        [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                        [ a6 ] "re"(reinterpret_cast<void*>(_6)),
                        [ a7 ] "re"(reinterpret_cast<void*>(_7)),
                        [ a8 ] "re"(reinterpret_cast<void*>(_8)),
                        [ a9 ] "re"(reinterpret_cast<void*>(_9)),
                        [ a10 ] "re"(reinterpret_cast<void*>(_10))
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9 _9, T10 _10, T11 _11)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $96, %%rsp\n"
                        "movq %[a5], 40(%%rsp)\n"
                        "movq %[a6], 48(%%rsp)\n"
                        "movq %[a7], 56(%%rsp)\n"
                        "movq %[a8], 64(%%rsp)\n"
                        "movq %[a9], 72(%%rsp)\n"
                        "movq %[a10], 80(%%rsp)\n"
                        "movq %[a11], 88(%%rsp)\n"
                        "syscall\n"
                        "add $96, %%rsp"
                        : "=a"(status),
                        "=r"(a1),
                        "=d"(_2),
                        "=r"(a3),
                        "=r"(a4),
                        "=c"(unused_output),
                        "=r"(unused_output2)
                        : "a"(id),
                        "r"(a1),
                        "d"(_2),
                        "r"(a3),
                        "r"(a4),
                        [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                        [ a6 ] "re"(reinterpret_cast<void*>(_6)),
                        [ a7 ] "re"(reinterpret_cast<void*>(_7)),
                        [ a8 ] "re"(reinterpret_cast<void*>(_8)),
                        [ a9 ] "re"(reinterpret_cast<void*>(_9)),
                        [ a10 ] "re"(reinterpret_cast<void*>(_10)),
                        [ a11 ] "re"(reinterpret_cast<void*>(_11))
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9 _9, T10 _10, T11 _11, T12 _12)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $112, %%rsp\n"
                        "movq %[a5], 40(%%rsp)\n"
                        "movq %[a6], 48(%%rsp)\n"
                        "movq %[a7], 56(%%rsp)\n"
                        "movq %[a8], 64(%%rsp)\n"
                        "movq %[a9], 72(%%rsp)\n"
                        "movq %[a10], 80(%%rsp)\n"
                        "movq %[a11], 88(%%rsp)\n"
                        "movq %[a12], 96(%%rsp)\n"
                        "syscall\n"
                        "add $112, %%rsp"
                        : "=a"(status),
                        "=r"(a1),
                        "=d"(_2),
                        "=r"(a3),
                        "=r"(a4),
                        "=c"(unused_output),
                        "=r"(unused_output2)
                        : "a"(id),
                        "r"(a1),
                        "d"(_2),
                        "r"(a3),
                        "r"(a4),
                        [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                        [ a6 ] "re"(reinterpret_cast<void*>(_6)),
                        [ a7 ] "re"(reinterpret_cast<void*>(_7)),
                        [ a8 ] "re"(reinterpret_cast<void*>(_8)),
                        [ a9 ] "re"(reinterpret_cast<void*>(_9)),
                        [ a10 ] "re"(reinterpret_cast<void*>(_10)),
                        [ a11 ] "re"(reinterpret_cast<void*>(_11)),
                        [ a12 ] "re"(reinterpret_cast<void*>(_12))
                        : "memory", "cc");
            return status;
        }


        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9 _9, T10 _10, T11 _11, T12 _12, T13 _13)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            void*          unused_output;
            register void* unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $112, %%rsp\n"
                        "movq %[a5], 40(%%rsp)\n"
                        "movq %[a6], 48(%%rsp)\n"
                        "movq %[a7], 56(%%rsp)\n"
                        "movq %[a8], 64(%%rsp)\n"
                        "movq %[a9], 72(%%rsp)\n"
                        "movq %[a10], 80(%%rsp)\n"
                        "movq %[a11], 88(%%rsp)\n"
                        "movq %[a12], 96(%%rsp)\n"
                        "movq %[a13], 104(%%rsp)\n"
                        "syscall\n"
                        "add $112, %%rsp"
                        : "=a"(status),
                        "=r"(a1),
                        "=d"(_2),
                        "=r"(a3),
                        "=r"(a4),
                        "=c"(unused_output),
                        "=r"(unused_output2)
                        : "a"(id),
                        "r"(a1),
                        "d"(_2),
                        "r"(a3),
                        "r"(a4),
                        [ a5 ] "re"(reinterpret_cast<void*>(_5)),
                        [ a6 ] "re"(reinterpret_cast<void*>(_6)),
                        [ a7 ] "re"(reinterpret_cast<void*>(_7)),
                        [ a8 ] "re"(reinterpret_cast<void*>(_8)),
                        [ a9 ] "re"(reinterpret_cast<void*>(_9)),
                        [ a10 ] "re"(reinterpret_cast<void*>(_10)),
                        [ a11 ] "re"(reinterpret_cast<void*>(_11)),
                        [ a12 ] "re"(reinterpret_cast<void*>(_12)),
                        [ a13 ] "re"(reinterpret_cast<void*>(_13))
                        : "memory", "cc");
            return status;
        }

        // clang-format on

#pragma GCC diagnostic pop

    } // namespace detail

    template<class R, class... Args>
    inline R syscall_function<R(Args...)>::operator()(Args... args) const noexcept
    {
        return detail::syscall(_id, args...);
    }

    inline JM_INLINE_SYSCALL_ENTRY_TYPE* syscall_entries() noexcept
    {
        return &detail::syscall_holder<0>::entry + 1;
    }

} // namespace jm

#endif // JM_INLINE_SYSCALL_INL

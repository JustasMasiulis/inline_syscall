/*
 * Copyright 2018 Justas Masiulis
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

#ifndef JM_INLINE_SYSCALL_HPP
#define JM_INLINE_SYSCALL_HPP

#include <cstdint>

#if !defined(__GNUC__) && !defined(__clang__)
#error Compiler not supported. If you think this is a mistake and your compiler does support gcc style inline assembly please open an issue.
#endif

#if defined(_MSC_VER)
#define JM_INLINE_SYSCALL_FORCEINLINE __forceinline
#else
#define JM_INLINE_SYSCALL_FORCEINLINE __attribute__((always_inline))
#endif

#define JM_INLINE_SYSCALL_STUB(...) \
    JM_INLINE_SYSCALL_FORCEINLINE std::int32_t syscall(__VA_ARGS__) noexcept


#define INLINE_SYSCALL(fn)

namespace jm {

    template<class Fn>
    class syscall_function;

    /// \brief A light wrapper around the syscall to provide some type safety
    template<class R, class... Args>
    class syscall_function<R(Args...)> {
        std::uint32_t _id = 0;

    public:
        /// \brief Initializes the syscall with zero id
        constexpr syscall_function() noexcept = default;

        /// \brief initializes syscall function with given id
        constexpr syscall_function(std::uint32_t id) noexcept : _id(id) {}

        /// \brief Performs a syscall with the given arguments
        JM_INLINE_SYSCALL_FORCEINLINE R operator()(Args... args) const noexcept;
    };

    /// \brief Holds syscall information.
    struct syscall_entry {
        // the syscall id that has to be changed during initialization.
        std::uint32_t id;

        // the hash of syscall function name.
        std::uint32_t hash;

        /// \brief Returns whether the entry is valid.
        explicit operator bool() const noexcept { return hash != 0; }
    };

    /// \brief Returns syscall entry array.
    /// \note The last entry is will be zeroed. Use provided operator bool.
    JM_INLINE_SYSCALL_FORCEINLINE syscall_entry* syscall_entries() noexcept;

    namespace detail {

        // stores syscall info in a section that we create
        template<std::uint32_t Hash>
        struct syscall_holder {
            [[gnu::section("_sysc")]] inline static syscall_entry entry{ 0, Hash };
        };

        // we instantiate the first entry with 0 hash to be able to get a pointer
        template struct syscall_holder<0>;


        /* syscall stubs begin here.
         *
         * They all seem more or less the same and that's true, however
         * we need them like this for best possible code generation.
         */

        JM_INLINE_SYSCALL_STUB(std::uint32_t id)
        {
            register std::uint64_t a1 asm("r10");
            std::uint64_t          a2;
            register std::uint64_t a3 asm("r8");
            register std::uint64_t a4 asm("r9");

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $40, %%rsp\n"
                         "syscall\n"
                         "add $40, %%rsp"
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
            register auto          a1 asm("r10") = _1;
            std::uint64_t          a2;
            register std::uint64_t a3 asm("r8");
            register std::uint64_t a4 asm("r9");

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $40, %%rsp\n"
                         "syscall\n"
                         "add $40, %%rsp"
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
            register auto          a1 asm("r10") = _1;
            register std::uint64_t a3 asm("r8");
            register std::uint64_t a4 asm("r9");

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $40, %%rsp\n"
                         "syscall\n"
                         "add $40, %%rsp"
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
            register auto          a1 asm("r10") = _1;
            register auto          a3 asm("r8")  = _3;
            register std::uint64_t a4 asm("r9");

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $40, %%rsp\n"
                         "syscall\n"
                         "add $40, %%rsp"
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

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

            std::int32_t status;
            asm volatile("sub $40, %%rsp\n"
                         "syscall\n"
                         "add $40, %%rsp"
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

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                         : "a"(id), "r"(a1), "d"(_2), "r"(a3), "r"(a4), [ a5 ] "rn"(_5)
                         : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                           [ a5 ] "rn"(_5),
                           [ a6 ] "rn"(_6)
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

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                        [ a5 ] "rn"(_5),
                        [ a6 ] "rn"(_6),
                        [ a7 ] "rn"(_7)
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                        [ a5 ] "rn"(_5),
                        [ a6 ] "rn"(_6),
                        [ a7 ] "rn"(_7),
                        [ a8 ] "rn"(_8)
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9 _9)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                        [ a5 ] "rn"(_5),
                        [ a6 ] "rn"(_6),
                        [ a7 ] "rn"(_7),
                        [ a8 ] "rn"(_8),
                        [ a9 ] "rn"(_9)
                        : "memory", "cc");
            return status;
        }


        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2,T3 _3,T4 _4,T5 _5,T6 _6,T7 _7,T8 _8, T9 _9, T10 _10)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                        [ a5 ] "rn"(_5),
                        [ a6 ] "rn"(_6),
                        [ a7 ] "rn"(_7),
                        [ a8 ] "rn"(_8),
                        [ a9 ] "rn"(_9),
                        [ a10 ] "rn"(_10)
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9 _9, T10 _10, T11 _11)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                        [ a5 ] "rn"(_5),
                        [ a6 ] "rn"(_6),
                        [ a7 ] "rn"(_7),
                        [ a8 ] "rn"(_8),
                        [ a9 ] "rn"(_9),
                        [ a10 ] "rn"(_10),
                        [ a11 ] "rn"(_11)
                        : "memory", "cc");
            return status;
        }

        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9_9, T10 _10, T11 _11, T12 _12)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                        [ a5 ] "rn"(_5),
                        [ a6 ] "rn"(_6),
                        [ a7 ] "rn"(_7),
                        [ a8 ] "rn"(_8),
                        [ a9 ] "rn"(_9),
                        [ a10 ] "rn"(_10),
                        [ a11 ] "rn"(_11),
                        [ a12 ] "rn"(_12)
                        : "memory", "cc");
            return status;
        }


        template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
        JM_INLINE_SYSCALL_STUB(std::uint32_t id, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9_9, T10 _10, T11 _11, T12 _12, T13 _13)
        {
            register auto a1 asm("r10") = _1;
            register auto a3 asm("r8")  = _3;
            register auto a4 asm("r9")  = _4;

            std::uint64_t          unused_output;
            register std::uint64_t unused_output2 asm("r11");

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
                        [ a5 ] "rn"(_5),
                        [ a6 ] "rn"(_6),
                        [ a7 ] "rn"(_7),
                        [ a8 ] "rn"(_8),
                        [ a9 ] "rn"(_9),
                        [ a10 ] "rn"(_10),
                        [ a11 ] "rn"(_11),
                        [ a12 ] "rn"(_12),
                        [ a13 ] "rn"(_13)
                        : "memory", "cc");
            return status;
        }

        // clang-format on


    } // namespace detail

    template<class R, class... Args>
    JM_INLINE_SYSCALL_FORCEINLINE R
    syscall_function<R(Args...)>::operator()(Args... args) const noexcept
    {
        return detail::syscall(_id, args...);
    }

    JM_INLINE_SYSCALL_FORCEINLINE syscall_entry* syscall_entries() noexcept
    {
        return &detail::syscall_holder<0>::entry;
    }

} // namespace jm

#endif // JM_INLINE_SYSCALL_HPP

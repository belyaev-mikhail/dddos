/*
 * macros.h
 *
 *  Created on: Dec 7, 2012
 *      Author: belyaev
 */

// GUARDS ARE NOT USED FOR A REASON!
// #ifndef MACROS_H_
// #define MACROS_H_

#ifdef CALLOPHRYS_MACROS_DEFINED
#error "macros.h included twice!"
#endif

#define CALLOPHRYS_MACROS_DEFINED

/*
 * Macro for quick-writing one-liners with tricky typing.
 * This can be used to replace the following (note the same `a+b` used twice):
 *
 * template<class A, class B>
 * auto plus(A a, B b) -> decltype(a+b) { return a+b; }
 *
 * with this:
 *
 * template<class A, class B>
 * auto plus(A a, B b) QUICK_RETURN(a+b)
 *
 * Note that the one-liners can be big and the impact will be significant.
 *
 * */
#define QUICK_RETURN(...) ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

#define QUICK_CONST_RETURN(...) const ->decltype(__VA_ARGS__) { return __VA_ARGS__; }
/*
#define BYE_BYE(type, msg) return exit<type>( \
        __FILE__, \
        __LINE__, \
        __PRETTY_FUNCTION__, \
        msg)

#define BYE_BYE_VOID(msg) { \
        exit<void>( \
            __FILE__, \
            __LINE__, \
            __PRETTY_FUNCTION__, \
            msg); \
        return; \
    }

#define ASSERT(cond, msg) while(!(cond)){ exit<void>( \
        __FILE__, \
        __LINE__, \
        __PRETTY_FUNCTION__, \
        msg); }

#define ASSERTC(cond) while(!(cond)){ exit<void>( \
        __FILE__, \
        __LINE__, \
        __PRETTY_FUNCTION__, \
        #cond); }
*/
#define GUARD(...) typename std::enable_if<(__VA_ARGS__)>::type
#define GUARDED(TYPE, ...) typename std::enable_if<(__VA_ARGS__), TYPE>::type

// XXX: change this to [[noreturn]] when mother..cking g++ supports it
#define NORETURN __attribute__((noreturn))

#define DEFAULT_CONSTRUCTOR_AND_ASSIGN(CLASSNAME) \
    \
    CLASSNAME() = default; \
    CLASSNAME(const CLASSNAME&) = default; \
    CLASSNAME(CLASSNAME&&) = default; \
    CLASSNAME& operator=(const CLASSNAME&) = default; \
    CLASSNAME& operator=(CLASSNAME&&) = default;

#define PRETOKENPASTE(x, y) x ## y
#define TOKENPASTE(x, y) PRETOKENPASTE(x, y)
// #define ON_SCOPE_EXIT(LAMBDA) \
//     auto TOKENPASTE(local_scope_guard_packed_lambda, __LINE__) = [&](){ LAMBDA; }; \
//     ::util::scope_guard<decltype(TOKENPASTE(local_scope_guard_packed_lambda, __LINE__))> TOKENPASTE(local_scope_guard, __LINE__) { TOKENPASTE(local_scope_guard_packed_lambda, __LINE__) };

#ifdef __clang__
#define COMPILER clang
#elif defined(__GNUC__)
#define COMPILER gcc
#else
#error "You are trying to use an unsupported compiler. Either add it to macros.h or quit trying."
#endif

// #endif /* MACROS_H_ */

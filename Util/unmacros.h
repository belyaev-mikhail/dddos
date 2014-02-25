/*
 * unmacros.h
 *
 *  Created on: Dec 7, 2012
 *      Author: belyaev
 */

// GUARDS ARE NOT USED FOR A REASON!
// #ifndef UNMACROS_H_
// #define UNMACROS_H_

#ifdef CALLOPHRYS_MACROS_DEFINED

#    undef QUICK_RETURN
#    undef QUICK_CONST_RETURN
#    undef BYE_BYE
#    undef BYE_BYE_VOID
#    undef ASSERT
#    undef ASSERTC
#    undef GUARD
#    undef GUARDED
#    undef NORETURN
#    undef COMPILER
#    undef DEFAULT_CONSTRUCTOR_AND_ASSIGN
#    undef ON_SCOPE_EXIT
#    undef PRETOKENPASTE
#    undef TOKENPASTE

#    undef CALLOPHRYS_MACROS_DEFINED

#else

#    error "unmacros.h is included without corresponding macros.h include!"

#endif // CALLOPHRYS_MACROS_DEFINED

// #endif /* UNMACROS_H_ */

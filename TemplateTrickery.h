#pragma once
#include <type_traits>

#define MAKE_TYPE_TRAIT_HAS_STATIC_FUNCTION(TRAIT_NAME, FUNCTION_NAME)              \
namespace TRAIT_TYPE_HELPERS                                                        \
{                                                                                   \
/* Primary Template is false case, this is the default if */                        \
/* SFINAE causes the partial specialization to fail        */                       \
template <typename T, typename ReturnType, typename = void, typename ... ARGS>      \
struct TRAIT_NAME##_helper : std::false_type {};                                    \
                                                                                    \
/* Partially specialized templated is true case, this will*/                        \
/* be instantiated if SFINAE doesnt fail                  */                        \
template <typename T, typename ReturnType, typename ... ARGS>                       \
struct TRAIT_NAME##_helper <T, ReturnType, std::enable_if_t< std::is_same_v< decltype(&T::FUNCTION_NAME), ReturnType(*)(ARGS...)>>, ARGS...> : std::true_type {}; \
}                                                                                   \
                                                                                    \
/* This using directive serves to mask the third template */                        \
/* parameter that is used for the SFINAE implementation   */                        \
template <typename T, typename ReturnType, typename ... ARGS>                       \
using TRAIT_NAME = TRAIT_TYPE_HELPERS::TRAIT_NAME##_helper<T, ReturnType, void, ARGS...>; \
                                                                                    \
/* This templated bool is used as a shortcut for ::value  */                        \
/* This is inline with the STL type traits                */                        \
template <typename T, typename ReturnType, typename ... ARGS>                       \
constexpr bool TRAIT_NAME##_v = TRAIT_NAME<T, ReturnType, ARGS...>::value;

//Creates a type trait definition that has the name TRAIT_NAME and checks for a member function FUNCTION_NAME
#define MAKE_TYPE_TRAIT_HAS_MEMBER_FUNCTION(TRAIT_NAME, FUNCTION_NAME)              \
namespace TRAIT_TYPE_HELPERS                                                        \
{                                                                                   \
/* Primary Template is false case, this is the default if */                        \
/* SFINAE causes the partial specialization to fail       */                        \
template <typename T, typename ReturnType, typename = void, typename ... ARGS>      \
struct TRAIT_NAME##_helper : std::false_type {};                                    \
                                                                                    \
/* Partially specialized templated is true case, this will*/                        \
/* be instantiated if SFINAE doesnt fail                  */                        \
template <typename T, typename ReturnType, typename ... ARGS>                       \
struct TRAIT_NAME##_helper <T, ReturnType, std::enable_if_t< std::is_same_v< decltype(&T::FUNCTION_NAME), ReturnType(T::*)(ARGS...)>>, ARGS...> : std::true_type {}; \
}                                                                                   \
                                                                                    \
/* This using directive serves to mask the third template */                        \
/* parameter that is used for the SFINAE implementation   */                        \
template <typename T, typename ReturnType, typename ... ARGS>                       \
using TRAIT_NAME = TRAIT_TYPE_HELPERS::TRAIT_NAME##_helper<T, ReturnType, void, ARGS...>; \
                                                                                    \
/* This templated bool is used as a shortcut for ::value  */                        \
/* This is inline with the STL type traits                */                        \
template <typename T, typename ReturnType, typename ... ARGS>                       \
constexpr bool TRAIT_NAME##_v = TRAIT_NAME<T, ReturnType, ARGS...>::value;


//Helpers to encapulate types in values and extract them from values
template <typename T>
struct TypeT
{
   using Type = T;
};

template <typename T>
constexpr TypeT<T> type = TypeT<T>{};

template<typename T>
T valueT(TypeT<T>);


//Checks if test is of type T*
template <typename T, typename U>
bool is_type_of(U* test)
{
   T tester{};
   return *(void**)&tester == *(void**)test;
}

//Checks if the object T hac the paren operator that takes ARGS
template <typename T, typename = void, typename ... ARGS>
struct isValid_Impl : std::false_type {};

template <typename T, typename ... ARGS>
struct isValid_Impl <T, std::void_t<decltype(std::declval<T>()(std::declval<ARGS&&>()...))>, ARGS...> : std::true_type {};

//Modification of isValid from C++ Templates - The Complete guide
//http://www.tmplbook.com/code/traits/isvalid.hpp.html
template <typename T>
constexpr auto is_valid(T f) {
   return [](auto&&... args) {
      return isValid_Impl<T, void, decltype(args) && ...>::value;
   };
}
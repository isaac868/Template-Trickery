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

//Checks if type T is any of the types provided as ARGS...
//I could have done this using a much simpler function template but I wanted the interface to be similar to stl type traits
namespace TRAIT_TYPE_HELPERS
{
	template <typename T, typename = void, typename ... ARGS>
	struct is_one_of_helper : std::false_type {};

	template <typename T, typename ... ARGS>
	struct is_one_of_helper <T, std::enable_if_t<(std::is_same_v<T, ARGS> || ...)>, ARGS...> : std::true_type {};
}

//Main type trait
template <typename T, typename ... ARGS>
using is_one_of = TRAIT_TYPE_HELPERS::is_one_of_helper<T, void, ARGS...>;

//convenience template
template <typename T, typename ... ARGS>
constexpr bool is_one_of_v = is_one_of<T, ARGS...>::value;

//Creates a copy of RuleToCopy by casting to a particular derived type first, not safe due to static_pointer_cast, 
//a better version would use dynamic pointer cast and std::optional
template<typename _DerivedRuleType>
std::shared_ptr<RenameRule> CopyDerivedRenameRule(std::shared_ptr<RenameRule> RuleToCopy)
{
  auto CastedRule = std::static_pointer_cast<_DerivedRuleType>(RuleToCopy);
  CastedRule      = std::make_shared<_DerivedRuleType>(*CastedRule);
  return std::static_pointer_cast<RenameRule>(CastedRule);
}
#pragma once
#ifndef ALGORITHM_HPP
# define ALGORUTHM_HPP

#include <type_traits>

#include <utility>

namespace detail
{

template <bool B>
using bool_ = ::std::integral_constant<bool, B>;

template <class A, class ...B>
struct all_of : bool_<A::value && all_of<B...>::value> { };

template <class A>
struct all_of<A> : bool_<A::value> { };

}

template <typename T>
inline constexpr T max(T const a, T const b)
{
  return a > b ? a : b;
}

template <typename T, typename ...A>
inline constexpr typename ::std::enable_if<bool(sizeof...(A)) &&
  ::detail::all_of<::std::is_same<
    typename ::std::decay<A>::type, typename ::std::decay<T>::type>...>{},
  T>::type
max(T const a, T const b, A const ...args)
{
  return a > b ? max(a, args...) : min(b, args...);
}

template <typename T>
inline constexpr T min(T const a, T const b)
{
  return a < b ? a : b;
}

template <typename T, typename ...A>
inline constexpr typename ::std::enable_if<bool(sizeof...(A)) &&
  ::detail::all_of<::std::is_same<
    typename ::std::decay<A>::type, typename ::std::decay<T>::type>...>{},
  T>::type
min(T const a, T const b, A const ...args)
{
  return a < b ? min(a, args...) : min(b, args...);
}

template <typename T>
inline constexpr ::std::pair<T, T> minmax(T const a, T const b)
{
  return a < b ? ::std::pair<T, T>(a, b) : ::std::pair<T, T>(b, a);
}

template <typename T, typename ...A>
inline constexpr typename ::std::enable_if<bool(sizeof...(A)) &&
  ::detail::all_of<::std::is_same<
    typename ::std::decay<A>::type, typename ::std::decay<T>::type>...>{},
  ::std::pair<T, T> >::type
minmax(T const a, T const b, A const ...args)
{
  return ::std::pair<T, T>(
    a < b ? min(a, args...) : min(b, args...),
    a > b ? max(a, args...) : max(b, args...));
}

#endif // ALGORITHM_HPP

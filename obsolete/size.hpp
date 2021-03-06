#ifndef GENERIC_SIZE_HPP
# define GENERIC_SIZE_HPP
# pragma once

namespace generic
{

// size
template <typename T, ::std::size_t N>
constexpr inline decltype(N) size(T const (&)[N]) noexcept
{
  return N;
}

template <typename T, ::std::size_t M, ::std::size_t N>
constexpr inline decltype(N) size(T const (&array)[M][N]) noexcept
{
  return M * size(*array);
}

}

#endif // GENERIC_SIZE_HPP

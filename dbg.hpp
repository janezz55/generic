#ifndef GNR_DBG_HPP
# define GNR_DBG_HPP
# pragma once

#include <iostream>

#include <ostream>

namespace gnr
{

#ifdef NDEBUG
static struct
{
  template <typename U>
  auto& operator<<(U&&) const noexcept
  {
    return *this;
  }

  auto& operator<<(std::ostream&(* const)(std::ostream&)) const noexcept
  {
    return *this;
  }
} const dbg;
#else
static constexpr auto& dbg(std::cout);
#endif // NDEBUG

}

#endif // GNR_DBG_HPP

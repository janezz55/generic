#ifndef GENERIC_SWALLOW_HPP
# define GENERIC_SWALLOW_HPP
# pragma once

namespace generic
{

struct swallow
{
  template <typename ...T>
  explicit swallow(T&& ...) noexcept
  {
  }
};

}

#endif // GENERIC_SWALLOW_HPP

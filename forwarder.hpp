#ifndef GNR_FORWARDER_HPP
# define GNR_FORWARDER_HPP
# pragma once

#include <cassert>

// std::size_t
#include <cstddef>

#include <functional>

#include <type_traits>

#include <utility>

namespace gnr
{

namespace detail::forwarder
{

enum : std::size_t { default_size = 4 * sizeof(void*) };

template <typename, std::size_t, bool>
class forwarder_impl2;

template <typename R, typename ...A, std::size_t N, bool E>
class forwarder_impl2<R (A...), N, E>
{
protected:
  R (*stub_)(void*, A&&...) noexcept(E) {};

  std::aligned_storage_t<N> store_;

public:
  using result_type = R;

public:
  R operator()(A... args) const noexcept(E)
  {
    //assert(stub_);
    return stub_(const_cast<decltype(store_)*>(std::addressof(store_)),
      std::forward<A>(args)...);
  }

  template <typename F,
    typename = std::enable_if_t<std::is_invocable_r_v<R, F, A...>>
  >
  void assign(F&& f)
#if !defined(__GNUC__) || defined(__clang__)
  noexcept(noexcept(std::decay_t<F>(std::forward<F>(f))))
#endif
  {
    using functor_type = std::decay_t<F>;

    static_assert(sizeof(functor_type) <= sizeof(store_),
      "functor too large");
    static_assert(std::is_trivially_copyable<functor_type>{},
      "functor not trivially copyable");

    ::new (std::addressof(store_)) functor_type(std::forward<F>(f));

    stub_ = [](void* const ptr, A&&... args) noexcept(E) -> R
    {
      return std::invoke(*static_cast<functor_type*>(ptr),
        std::forward<A>(args)...);
    };
  }
};

template <typename, std::size_t>
class forwarder_impl;

template <typename R, typename ...A, std::size_t N>
class forwarder_impl<R (A...), N> :
  public forwarder_impl2<R (A...), N, false>
{
};

template <typename R, typename ...A, std::size_t N>
class forwarder_impl<R (A...) noexcept, N> :
  public forwarder_impl2<R (A...), N, true>
{
};

}

template <typename A, std::size_t N = detail::forwarder::default_size>
class forwarder : public detail::forwarder::forwarder_impl<A, N>
{
  using inherited_t = detail::forwarder::forwarder_impl<A, N>;

public:
  enum : std::size_t { size = N };

  forwarder() = default;

  forwarder(forwarder const&) = default;

  forwarder(forwarder&&) = default;

  template <typename F>
  static constexpr auto have_assign(int) ->
    decltype(std::declval<inherited_t>().assign(std::declval<F>()), bool())
  {
    return true;
  }

  template <typename F>
  static constexpr auto have_assign(...)
  {
    return false;
  }

  template <typename F,
    typename = std::enable_if_t<
      !std::is_same_v<std::decay_t<F>, forwarder> &&
      have_assign<F>(int())
    >
  >
  forwarder(F&& f) noexcept(noexcept(inherited_t::assign(std::forward<F>(f))))
  {
    inherited_t::assign(std::forward<F>(f));
  }

  forwarder& operator=(forwarder const&) = default;

  forwarder& operator=(forwarder&&) = default;

  template <typename F>
  forwarder& operator=(F&& f) noexcept(
    noexcept(inherited_t::assign(std::forward<F>(f))))
  {
    static_assert(std::is_invocable_v<
        decltype(&inherited_t::template assign<F>), inherited_t&, F
      >
    );

    inherited_t::assign(std::forward<F>(f));

    return *this;
  }

  explicit operator bool() const noexcept
  {
    return inherited_t::stub_;
  }

  bool operator==(std::nullptr_t) noexcept
  {
    return *this;
  }

  bool operator!=(std::nullptr_t) noexcept
  {
    return !operator==(nullptr);
  }

  void assign(std::nullptr_t) noexcept
  {
    reset();
  }

  void reset() noexcept
  {
    inherited_t::stub_ = {};
  }

  void swap(forwarder& other) noexcept
  {
    std::swap(*this, other);
  }

  void swap(forwarder&& other) noexcept
  {
    std::swap(*this, std::move(other));
  }

  template <typename T>
  auto target() noexcept
  {
    return reinterpret_cast<T*>(std::addressof(inherited_t::store_));
  }

  template <typename T> 
  auto target() const noexcept
  {
    return reinterpret_cast<T const*>(std::addressof(inherited_t::store_));
  }
};

}

#endif // GNR_FORWARDER_HPP

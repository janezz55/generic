#ifndef GNR_STACKALLOCATOR_HPP
# define GNR_STACKALLOCATOR_HPP
# pragma once

#include <cassert>

#include <cstddef>

#include <new>

#include <string>

#include <utility>

namespace gnr
{

template <std::size_t N>
class stack_store
{
public:
  stack_store() = default;

  stack_store(stack_store const&) = delete;

  stack_store& operator=(stack_store const&) = delete;

  char* allocate(std::size_t n)
  {
    n = align(n);

    if (std::greater_equal<void*>()(&buf_ + 1, ptr_ + n))
    {
      auto const r(ptr_);

      ptr_ += n;

      return r;
    }
    else
    {
      return static_cast<char*>(::operator new(n));
    }
  }

  void deallocate(char* const p, std::size_t n) noexcept
  {
    if (pointer_in_buffer(p))
    {
      if (p + align(n) == ptr_)
      {
        ptr_ = p;
      }
      // else do nothing
    }
    else
    {
      ::operator delete(p);
    }
  }

  void reset() noexcept { ptr_ = reinterpret_cast<char*>(&buf_); }

  static constexpr std::size_t size() noexcept { return N; }

  std::size_t used() const noexcept
  {
    return std::size_t(ptr_ - reinterpret_cast<char*>(&buf_));
  }

private:
  // assume that alignment is a power of 2
  static constexpr std::size_t align(std::size_t const n) noexcept
  {
    static_assert(!(alignment % 2), "alignment must be a power of 2");
    return (n + (alignment - 1)) & -alignment;
  }

  bool pointer_in_buffer(char* const p) noexcept
  {
    return std::less_equal<void*>()(&buf_, p) &&
      std::less<void*>()(p, &buf_ + 1);
  }

private:
  typename std::aligned_storage_t<N> buf_;

  char* ptr_{reinterpret_cast<char*>(&buf_)};

  static constexpr auto const alignment = alignof(buf_);
};

template <class T, std::size_t N>
class stack_allocator
{
public:
  using store_type = stack_store<N>;

  using size_type = std::size_t;

  using difference_type = std::ptrdiff_t;

  using pointer = T*;
  using const_pointer = T const*;

  using reference = T&;
  using const_reference = T const&;

  using value_type = T;

  template <class U> struct rebind { using other = stack_allocator<U, N>; };

  stack_allocator() = default;

  stack_allocator(stack_store<N>& s) noexcept : store_(&s) { }

  template <class U>
  stack_allocator(stack_allocator<U, N> const& other) noexcept :
    store_(other.store_)
  {
  }

  stack_allocator& operator=(stack_allocator const&) = delete;

  T* allocate(std::size_t const n)
  {
    return reinterpret_cast<T*>(store_->allocate(n * sizeof(T)));
  }

  void deallocate(T* const p, std::size_t const n) noexcept
  {
    store_->deallocate(reinterpret_cast<char*>(p), n * sizeof(T));
  }

  template <class U, class ...A>
  void construct(U* const p, A&& ...args)
  {
    new (p) U(std::forward<A>(args)...);
  }

  template <class U>
  void destroy(U* const p)
  {
    p->~U();
  }

  template <class U, std::size_t M>
  inline bool operator==(stack_allocator<U, M> const& rhs) const noexcept
  {
    return store_ == rhs.store_;
  }

  template <class U, std::size_t M>
  inline bool operator!=(stack_allocator<U, M> const& rhs) const noexcept
  {
    return !(*this == rhs);
  }

private:
  template <class U, std::size_t M> friend class stack_allocator;

  store_type* store_{};
};

}

namespace std
{
  // map
  template<class Key, class T, class Compare, class Allocator> class map;

  // unordered_map
  template<class Key, class T, class Hash, class Pred, class Alloc>
    class unordered_map;

  // vector
  template <class T, class Alloc> class vector;
}

template <class Key, class T, std::size_t N = 256,
  class Compare = std::less<Key>
>
using stack_map = std::map<Key, T, Compare,
  gnr::stack_allocator<std::pair<Key const, T>, N> >;

template <std::size_t N = 128>
using stack_string = std::basic_string<char, std::char_traits<char>,
  gnr::stack_allocator<char, N> >;

template <class Key, class T, std::size_t N = 256,
  class Hash = std::hash<Key>, class Pred = std::equal_to<Key> >
using stack_unordered_map = std::unordered_map<Key, T, Hash, Pred,
  gnr::stack_allocator<std::pair<Key const, T>, N> >;

template <typename T, std::size_t N = 256>
using stack_vector = std::vector<T, gnr::stack_allocator<T, N> >;

#endif // GNR_STACKALLOCATOR_HPP

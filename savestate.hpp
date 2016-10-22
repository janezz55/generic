#ifndef SAVE_STATE_H
# define SAVE_STATE_H
# pragma once

struct statebuf
{
  void* sp;
  void* label;
};

#if defined(__GNUC__)
inline __attribute__((always_inline)) bool savestate(statebuf& ssb) noexcept
{
  bool r;

#if defined(i386) || defined(__i386) || defined(__i386__)
  asm volatile (
    "movl %%esp, %0\n\t" // store sp
    "movl $1f, %1\n\t" // store label
    "movb $0, %2\n\t" // return false
    "jmp 2f\n\t"
    "1:movb $1, %2\n\t" // return true
    "2:"
    : "=m" (ssb.sp), "=m" (ssb.label), "=r" (r)
    :
    : "memory"
  );
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
  asm volatile (
    "movq %%rsp, %0\n\t" // store sp
    "movq $1f, %1\n\t" // store label
    "movb $0, %2\n\t" // return false
    "jmp 2f\n\r"
    "1:movb $1, %2\n\t" // return true
    "2:"
    : "=m" (ssb.sp), "=m" (ssb.label), "=r" (r)
    :
    : "memory"
  );
#endif

  return r;
}
#elif defined(_MSC_VER)
  __forceinline bool savestate(statebuf& ssb) noexcept
  {
  register bool r;

  __asm {
    lea ecx, ssb
    mov [ecx]statebuf.sp, esp
    mov [ecx]statebuf.label, offset _1f
    mov r, 0x0
    jmp _2f
    _1f : mov r, 0x1
    _2f :
  }

    return r;
  }
#else
# error ""
#endif

#if defined(__GNUC__)
#if defined(i386) || defined(__i386) || defined(__i386__)
#define restorestate(SSB)          \
  asm volatile (                   \
    "movl %0, %%esp\n\t"           \
    "jmp *%1"                      \
    :                              \
    : "m" (ssb.sp), "m" (ssb.label)\
  );
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#define restorestate(SSB)          \
  asm volatile (                   \
    "movq %0, %%rsp\n\t"           \
    "jmp *%1"                      \
    :                              \
    : "m" (SSB.sp), "m" (SSB.label)\
  );
#else
# error ""
#endif
#elif defined(_MSC_VER)
#define restorestate(SSB)          \
  __asm lea eax, this.SSB        \
  __asm mov esp, [eax]statebuf.sp\
  __asm jmp [eax]statebuf.label
#else
# error "unsupported compiler"
#endif

#endif // SAVE_STATE_H

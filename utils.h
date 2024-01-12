#ifndef H_UTILS_H

#include <stdint.h>
#include <stddef.h>

typedef int8_t     i8;
typedef int16_t    i16;
typedef int32_t    i32;
typedef int64_t    i64;

typedef uint8_t    u8;
typedef uint16_t   u16;
typedef uint32_t   u32;
typedef uint64_t   u64;

typedef int32_t    b32; // NOTE: For bool.

typedef size_t    memory_size;
typedef uintptr_t uptr;

typedef float      f32;
typedef double     f64;

#undef TRUE
#undef FALSE
#define TRUE       (1)
#define FALSE      (0)

#define ASSERT(x) do { if (!(x)) { *(volatile int*)0; } } while (0)
#define ARRAY_COUNT(x)      (sizeof(x) / sizeof(*(x)))

#define OFFSETOF(type, member)  ((memory_size)&(((type*)0)->member))
#define COUNTOF(type, member)  (ARRAY_COUNT(((type*)0)->member))

#define KILOBYTES(x) ((x) * (1024ULL))
#define MEGABYTES(x) (KILOBYTES(x) * (1024ULL))
#define GIGABYTES(x) (MEGABYTES(x) * (1024ULL))

static inline u32 string_length(const u8* string)
{
    u32 length = 0;

    while (string && *string)
    {
        ++length;
        ++string;
    }

    return length;
}

#define H_UTILS_H
#endif

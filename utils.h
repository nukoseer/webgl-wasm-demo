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

#define SWAP(a, b, t)                           \
    {                                           \
        t temp = a;                             \
        a = b;                                  \
        b = temp;                               \
    }

#define DOUBLY_LINKED_LIST_INIT(sentinel)       \
    (sentinel)->prev = (sentinel);              \
    (sentinel)->next = (sentinel);

#define DOUBLY_LINKED_LIST_INSERT(sentinel, element)    \
    (element)->next = (sentinel)->next;                 \
    (element)->prev = (sentinel);                       \
    (element)->next->prev = (element);                 \
    (element)->prev->next = (element);

static inline f32 lerp(f32 min, f32 value, f32 max)
{
    f32 result = min + value * (max - min);

    return result;
}

static inline i32 modulo(i32 a, i32 b)
{
    return (a % b + b) % b;
}

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

static void u32_to_string(u8* string, u32 value)
{
    u32 number = value;
    i32 digit_count = 0;

    while (number)
    {
        number /= 10;
        ++digit_count;
    }

    number = value;

    if (!number)
    {
        ++digit_count;
    }

    string[digit_count] = 0;

    while (digit_count > 0)
    {
        string[digit_count - 1] = (u8)(number % 10) + '0';
        number /= 10;
        --digit_count;
    }
}

static u32 lfsr_rand(void)
{
    static u16 start_value = 0xACE1U;
    u16 lfsr = start_value;
    u32 bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;

    lfsr = (lfsr >> 1) | (bit << 15);

    ++start_value;
    
    return lfsr;
}

#define H_UTILS_H
#endif

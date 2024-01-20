#ifndef H_MEMORY_UTILS_H

#define DEFAULT_ALIGNMENT 4

#define PUSH_SIZE(memory_arena, size) (push_size(memory_arena, size, DEFAULT_ALIGNMENT))
#define PUSH_ARRAY(memory_arena, count, type) (push_size(memory_arena, ((count) * sizeof(type)), DEFAULT_ALIGNMENT))
#define PUSH_ARRAY_N(memory_arena, count, type, alignment) (push_size(memory_arena, ((count) * sizeof(type)), alignment))
#define PUSH_STRUCT(memory_arena, type) ((type*)(push_size(memory_arena, sizeof(type), DEFAULT_ALIGNMENT)))
#define PUSH_STRUCT_N(memory_arena, type, alignment) ((type*)(push_size(memory_arena, sizeof(type), alignment)))
#define PUSH_ARRAY_ZERO(memory_arena, count, type) (push_size_zero(memory_arena, ((count) * sizeof(type)), DEFAULT_ALIGNMENT))
#define PUSH_ARRAY_ZERO_N(memory_arena, count, type, alignment) (push_size_zero(memory_arena, ((count) * sizeof(type)), alignment))
#define PUSH_STRUCT_ZERO(memory_arena, type) ((type*)(push_size_zero(memory_arena, sizeof(type), DEFAULT_ALIGNMENT)))
#define PUSH_STRUCT_ZERO_N(memory_arena, type, alignment) ((type*)(push_size_zero(memory_arena, sizeof(type)), alignment))
#define STRUCT_ZERO(memory, type) (memory_zero(memory, sizeof(type)))

typedef struct MemoryArena
{
    void* memory;
    memory_size size;
    memory_size used;
} MemoryArena;

typedef struct TemporaryMemory
{
    MemoryArena* arena;
    memory_size initial_size;
} TemporaryMemory;

static inline MemoryArena* get_memory_arena(u8* base_address, memory_size size)
{
    MemoryArena* memory_arena = (MemoryArena*)base_address;

    memory_arena->memory = (u8*)memory_arena + sizeof(MemoryArena);
    memory_arena->used = 0;
    memory_arena->size = size - sizeof(MemoryArena);

    return memory_arena;
}

static inline void reset_memory_arena(MemoryArena* memory_arena)
{
    memory_arena->used = 0;
}

static inline TemporaryMemory begin_temporary_memory(MemoryArena* memory_arena)
{
    TemporaryMemory temporary_memory = { 0 };

    temporary_memory.arena = memory_arena;
    temporary_memory.initial_size = memory_arena->used;

    return temporary_memory;
}

static inline void end_temporary_memory(TemporaryMemory temporary_memory)
{
    temporary_memory.arena->used = temporary_memory.initial_size;
}

static inline memory_size get_alignment_offset(MemoryArena* arena, memory_size alignment)
{
	memory_size alignment_offset = 0;
	
	memory_size result_pointer = (memory_size)arena->memory + arena->used;
	memory_size alignment_mask = alignment - 1;
	
	if (result_pointer & alignment_mask)
    {
		alignment_offset = alignment - (result_pointer & alignment_mask);
	}

	return alignment_offset;
}

// TODO: Probably, we should also handle alignment in these functions.
static inline void* push_size(MemoryArena* memory_arena, memory_size size, memory_size alignment)
{
    void* result = 0;
    memory_size alignment_offset = get_alignment_offset(memory_arena, alignment);
    size += alignment_offset;
    
    ASSERT(memory_arena->used + size <= memory_arena->size);
    result = ((u8*)memory_arena->memory + memory_arena->used + alignment_offset);
    memory_arena->used += size;

    return result;
}

static inline void memory_set(void* memory, u8 character, memory_size size)
{
    u8* pointer = (u8*)memory;
    
    while (size--)
    {
        *pointer++ = character;
    }
}

static inline void memory_zero(void* memory, memory_size size)
{
    memory_set(memory, 0, size);
}

static inline memory_size memory_copy(void* destination, void* source, memory_size size)
{
    u8* dst = destination;
    u8* src = source;
    memory_size copy_size = size;
    
    while (copy_size)
    {
        *dst++ = *src++;
        --copy_size;
    }

    return size - copy_size;
}

static inline void* push_size_zero(MemoryArena* memory_arena, memory_size size, memory_size alignment)
{
    void* result = push_size(memory_arena, size, alignment);
    
    memory_zero(result, size);

    return result;
}

static inline MemoryArena* get_sub_arena(MemoryArena* memory_arena, memory_size size, memory_size alignment)
{
    MemoryArena* sub_arena = push_size(memory_arena, size, alignment);

    sub_arena->memory = sub_arena + sizeof(MemoryArena);
    sub_arena->used = 0;
    sub_arena->size = size - sizeof(MemoryArena);

    return sub_arena;
}

#define H_MEMORY_UTILS_H
#endif

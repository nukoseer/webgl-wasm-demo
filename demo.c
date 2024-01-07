#include "utils.h"
#include "memory_utils.h"
#include "demo.h"

extern u8 __heap_base;
extern u8 __data_end;

typedef struct State
{
    MemoryArena* arena;
    GraphicsHandle graphics;
    u32 width;
    u32 height;
    ShaderHandle vertex_shader;
    ShaderHandle pixel_shader;
    ProgramHandle program;
} State;

static State* state;

static GraphicsHandle create_graphics(void)
{
    return platform_create_graphics();
}

static ShaderHandle create_shader(GraphicsHandle graphics, ShaderType type, const u8* source)
{
    return platform_create_shader(graphics, type, source);
}

static ProgramHandle create_program(GraphicsHandle graphics, ShaderHandle vertex_shader, ShaderHandle pixel_shader)
{
    return platform_create_program(graphics, vertex_shader, pixel_shader);
}

static void log_integer(u32 integer)
{
    platform_log_integer(integer);
}

void init(u32 width, u32 height)
{
    MemoryArena* arena = get_memory_arena(&__heap_base, &__heap_base - &__data_end);

    state = PUSH_STRUCT(arena, State);
    state->arena = arena;
    state->width = width;
    state->height = height;

    const u8* vertex_shader_source = (const u8*)
    "#version 300 es                              \n"
    "                                             \n"
    "layout (location = 0) in vec2 a_position;    \n"
    "layout (location = 1) in vec3 a_color;       \n"
    "                                             \n"
    "out vec4 color;                              \n"
    "                                             \n"
    "void main()                                  \n"
    "{                                            \n"
      "gl_Position = vec4(a_position, 0.0, 1.0);  \n"
      "color = vec4(a_color, 1.0);                \n"
    "}                                            \n";

    const u8* pixel_shader_source = (const u8*)
    "#version 300 es                              \n"
    "                                             \n"
    "precision highp float;                       \n"
    "                                             \n"
    "in vec4 color;                               \n"
    "                                             \n"
    "out vec4 out_color;                          \n"
    "                                             \n"
    "void main()                                  \n"
    "{                                            \n"
      "out_color = color;                         \n"
    "}                                            \n";

    state->graphics = create_graphics();
    
    state->vertex_shader = create_shader(state->graphics, VERTEX_SHADER_TYPE, vertex_shader_source);
    state->pixel_shader = create_shader(state->graphics, PIXEL_SHADER_TYPE, pixel_shader_source);

    state->program = create_program(state->graphics, state->vertex_shader, state->pixel_shader);
    
    log_integer(width);
    log_integer(height);
    log_integer((u32)state->graphics);
    log_integer((u32)state->vertex_shader);
    log_integer((u32)state->pixel_shader);
    log_integer((u32)state->program);
}

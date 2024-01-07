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
    // TODO: No need to store these in this struct?
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

static BufferHandle create_buffer(GraphicsHandle graphics)
{
    return platform_create_buffer(graphics);
}

static void bind_buffer(GraphicsHandle graphics, BufferHandle buffer, BufferType type)
{
    platform_bind_buffer(graphics, buffer, type);
}

static void set_buffer_data(GraphicsHandle graphics, BufferHandle buffer, void* buffer_data, u32 buffer_size, BufferType type)
{
    platform_set_buffer_data(graphics, buffer, buffer_data, buffer_size, type);
}

static InputLayoutHandle create_input_layout(GraphicsHandle graphics, ProgramHandle program,
                                const char** names, const u32* offsets, const u32* formats, u32 stride, u32 count)
{
    return platform_create_input_layout(graphics, program, names, offsets, formats, stride, count);
}

static void use_input_layout(GraphicsHandle graphics, InputLayoutHandle input_layout)
{
    return platform_use_input_layout(graphics, input_layout);
}

static void set_viewport(GraphicsHandle graphics, u32 width, u32 height)
{
    platform_set_viewport(graphics, width, height);
}

static void clear_color(GraphicsHandle graphics, f32 r, f32 g, f32 b, f32 a)
{
    platform_clear_color(graphics, r, g, b, a);
}

static void use_program(GraphicsHandle graphics, ProgramHandle program)
{
    platform_use_program(graphics, program);
}

static void draw_arrays(GraphicsHandle graphics, PrimitiveType primitive_type, u32 offset, u32 count)
{
    platform_draw_arrays(graphics, primitive_type, offset, count);
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
    "  gl_Position = vec4(a_position, 0.0, 1.0);  \n"
    "  color = vec4(a_color, 1.0);                \n"
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
    // "  vec4 new_color = color;                    \n"
    // "  new_color.w = 0.5;                         \n"
    "  out_color = color;                         \n"
    "}                                            \n";

    state->graphics = create_graphics();
    state->vertex_shader = create_shader(state->graphics, VERTEX_SHADER_TYPE, vertex_shader_source);
    state->pixel_shader = create_shader(state->graphics, PIXEL_SHADER_TYPE, pixel_shader_source);
    state->program = create_program(state->graphics, state->vertex_shader, state->pixel_shader);

    BufferHandle vertex_buffer = create_buffer(state->graphics);
    // bind_buffer(state->graphics, vertex_buffer, ARRAY_BUFFER_TYPE);

    f32 vertex_data[] =
    {
        -0.00f, +0.75f, 1.0f, 0.0f, 0.0f,
        +0.75f, -0.50f, 0.0f, 1.0f, 0.0f,
        -0.75f, -0.50f, 0.0f, 0.0f, 1.0f,
    };
    
    set_buffer_data(state->graphics, vertex_buffer, vertex_data, sizeof(vertex_data), ARRAY_BUFFER_TYPE);

    const char* input_layout_names[] = { "a_position", "a_color" };
    u32 input_layout_offsets[] = { 0, 2 * 4 };
    u32 input_layout_formats[] = { 2, 3 };

    InputLayoutHandle input_layout = create_input_layout(state->graphics, state->program,
                                                         input_layout_names, input_layout_offsets, input_layout_formats, 20, 2);
    use_input_layout(state->graphics, input_layout);

    set_viewport(state->graphics, state->width, state->height);
    clear_color(state->graphics, 0.392f, 0.584f, 0.929f, 1.0f);

    use_program(state->graphics, state->program);
    draw_arrays(state->graphics, TRIANGLE_PRIMITIVE_TYPE, 0, 3);

    log_integer(width);
    log_integer(height);
    log_integer(state->graphics);
    log_integer(state->vertex_shader);
    log_integer(state->pixel_shader);
    log_integer(state->program);
    log_integer(vertex_buffer);
    log_integer(input_layout);
}

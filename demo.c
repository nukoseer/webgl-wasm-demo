#include "utils.h"
#include "memory_utils.h"
#include "demo.h"

extern u8 __heap_base;
extern u8 __data_end;

typedef struct InputLayout
{
    InputLayoutHandle handle;
    u32 stride;
} InputLayout;

typedef struct GraphicsState
{
    GraphicsHandle handle;

    ShaderHandle vertex_shader_handle;
    ShaderHandle pixel_shader_handle;
    ProgramHandle program_handle;
    BufferHandle vertex_buffer_handle;
    InputLayout input_layout;
} GraphicsState;

typedef struct State
{
    MemoryArena* arena;
    u32 width;
    u32 height;
    GraphicsState graphics;
} State;

typedef struct Vertex
{
    f32 position[2];
    f32 uv[2];
    f32 color[3];
} Vertex;

static Vertex vertex_data[] =
{
    { { 600.0f, 300.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    { { 800.0f, 600.0f }, { 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
    { { 1000.0f, 300.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
};

static State* state;

void render(void)
{
    GraphicsState* graphics = &state->graphics;

    platform_clear_color(graphics->handle, 0.0f, 0.0f, 0.0f, 1.0f);
    platform_bind_buffer(graphics->handle, graphics->vertex_buffer_handle, ARRAY_BUFFER_TYPE);
    platform_set_buffer_data(graphics->handle, vertex_data, sizeof(vertex_data), ARRAY_BUFFER_TYPE);
    platform_use_input_layout(graphics->handle, graphics->input_layout.handle);
    platform_use_program(graphics->handle, graphics->program_handle);
    platform_draw_arrays(graphics->handle, TRIANGLE_PRIMITIVE_TYPE, 0, 3);
}

void graphics_init(void)
{
    const u8* vertex_shader_source = (const u8*)
    "#version 300 es                                  \n"
    "                                                 \n"
    "layout (location = 0) in vec2 a_position;        \n"
    "layout (location = 1) in vec2 a_uv;              \n"
    "layout (location = 2) in vec3 a_color;           \n"
    "                                                 \n"
    "uniform vec2 u_resolution;                       \n"
    "                                                 \n"
    "out vec4 color;                                  \n"
    "out vec2 uv;                                     \n"
    "                                                 \n"
    "void main()                                      \n"
    "{                                                \n"
    "  vec2 zero_to_one = a_position / u_resolution;  \n"
    "  vec2 clip_space = (zero_to_one * 2.0) - 1.0;   \n"
    "                                                 \n"
    "  gl_Position = vec4(clip_space, 0.0, 1.0);      \n"
    "  color = vec4(a_color, 1.0);                    \n"
    "  uv = a_uv;                                     \n"
    "}                                                \n";

    const u8* pixel_shader_source = (const u8*)
    "#version 300 es                                  \n"
    "                                                 \n"
    "precision highp float;                           \n"
    "                                                 \n"
    "in vec4 color;                                   \n"
    "in vec2 uv;                                      \n"
    "                                                 \n"
    "out vec4 out_color;                              \n"
    "                                                 \n"
    "void main()                                      \n"
    "{                                                \n"
    "  vec2 n_uv = uv * 2.0 - 1.0;                    \n"
    "  out_color = color;                             \n"
    "}                                                \n";

    state->graphics.handle = platform_create_graphics();
    state->graphics.vertex_shader_handle = platform_create_shader(state->graphics.handle, VERTEX_SHADER_TYPE, vertex_shader_source);
    state->graphics.pixel_shader_handle = platform_create_shader(state->graphics.handle, PIXEL_SHADER_TYPE, pixel_shader_source);
    state->graphics.program_handle = platform_create_program(state->graphics.handle,
                                                    state->graphics.vertex_shader_handle, state->graphics.pixel_shader_handle);
    state->graphics.vertex_buffer_handle = platform_create_buffer(state->graphics.handle);

    const char* input_layout_names[] = { "a_position", "a_uv", "a_color" };
    u32 input_layout_offsets[] = { OFFSETOF(Vertex, position), OFFSETOF(Vertex, uv), OFFSETOF(Vertex, color) };
    u32 input_layout_formats[] = { COUNTOF(Vertex, position), COUNTOF(Vertex, uv), COUNTOF(Vertex, color) };

    state->graphics.input_layout.handle = platform_create_input_layout(state->graphics.handle, state->graphics.program_handle,
                                                                       input_layout_names, input_layout_offsets, input_layout_formats,
                                                                       sizeof(Vertex), ARRAY_COUNT(input_layout_names));
    state->graphics.input_layout.stride = sizeof(Vertex);

    platform_use_program(state->graphics.handle, state->graphics.program_handle);
    UniformHandle uniform_handle = platform_get_uniform_location(state->graphics.handle, state->graphics.program_handle, (u8*)"u_resolution");
    platform_set_uniform_2f(state->graphics.handle, uniform_handle, state->width, state->height);
    
    platform_set_viewport(state->graphics.handle, state->width, state->height);

    platform_log_number(state->graphics.handle);
    platform_log_number(state->graphics.vertex_shader_handle);
    platform_log_number(state->graphics.pixel_shader_handle);
    platform_log_number(state->graphics.program_handle);
    platform_log_number(state->graphics.vertex_buffer_handle);
    platform_log_number(state->graphics.input_layout.handle);
    platform_log_number(uniform_handle);
}

void init(u32 width, u32 height)
{
    MemoryArena* arena = get_memory_arena(&__heap_base, &__heap_base - &__data_end);

    state = PUSH_STRUCT(arena, State);
    state->arena = arena;
    state->width = width;
    state->height = height;

    graphics_init();
}

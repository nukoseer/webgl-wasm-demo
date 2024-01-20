#include "utils.h"
#include "memory_utils.h"
#include "demo.h"

extern u8 __heap_base;
extern u8 __data_end;

typedef struct DrawBuffer
{
    u8* base;
    memory_size size;
    memory_size max_size;
} DrawBuffer;

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
    DrawBuffer* draw_buffer;
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

static void delete_buffer(GraphicsHandle graphics, BufferHandle buffer)
{
    platform_delete_buffer(graphics, buffer);
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

static UniformHandle get_uniform_location(GraphicsHandle graphics, ProgramHandle program, const u8* name)
{
    return platform_get_uniform_location(graphics, program, name);
}

static void set_uniform_2f(GraphicsHandle graphics, UniformHandle uniform, f32 f1, f32 f2)
{
    platform_set_uniform_2f(graphics, uniform, f1, f2);
}

static void draw(GraphicsHandle graphics, PrimitiveType primitive_type, u32 offset, u32 count)
{
    platform_draw_arrays(graphics, primitive_type, offset, count);
}

static void log_number(f32 number)
{
    platform_log_number(number);
}

static DrawBuffer* allocate_draw_buffer(MemoryArena* arena, memory_size size)
{
    DrawBuffer* draw_buffer = PUSH_STRUCT(arena, DrawBuffer);

    draw_buffer->max_size = size;
    draw_buffer->size = 0;
    draw_buffer->base = PUSH_SIZE(arena, draw_buffer->max_size);
    // draw_buffer->buffer = create_buffer(graphics);

    return draw_buffer;
}

static void reset_draw_buffer(DrawBuffer* draw_buffer)
{
    draw_buffer->size = 0;
    // delete_buffer(draw_buffer->graphics, draw_buffer->buffer);
}

static void push_draw_data(DrawBuffer* draw_buffer, void* data, memory_size size)
{
    ASSERT(draw_buffer->size + size < draw_buffer->max_size);

    memory_copy(draw_buffer->base + draw_buffer->size, data, size);

    draw_buffer->size += size;
}

static void draw_rectangle(DrawBuffer* draw_buffer,
                           f32 x, f32 y, f32 width, f32 height,
                           f32 r, f32 g, f32 b)
{
    f32 x1 = x;
    f32 x2 = x + width;
    f32 y1 = y;
    f32 y2 = y + height;

    f32 rectangle_data[] =
    {
        x1, y1, 0.0f, 0.0f, r, g, b,
        x2, y1, 1.0f, 0.0f, r, g, b,
        x1, y2, 0.0f, 1.0f, r, g, b,
        x1, y2, 0.0f, 1.0f, r, g, b,
        x2, y1, 1.0f, 0.0f, r, g, b,
        x2, y2, 1.0f, 1.0f, r, g, b,
    };
    push_draw_data(draw_buffer, &rectangle_data, sizeof(rectangle_data));
}

typedef struct Rectangle
{
    f32 x, y;
    f32 width, height;
} Rectangle;

static Rectangle rectangle[2];

void update(f32 delta_time)
{
    // DrawBuffer* draw_buffer = state->draw_buffer;

    for (u32 i = 0; i < ARRAY_COUNT(rectangle); ++i)
    {
        Rectangle* rect = rectangle + i;
        
        if (rect->x >= (f32)state->width)
        {
            rect->x = 0.0f;
        }
        rect->x += 0.80f;   
    }
}

void render(void)
{
    GraphicsState* graphics = &state->graphics;
    DrawBuffer* draw_buffer = state->draw_buffer;

    clear_color(graphics->handle, 0.0f, 0.0f, 0.0f, 1.0f);
    draw_rectangle(draw_buffer, rectangle[0].x, rectangle[0].y, rectangle[0].width, rectangle[0].height, 1.0f, 0.0f, 0.0f);
    draw_rectangle(draw_buffer, rectangle[1].x, rectangle[1].y, rectangle[1].width, rectangle[1].height, 1.0f, 0.0f, 0.0f);
    // draw_rectangle(draw_buffer, -0.25f, -0.25f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
    // draw_rectangle(draw_buffer, 0.0f, 0.0f, 0.01f, 0.01f, 1.0f, 1.0f, 1.0f);

    bind_buffer(graphics->handle, graphics->vertex_buffer_handle, ARRAY_BUFFER_TYPE);
    set_buffer_data(graphics->handle, graphics->vertex_buffer_handle, draw_buffer->base, draw_buffer->size, ARRAY_BUFFER_TYPE);
    use_input_layout(graphics->handle, graphics->input_layout.handle);
    use_program(graphics->handle, graphics->program_handle);

    ASSERT(draw_buffer->size % graphics->input_layout.stride == 0);
    draw(graphics->handle, TRIANGLE_PRIMITIVE_TYPE, 0, draw_buffer->size / graphics->input_layout.stride);
    reset_draw_buffer(draw_buffer);
}

void graphics_init(void)
{
    state->draw_buffer = allocate_draw_buffer(state->arena, KILOBYTES(2));

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
    "#version 300 es                              \n"
    "                                             \n"
    "precision highp float;                       \n"
    "                                             \n"
    "in vec4 color;                               \n"
    "in vec2 uv;                                  \n"
    "                                             \n"
    "out vec4 out_color;                          \n"
    "                                             \n"
    "void main()                                  \n"
    "{                                            \n"
    "  vec2 n_uv = uv * 2.0 - 1.0;                               \n"
    "  vec2 border_size = vec2(1.5);                                               \n"
    "  vec2 rectangle_size = vec2(1.0) - border_size;                                               \n"
    "                                                 \n"
    "  float distance_field = length(max(abs(n_uv) - rectangle_size, 0.0) / border_size);                                               \n"
    "  float alpha = 1.0 - distance_field;                                               \n"
    "                                                  \n"
    "  out_color = vec4(vec3(color), distance_field);           \n"
    // "  out_color = color;                         \n"
    "}                                            \n";


    state->graphics.handle = create_graphics();
    state->graphics.vertex_shader_handle = create_shader(state->graphics.handle, VERTEX_SHADER_TYPE, vertex_shader_source);
    state->graphics.pixel_shader_handle = create_shader(state->graphics.handle, PIXEL_SHADER_TYPE, pixel_shader_source);
    state->graphics.program_handle = create_program(state->graphics.handle,
                                                    state->graphics.vertex_shader_handle, state->graphics.pixel_shader_handle);
    state->graphics.vertex_buffer_handle = create_buffer(state->graphics.handle);

    typedef struct Vertex
    {
        f32 position[2];
        f32 uv[2];
        f32 color[3];
    } Vertex;
    
    // f32 vertex_data[] =
    // {
    //     -0.00f, +0.75f, 1.0f, 0.0f, 0.0f,
    //     +0.75f, -0.50f, 0.0f, 1.0f, 0.0f,
    //     -0.75f, -0.50f, 0.0f, 0.0f, 1.0f,
    // };

    // Vertex vertex_data[] =
    // {
    //     { { -0.00f, +0.75f }, { 1.0f, 0.0f, 0.0f } },
    //     { { +0.75f, -0.50f }, { 0.0f, 1.0f, 0.0f } },
    //     { { -0.75f, -0.50f }, { 0.0f, 0.0f, 1.0f } },
    // };
    
    const char* input_layout_names[] = { "a_position", "a_uv", "a_color" };
    u32 input_layout_offsets[] = { OFFSETOF(Vertex, position), OFFSETOF(Vertex, uv), OFFSETOF(Vertex, color) };
    u32 input_layout_formats[] = { COUNTOF(Vertex, position), COUNTOF(Vertex, uv), COUNTOF(Vertex, color) };

    state->graphics.input_layout.handle = create_input_layout(state->graphics.handle, state->graphics.program_handle,
                                                              input_layout_names, input_layout_offsets, input_layout_formats,
                                                              sizeof(Vertex), ARRAY_COUNT(input_layout_names));
    state->graphics.input_layout.stride = sizeof(Vertex);

    use_program(state->graphics.handle, state->graphics.program_handle);
    UniformHandle uniform_handle = get_uniform_location(state->graphics.handle, state->graphics.program_handle, (u8*)"u_resolution");
    set_uniform_2f(state->graphics.handle, uniform_handle, state->width, state->height);
    
    set_viewport(state->graphics.handle, state->width, state->height);

    log_number(state->graphics.handle);
    log_number(state->graphics.vertex_shader_handle);
    log_number(state->graphics.pixel_shader_handle);
    log_number(state->graphics.program_handle);
    log_number(state->graphics.vertex_buffer_handle);
    log_number(state->graphics.input_layout.handle);
    log_number(uniform_handle);
}

void init(u32 width, u32 height)
{
    MemoryArena* arena = get_memory_arena(&__heap_base, &__heap_base - &__data_end);

    state = PUSH_STRUCT(arena, State);
    state->arena = arena;
    state->width = width;
    state->height = height;

    graphics_init();

    rectangle[0].x = 0.0f;
    rectangle[0].y = 10.0f;
    rectangle[0].width = 40.0f;
    rectangle[0].height = 40.0f;
    rectangle[1].x = 110.0f;
    rectangle[1].y = 10.0f;
    rectangle[1].width = 40.0f;
    rectangle[1].height = 40.0f;
}

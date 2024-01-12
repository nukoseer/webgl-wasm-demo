#include "utils.h"
#include "memory_utils.h"
#include "demo.h"

extern u8 __heap_base;
extern u8 __data_end;

typedef struct DrawBuffer
{
    GraphicsHandle graphics;
    BufferHandle buffer;
    u8* base;
    memory_size size;
    memory_size max_size;
} DrawBuffer;

typedef struct State
{
    MemoryArena* arena;
    GraphicsHandle graphics;
    u32 width;
    u32 height;

    DrawBuffer* draw_buffer;

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

static void draw_arrays(GraphicsHandle graphics, PrimitiveType primitive_type, u32 offset, u32 count)
{
    platform_draw_arrays(graphics, primitive_type, offset, count);
}

static void log_integer(u32 integer)
{
    platform_log_integer(integer);
}

static DrawBuffer* allocate_draw_buffer(GraphicsHandle graphics, MemoryArena* arena, memory_size size)
{
    DrawBuffer* draw_buffer = PUSH_STRUCT(arena, DrawBuffer);

    draw_buffer->max_size = size;
    draw_buffer->size = 0;
    draw_buffer->base = push_size(arena, draw_buffer->max_size);
    // draw_buffer->buffer = create_buffer(graphics);
    draw_buffer->graphics = graphics;

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
        x1, y1, r, g, b,
        x2, y1, r, g, b,
        x1, y2, r, g, b,
        x1, y2, r, g, b,
        x2, y1, r, g, b,
        x2, y2, r, g, b,
    };

    push_draw_data(draw_buffer, &rectangle_data, sizeof(rectangle_data));
}

void update(void)
{

    
}

void render(void)
{
    DrawBuffer* draw_buffer = state->draw_buffer;

    clear_color(draw_buffer->graphics, 0.7f, 0.584f, 0.7, 1.0f);
    draw_rectangle(draw_buffer, -0.25f, -0.25f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f);
    draw_rectangle(draw_buffer, 0.0f, 0.0f, 0.01f, 0.01f, 1.0f, 1.0f, 1.0f);

    set_buffer_data(draw_buffer->graphics, draw_buffer->buffer, draw_buffer->base, draw_buffer->size, ARRAY_BUFFER_TYPE);
    use_program(state->graphics, state->program);
    draw_arrays(draw_buffer->graphics, TRIANGLE_PRIMITIVE_TYPE, 0, 12);
    reset_draw_buffer(draw_buffer);
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

    state->draw_buffer = allocate_draw_buffer(state->graphics, arena, KILOBYTES(2));
    
    state->vertex_shader = create_shader(state->graphics, VERTEX_SHADER_TYPE, vertex_shader_source);
    state->pixel_shader = create_shader(state->graphics, PIXEL_SHADER_TYPE, pixel_shader_source);
    state->program = create_program(state->graphics, state->vertex_shader, state->pixel_shader);

    BufferHandle vertex_buffer = create_buffer(state->graphics);
    state->draw_buffer->buffer = vertex_buffer;
    bind_buffer(state->graphics, vertex_buffer, ARRAY_BUFFER_TYPE);

    typedef struct Vertex
    {
        f32 position[2];
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
    
    // set_buffer_data(state->graphics, vertex_buffer, vertex_data, sizeof(vertex_data), ARRAY_BUFFER_TYPE);

    const char* input_layout_names[] = { "a_position", "a_color" };
    u32 input_layout_offsets[] = { OFFSETOF(Vertex, position), OFFSETOF(Vertex, color) };
    u32 input_layout_formats[] = { COUNTOF(Vertex, position), COUNTOF(Vertex, color) };

    InputLayoutHandle input_layout = create_input_layout(state->graphics, state->program,
                                                         input_layout_names, input_layout_offsets, input_layout_formats,
                                                         sizeof(Vertex), ARRAY_COUNT(input_layout_names));
    use_input_layout(state->graphics, input_layout);

    set_viewport(state->graphics, state->width, state->height);
    clear_color(state->graphics, 0.392f, 0.584f, 0.929f, 1.0f);

    // use_program(state->graphics, state->program);
    // draw_arrays(state->graphics, TRIANGLE_PRIMITIVE_TYPE, 0, 3);

    log_integer(width);
    log_integer(height);
    log_integer(state->graphics);
    log_integer(state->vertex_shader);
    log_integer(state->pixel_shader);
    log_integer(state->program);
    log_integer(vertex_buffer);
    log_integer(input_layout);
}

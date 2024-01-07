#ifndef H_DEMO_H

typedef uptr GraphicsHandle;
typedef uptr ShaderHandle;
typedef uptr ProgramHandle;
typedef uptr BufferHandle;
typedef uptr InputLayoutHandle;

// NOTE: WebGL 1.0 and 2.0 Specs
// https://registry.khronos.org/webgl/specs/1.0/
// https://registry.khronos.org/webgl/specs/2.0/

typedef enum ShaderType
{
    PIXEL_SHADER_TYPE=0x8B30,
    VERTEX_SHADER_TYPE=0x8B31,
} ShaderType;

typedef enum BufferType
{
    ARRAY_BUFFER_TYPE=0x8892,
} BufferType;

typedef enum PrimitiveType
{
    TRIANGLE_PRIMITIVE_TYPE=0x0004,
} PrimitiveType;

void platform_log_integer(u32 integer);

GraphicsHandle platform_create_graphics(void);
ShaderHandle platform_create_shader(GraphicsHandle graphics, ShaderType type, const u8* source);
ProgramHandle platform_create_program(GraphicsHandle graphics, ShaderHandle vertex_shader, ShaderHandle pixel_shader);
BufferHandle platform_create_buffer(GraphicsHandle graphics);
void platform_bind_buffer(GraphicsHandle graphics, BufferHandle buffer, BufferType type);
void platform_set_buffer_data(GraphicsHandle graphics, BufferHandle buffer, const void* buffer_data, u32 buffer_size, BufferType type);
InputLayoutHandle platform_create_input_layout(GraphicsHandle graphics, ProgramHandle program,
                                               const char** names, const u32* offsets, const u32* formats, u32 stride, u32 count);
void platform_use_input_layout(GraphicsHandle graphics, InputLayoutHandle input_layout);
void platform_set_viewport(GraphicsHandle graphics, u32 width, u32 height);
void platform_clear_color(GraphicsHandle graphics, f32 r, f32 g, f32 b, f32 a);
void platform_use_program(GraphicsHandle graphics, ProgramHandle program);
void platform_draw_arrays(GraphicsHandle graphics, PrimitiveType primitive_type, u32 offset, u32 count);

void init(u32 width, u32 height);

#define H_DEMO_H
#endif

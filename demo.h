#ifndef H_DEMO_H

typedef uptr GraphicsHandle;
typedef uptr ShaderHandle;
typedef uptr ProgramHandle;

typedef enum ShaderType
{
    PIXEL_SHADER_TYPE=0x8B30,
    VERTEX_SHADER_TYPE=0x8B31,
} ShaderType;

void platform_log_integer(u32 integer);

GraphicsHandle platform_create_graphics(void);
ShaderHandle platform_create_shader(GraphicsHandle graphics, ShaderType type, const u8* source);
ProgramHandle platform_create_program(GraphicsHandle graphics, ShaderHandle vertex_shader, ShaderHandle pixel_shader);
void init(u32 width, u32 height);

#define H_DEMO_H
#endif

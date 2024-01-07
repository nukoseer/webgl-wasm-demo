'use strict';

let application = document.getElementById("application");
let gl = 0;
let wasm = null;

let reference_id = 0;
let references = [];

function create_new_reference(object)
{
    let id = reference_id;
    
    references.push(object);
    ++reference_id;

    return id;
}

function get_object_from_reference(id)
{
    return references[id];
}

function string_length(memory, start_address)
{
    let length = 0;
    let address = start_address;

    while (memory[address] != 0)
    {
	++length;
	++address;
    }

    return length;
}

function convert_to_string(buffer, string_address)
{
    const memory = new Uint8Array(buffer);
    const length = string_length(memory, string_address);
    const bytes = new Uint8Array(buffer, string_address, length);

    return new TextDecoder().decode(bytes);
}


let vertex_shader_source = `#version 300 es
     
    // an attribute is an input (in) to a vertex shader.
    // It will receive data from a buffer
    layout (location = 0) in vec2 a_position;
    layout (location = 1) in vec3 a_color;

    out vec4 color;
    // all shaders have a main function
    void main() {
     
      // gl_Position is a special variable a vertex shader
      // is responsible for setting
      gl_Position = vec4(a_position, 0.0, 1.0);
      color = vec4(a_color, 1.0);
    }
    `;

let fragment_shader_source = `#version 300 es
     
    // fragment shaders don't have a default precision so we need
    // to pick one. highp is a good default. It means "high precision"
    precision highp float;

    in vec4 color;

    // we need to declare an output for the fragment shader
    out vec4 outColor;
     
    void main() {
      // Just set the output to a constant reddish-purple
      // outColor = vec4(1, 0, 0.5, 1);
      outColor = color;
    }
    `;

function create_shader(gl, type, source)
{
    let shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    let success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
    
    if (success)
    {
        return shader;
    }

    console.log(gl.getShaderInfoLog(shader));
    gl.deleteShader(shader);
}

function create_program(gl, vertex_shader, fragment_shader)
{
    let program = gl.createProgram();
    
    gl.attachShader(program, vertex_shader);
    gl.attachShader(program, fragment_shader);
    gl.linkProgram(program);
    
    var success = gl.getProgramParameter(program, gl.LINK_STATUS);
    
    if (success)
    {
        return program;
    }
    
    console.log(gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
}

function platform_create_shader(gl_reference, type, source)
{
    const buffer = wasm.instance.exports.memory.buffer;
    const shader_source = convert_to_string(buffer, source);

    const gl_object = get_object_from_reference(gl_reference);
    const shader = create_shader(gl_object, type, shader_source);
    const shader_reference = create_new_reference(shader);

    return shader_reference;
}

function platform_create_program(gl_reference, vertex_shader_reference, fragment_shader_reference)
{
    const gl_object = get_object_from_reference(gl_reference);
    const vertex_shader_object = get_object_from_reference(vertex_shader_reference);
    const fragment_shader_object = get_object_from_reference(fragment_shader_reference);
    
    const program = create_program(gl_object, vertex_shader_object, fragment_shader_object);
    const program_reference = create_new_reference(program);

    return program_reference;
}

function main()
{
    // let vertex_shader = create_shader(gl, gl.VERTEX_SHADER, vertex_shader_source);
    // let fragment_shader = create_shader(gl, gl.FRAGMENT_SHADER, fragment_shader_source);
    // let program = create_program(gl, vertex_shader, fragment_shader);

    let vertex_shader = get_object_from_reference(1);
    let fragment_shader = get_object_from_reference(2);
    let program = get_object_from_reference(3);

    let vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);

    let vertex_data = [
        -0.00, +0.75, 1.0, 0.0, 0.0,
        +0.75, -0.50, 0.0, 1.0, 0.0,
        -0.75, -0.50, 0.0, 0.0, 1.0,
    ];
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertex_data), gl.STATIC_DRAW);

    // TODO: Is it enough to use only one VAO and use it for everything? Probably, yes.
    let vao = gl.createVertexArray();
    gl.bindVertexArray(vao);
    
    let a_pos = 0;
    gl.vertexAttribPointer(a_pos, 2, gl.FLOAT, false, 5 * 4, 0);
    gl.enableVertexAttribArray(a_pos);

    let a_color = 1
    gl.vertexAttribPointer(a_color, 3, gl.FLOAT, false, 5 * 4, 2 * 4);
    gl.enableVertexAttribArray(a_color);

    gl.viewport(0, 0, application.width, application.height);
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.useProgram(program);

    gl.bindVertexArray(vao);

    let primitiveType = gl.TRIANGLES;
    let count = 3;
    let offset = 0;

    gl.drawArrays(primitiveType, offset, count);
}

function create_graphics()
{
    return application.getContext("webgl2");
}

function platform_create_graphics()
{
    gl = create_graphics();
    const gl_reference = create_new_reference(gl);

    return gl_reference;
}

function platform_log_integer(integer)
{
    console.log(integer);
}

WebAssembly.instantiateStreaming(
    fetch("./build/demo.wasm"),
    {
	env:
	{
	    platform_log_integer,
	    platform_create_graphics,
	    platform_create_shader,
	    platform_create_program,
	}
    }
).then((w) =>
    {
	wasm = w;

        wasm.instance.exports.init(application.width, application.height);
	main();
    }
);


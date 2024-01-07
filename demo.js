'use strict';

let application = document.getElementById("application");
let gl = 0;
let wasm = null;
let memory_buffer = 0;

// NOTE: 0 is invalid.
let reference_id = 1;
let references = [ 0 ];

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

function create_graphics()
{
    gl = application.getContext("webgl2");

    return gl;
}

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

function create_buffer(gl)
{
    return gl.createBuffer();
}

function bind_buffer(gl, buffer, type)
{
    return gl.bindBuffer(type, buffer);
}

function set_buffer_data(gl, data, type)
{
    // TODO: Parameterize gl.STATIC_DRAW
    return gl.bufferData(type, data, gl.STATIC_DRAW);
}

function create_input_layout(gl, program, names, offsets, formats, stride, count)
{
    let input_layout = {
	layout: [],
	stride: stride,
	count: count,
    };
    
    for (let i = 0; i < count; ++i)
    {
	const location = gl.getAttribLocation(program, names[i]);
	input_layout.layout.push({
		    location: location,
	            offset: offsets[i],
		    format: formats[i]});
    }

    return input_layout;
}


function use_input_layout(gl, input_layout)
{
    for (let i = 0; i < input_layout.count; ++i)
    {
        gl.vertexAttribPointer(input_layout.layout[i].location,
				      input_layout.layout[i].format, gl.FLOAT, false,
				      input_layout.stride, input_layout.layout[i].offset);
        gl.enableVertexAttribArray(input_layout.layout[i].location);	
    }
}

function set_viewport(gl, width, height)
{
    gl.viewport(0, 0, width, height);
}

function clear_color(gl, r, g, b, a)
{
    gl.clearColor(r, g, b, a);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT | gl.STENCIL_BUFFER_BIT);
}

function platform_create_shader(gl_reference, type, source)
{
    const shader_source = convert_to_string(memory_buffer, source);

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

function platform_create_buffer(gl_reference)
{
    const gl_object = get_object_from_reference(gl_reference);
    const buffer = create_buffer(gl_object);
    const buffer_reference = create_new_reference(buffer);

    return buffer_reference;
}

function platform_bind_buffer(gl_reference, buffer_reference, type)
{
    const gl_object = get_object_from_reference(gl_reference);
    const buffer_object = get_object_from_reference(buffer_reference);

    bind_buffer(gl_object, buffer_object, type);
}

function platform_set_buffer_data(gl_reference, buffer_reference, buffer_data, buffer_size, type)
{
    const gl_object = get_object_from_reference(gl_reference);
    const buffer_object = get_object_from_reference(buffer_reference);

    const data = new Float32Array(memory_buffer, buffer_data, buffer_size);

    bind_buffer(gl_object, buffer_object, type);
    set_buffer_data(gl_object, data, type);
}

function platform_create_input_layout(gl_reference, program_reference, names, offsets, formats, stride, count)
{
    const gl_object = get_object_from_reference(gl_reference);
    const program_object = get_object_from_reference(program_reference);
    
    const string_address_array = new Uint32Array(memory_buffer, names, count);
    const offset_array = new Uint32Array(memory_buffer, offsets, count);
    const format_array = new Uint32Array(memory_buffer, formats, count);
    const names_array = [];

    for (let i = 0; i < count; ++i)
    {
        names_array.push(convert_to_string(memory_buffer, string_address_array[i]));
    }

    const input_layout = create_input_layout(gl_object, program_object, names_array, offset_array, format_array, stride, count);
    
    return create_new_reference(input_layout);
}

function platform_use_input_layout(gl_reference, input_layout_reference)
{
    const gl_object = get_object_from_reference(gl_reference);
    const input_layout_object = get_object_from_reference(input_layout_reference);

    use_input_layout(gl_object, input_layout_object);
}

function platform_set_viewport(gl_reference, width, height)
{
    const gl_object = get_object_from_reference(gl_reference);

    set_viewport(gl_object, width, height);
}

function platform_clear_color(gl_reference, r, g, b, a)
{
    const gl_object = get_object_from_reference(gl_reference);

    clear_color(gl_object, r, g, b, a);
}

function use_program(gl, program)
{
    gl.useProgram(program);
}

function platform_use_program(gl_reference, program_reference)
{
    const gl_object = get_object_from_reference(gl_reference);
    const program_object = get_object_from_reference(program_reference);

    use_program(gl_object, program_object);
}

function draw_arrays(gl, primitive_type, offset, count)
{
    gl.drawArrays(primitive_type, offset, count);
}

function platform_draw_arrays(gl_reference, primitive_type, offset, count)
{
    const gl_object = get_object_from_reference(gl_reference);

    draw_arrays(gl_object, primitive_type, offset, count);
}

// function main()
// {
    // let vertex_shader = create_shader(gl, gl.VERTEX_SHADER, vertex_shader_source);
    // let fragment_shader = create_shader(gl, gl.FRAGMENT_SHADER, fragment_shader_source);
    // let program = create_program(gl, vertex_shader, fragment_shader);

    // let vbo = gl.createBuffer();
    // gl.bindBuffer(gl.ARRAY_BUFFER, vbo);

    // let vertex_data = [
    //     -0.00, +0.75, 1.0, 0.0, 0.0,
    //     +0.75, -0.50, 0.0, 1.0, 0.0,
    //     -0.75, -0.50, 0.0, 0.0, 1.0,
    // ];
    // gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertex_data), gl.STATIC_DRAW);

    // let vao = gl.createVertexArray();
    // gl.bindVertexArray(vao);

    // let a_pos = 0;
    // gl.vertexAttribPointer(a_pos, 2, gl.FLOAT, false, 5 * 4, 0);
    // gl.enableVertexAttribArray(a_pos);

    // let a_color = 1
    // gl.vertexAttribPointer(a_color, 3, gl.FLOAT, false, 5 * 4, 2 * 4);
    // gl.enableVertexAttribArray(a_color);

    // gl.viewport(0, 0, application.width, application.height);
    // gl.clearColor(0, 0, 0, 0);
    // gl.clear(gl.COLOR_BUFFER_BIT);
    
    // let program = get_object_from_reference(4);

    // gl.useProgram(program);

    // gl.bindVertexArray(vao);
    
    // let primitiveType = gl.TRIANGLES;
    // let count = 3;
    // let offset = 0;

    // gl.drawArrays(primitiveType, offset, count);
// }

function platform_create_graphics()
{
    let gl_reference = 0;

    if (!gl)
    {
        let gl_object = create_graphics();
        gl_reference = create_new_reference(gl_object);
	// TODO: Is it enough to create only one VAO, bind it at the beginning of the program
	// and use it forever? Probably, yes. I heard many times that VAOs are pretty useless.
	const vao = gl.createVertexArray();
	gl.bindVertexArray(vao);
    }

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
	    platform_create_buffer,
	    platform_bind_buffer,
	    platform_set_buffer_data,
	    platform_create_input_layout,
	    platform_use_input_layout,
	    platform_set_viewport,
	    platform_clear_color,
	    platform_use_program,
	    platform_draw_arrays,
	}
    }
).then((w) =>
    {
	wasm = w;
	memory_buffer = wasm.instance.exports.memory.buffer;

        wasm.instance.exports.init(application.width, application.height);
	// main();
    }
);


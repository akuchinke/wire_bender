#version 150

uniform mat4 transform;
uniform mat4 viewprojection;

in vec3 in_vertex;

out vec4 vertex;

void main()
{
    gl_Position = viewprojection * transform * vec4(in_vertex, 1.0);
    vertex = transform * vec4(in_vertex, 1.0);
}

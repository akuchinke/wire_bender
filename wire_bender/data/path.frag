#version 150

in vec4 vertex;

out vec4 out_color;

uniform vec4 color;

void main()
{
    out_color = color;
}

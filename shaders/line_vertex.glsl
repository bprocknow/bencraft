#version 300 es

uniform mat4 matrix;
attribute vec4 vPosition;
uniform vec4 color;
out vec4 f_color;

void main()
{
    f_color = color;
    gl_Position = matrix * vPosition;
}

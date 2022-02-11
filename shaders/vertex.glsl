#version 300 es

uniform mat4 matrix;
attribute vec4 vPosition;
attribute vec2 a_textCoord;

out vec2 f_textCoord;

void main()
{
    f_textCoord = a_textCoord;
    gl_Position = matrix * vPosition; 
}

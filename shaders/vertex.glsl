#version 110

uniform mat4 matrix;
attribute vec4 vPosition;
attribute vec4 color;
varying vec4 f_color;

void main()
{
    f_color = color; 

    gl_Position = matrix * vPosition; 
}

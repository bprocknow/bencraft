#version 110

uniform float rotation;
attribute vec3 vPosition;

void main()
{
    vec4 pos;
    float rad = radians(rotation);
    pos.x = vPosition.x*cos(rotation) + vPosition.y*sin(rotation);
    pos.y = -vPosition.x*sin(rotation) + vPosition.y*cos(rotation);
    pos.z = 0.0;
    pos.w = 1.0;
    gl_Position = pos;
}

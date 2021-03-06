#version 300 es

precision mediump float;
in vec2 f_textCoord;
uniform sampler2D s_texture;


void main() {
    vec3 color = vec3(texture2D(s_texture, f_textCoord));
    gl_FragColor = vec4(color, 1.0);
}

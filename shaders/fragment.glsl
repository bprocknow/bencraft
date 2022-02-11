#version 300 es

precision mediump float;
in vec2 f_textCoord;
uniform sampler2D s_texture;


void main() {
    gl_FragColor = texture2D(s_texture, f_textCoord);
}

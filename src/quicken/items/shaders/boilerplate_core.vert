#version 150 core

in vec4 positionAttrib;
in vec4 colorAttrib;
out vec4 color;
uniform mat4 matrix;

void main() {
    color = colorAttrib;
    gl_Position = matrix * positionAttrib;
}

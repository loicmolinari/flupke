#version 150 core

in vec4 color;
out vec4 fragColor;
uniform float opacity;

void main() {
  fragColor = vec4(opacity) * color;
}

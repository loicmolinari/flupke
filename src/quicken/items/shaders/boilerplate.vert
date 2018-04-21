attribute highp vec4 positionAttrib;
attribute lowp vec4 colorAttrib;
varying lowp vec4 color;
uniform highp mat4 matrix;

void main() {
    color = colorAttrib;
    gl_Position = matrix * positionAttrib;
}

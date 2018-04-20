varying lowp vec4 color;
uniform lowp float opacity;

void main() {
    gl_FragColor = vec4(opacity) * color;
}

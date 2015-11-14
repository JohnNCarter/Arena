#version 400

//
// Not a good idea as attribute is optimised out and causes error.
//

precision highp float;

in  vec3 vEx_Color;

out vec4 fFragColor;

void main(void) {
    fFragColor = vec4(0.0);
}

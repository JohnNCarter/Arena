#version 400

precision highp float;

in vec4 aPosition;
//in vec3 aColor;
in vec3 aNormal;

uniform mat4 uMVPmatrix;

//out vec3 vEx_Color;
out vec3 vEx_Normal;

void main(void) {
    gl_Position = uMVPmatrix * aPosition;
//    vEx_Color = aColor;
    vEx_Normal = aNormal;
}

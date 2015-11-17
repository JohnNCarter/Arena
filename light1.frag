#version 400

precision highp float;

uniform vec4 uAmbientLight;
uniform vec4 uDiffuseLight;
uniform vec3 uLightVector;

//in  vec3 vEx_Color;
in  vec3 vEx_Normal;

out vec4 fFragColor;

void main(void) {
    float cos_angle = dot(vEx_Normal, uLightVector);
//    cos_angle = .5;
    if(cos_angle < 0.)
        cos_angle *= -1.0;
    vec4 t = uAmbientLight + uDiffuseLight;
    fFragColor = t; //vec4(t,1.0);
}

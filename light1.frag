#version 400

precision highp float;

uniform vec4 uAmbientLight;
uniform vec4 uDiffuseLight;
uniform vec3 uLightVector;

//in  vec3 vEx_Color;
in  vec3 vEx_Normal;

out vec4 fFragColor;

void main(void) {
    vec4 t;
    float cos_angle = dot(vEx_Normal, uLightVector);
    cos_angle = clamp(cos_angle, 0.0, 1.0);
    t = vec4(vEx_Normal, 1.);
    t = vec4(cos_angle, 0., 0., 1.);
    t = uAmbientLight + uDiffuseLight * cos_angle;
//    t = uDiffuseLight * cos_angle;
    fFragColor = clamp(t, 0., 1.);
}

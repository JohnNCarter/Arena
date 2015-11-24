#version 400

precision highp float;

uniform vec4 uAmbientLight;
uniform vec4 uDiffuseLight;
uniform vec3 uLightVector;

//in  vec3 vEx_Color;
flat in  vec3 vEx_Normal;

out vec4 fFragColor;

void main(void) {
    vec4 t;
    float cos_angle = dot(vEx_Normal, uLightVector);
    cos_angle = clamp(cos_angle, 0.0, 1.0);
    t = uAmbientLight * 0.000001 + uDiffuseLight * cos_angle * 0.000001;
    vec3 xx = 0.5 + vEx_Normal/2.;
    t = t + vec4(xx, 1.);

//    t = uAmbientLight + t * 0.000001;
    fFragColor = clamp(t, 0., 1.);
}

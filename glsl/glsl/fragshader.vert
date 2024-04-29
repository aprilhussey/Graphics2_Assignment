#version 440   
in vec4 P;
in vec3 N;
in vec2 TexCoord;

uniform sampler2D Tex1;

out vec4 FragColor;

uniform mat4 View;

struct LightInfo {
    vec4 LightPosition;
    vec3 La;
    vec3 L;
};
uniform LightInfo Light;

struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float  Shininess;
};
uniform MaterialInfo Material;

vec3 phoneShading(vec3 N, vec4 P, out vec3 dif) {
    vec4 LightP = View * Light.LightPosition;
    vec3 L = normalize(vec3(LightP - P));
    vec3 V = normalize(vec3(vec4(0.0, 0.0, 0.0, 1.0) - P));
    vec3 H = normalize(L + V); 

    float LDotN  = dot(L, N); 
    float HDotN  = max(dot(H, N), 0.0);

    vec3 ambient = Light.La * Material.Ka;
    vec3 diffuse = Light.L * Material.Kd * LDotN;
    vec3 specula = vec3(0.0, 0.0, 0.0);
    if (LDotN > 0.0) {
       if (HDotN == 0.0) HDotN =1.0;
       specula = Light.L * Material.Ks * pow(HDotN, Material.Shininess); 
    }
    dif = ambient + diffuse;
    return specula;
}

void main() {
    vec3 dif;
    vec3 spe;
    vec4 texColor = texture(Tex1, TexCoord);
    spe = phoneShading(N, P, dif); 
    FragColor =  vec4(dif, 1.0) * texColor + vec4(spe, 1.0);

    //if ( dif.r < 0.5) discard;
}

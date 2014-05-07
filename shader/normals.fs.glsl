

in vec3 outNormal;
out vec4 oColor;

vec3 light=vec3(0,2,1);
vec4 diffuse=vec4(1.0,1.0,1.0,1.0);

void main(){
    
    vec3  lightDirection=normalize(light);
    float intensity=max(dot(outNormal, lightDirection), 0.0);
    
    oColor =  diffuse*intensity;
}
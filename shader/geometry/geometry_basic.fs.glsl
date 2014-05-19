//inputs
in  vec3  outNormal;
in  vec4 outShadow;
out vec4 oColor;

//uniforms
uniform vec3 light;
uniform vec4 diffuse;
uniform sampler2DShadow shadowMap;

//globals
const float zBias = 0.005; 
//kernel
void main(){
    //normals
    vec3  lightDirection=normalize(light);
    float intensity=max(dot(outNormal, lightDirection), 0.0);
    //shadow
    float visibility = 1.0;
    if( outShadow.w > 0.0) {
        //get shadow value
        vec3  sCoors= vec3(outShadow.xy ,outShadow.z-zBias) / outShadow.w;
        float sDepth = texture(shadowMap, sCoors );       
        //zbuffer
        float depth = outShadow.z / outShadow.w;
        //is it a pixel shadows?
        if(sDepth < depth) {
          //shadow color
          visibility = 0.5; 
        }
    }
    //ouput
    oColor =  diffuse*intensity*visibility;
}


in vec3 outNormal;
out vec4 oColor;

uniform vec3 light;//=vec3(0,0,1);
uniform vec4 diffuse;//=vec4(1.0,1.0,1.0,1.0);

#ifdef EnableShadow
uniform sampler2DShadow shadowMap;
in  vec4 outShadowCoord;
#endif    

void main(){

    float visibility = 1.0;
    vec3  lightDirection=normalize(light);
    float intensity=max(dot(outNormal, lightDirection), 0.0);
    
#ifdef EnableShadow
#if 1
    const float bias = 0.005; 
    if( outShadowCoord.w > 0.0) {
        vec3  dcoors= vec3(outShadowCoord.xy,outShadowCoord.z-bias) / outShadowCoord.w;
        float depth = outShadowCoord.z / outShadowCoord.w;
        float val = texture(shadowMap, dcoors );       
        if(val < depth) {
          visibility = max(val,0.5);   //shadow
        }
    }
#else    
    if( outShadowCoord.w > 0.0) {
            visibility = textureProj( shadowMap, outShadowCoord );
    }
#endif
    
#endif  

    oColor =  diffuse*intensity*visibility;
}
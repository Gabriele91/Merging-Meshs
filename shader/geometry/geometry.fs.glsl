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
vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);
//kernel
void main(){
    //normals
    vec3  lightDirection=normalize(light);
    float intensity=max(dot(outNormal, lightDirection), 0.0);
    //shadow
    float visibility = 1.0;
	const float sizetexture = 1.0f / 102.4f;

	for (int i=0;i<16;i++){
		if( outShadow.w > 0.0) {
			//get shadow value
			vec2 coord = outShadow.xy + poissonDisk[i] * sizetexture;
			vec3  sCoors= vec3( coord ,outShadow.z-zBias) / outShadow.w;
			float sDepth = texture(shadowMap, sCoors );       
			//zbuffer
			float depth = outShadow.z / outShadow.w;
			//is it a pixel shadows?
			if(sDepth < depth) {
				//shadow color
				visibility -= 0.022; 
			}
		}
	}
	
    //ouput
    oColor =  diffuse*intensity*visibility;
}
//inputs
in  vec3 inPosition;
in  vec3 inNormal;
out vec3 outNormal;
out vec4 outShadow;
//uniform camera
uniform mat4 projection;
uniform mat4 view;
//uniform shadow camera
uniform mat4 projectionSW;
uniform mat4 viewSW;
//uniform model
uniform mat4 model;
//shadow bias
const mat4 bias= mat4(0.5, 0.0, 0.0, 0.0,
                      0.0, 0.5, 0.0, 0.0,
                      0.0, 0.0, 0.5, 0.0,
                      0.5, 0.5, 0.5, 1.0);
//kernel
void main(){
    outNormal   =  normalize((model * vec4(inNormal,0.0)).xyz);	
	outShadow   =  bias * projectionSW * viewSW *  model  * vec4(inPosition, 1.0) ;
	gl_Position =  projection * view *  model  * vec4(inPosition, 1.0) ;
}
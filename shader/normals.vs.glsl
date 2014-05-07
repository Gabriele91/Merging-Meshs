
in  vec3 inPosition;
in  vec3 inNormal;
out vec3 outNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
    outNormal   = normalize((model * vec4(inNormal,0.0)).xyz);
	gl_Position = projection * model * view * vec4(inPosition, 1.0);
 }
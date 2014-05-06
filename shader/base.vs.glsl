
in  vec3 inPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){    
	gl_Position = projection * model * view * vec4(inPosition, 1.0);
 }
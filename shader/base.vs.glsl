
in  vec3 inPosition;
in  vec4 inColor;
out vec4 vtColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){    
	gl_Position = projection * model * view * vec4(inPosition, 1.0);
    vtColor     = inColor;
 }
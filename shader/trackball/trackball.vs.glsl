
in  vec3 inPosition;
in  vec4 inColor;
out vec4 vColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){    
	vColor=inColor;
	gl_Position = projection * ( view * ( model  * vec4(inPosition, 1.0) ) );
 }
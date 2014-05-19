
in  vec3 inPosition;
in  vec3 inNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	gl_Position = projection * ( view * ( model  * vec4(inPosition, 1.0) ) );
}
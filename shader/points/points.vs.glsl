
in  vec3 inPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){    
	gl_PointSize = 10.0;
	gl_Position = (projection * view) * ( model  * vec4(inPosition, 1.0) ) ;
 }
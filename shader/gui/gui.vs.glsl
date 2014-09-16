
in  vec2 inPosition;
in  vec2 inCoords;
in  vec4 inColor;

out vec2 vCoords;
out vec4 vColor;

uniform mat4 projection;

void main(){
    //outs
	vCoords=inCoords;
	vColor=inColor;
    //position
	gl_Position = projection * vec4(inPosition, 0.0, 1.0);
 }
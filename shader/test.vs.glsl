
in  vec3 inPosition;
in  vec3 inColor;
out vec4 vtColor;

uniform vec4 constVec4;
uniform mat4 constMat4;
uniform float constAlpha;

void main(){    
	gl_Position = constMat4*(constVec4+vec4(inPosition, 1.0));
    vtColor     = vec4(inColor,1.0)*constAlpha;
 }
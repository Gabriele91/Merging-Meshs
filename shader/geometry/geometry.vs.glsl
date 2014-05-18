
in  vec3 inPosition;
in  vec3 inNormal;
out vec3 outNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

#ifdef EnableShadow
const mat4 bias= mat4(0.5, 0.0, 0.0, 0.0,
                      0.0, 0.5, 0.0, 0.0,
                      0.0, 0.0, 0.5, 0.0,
                      0.5, 0.5, 0.5, 1.0);

out  vec4 outShadowCoord;
#endif

void main(){    
    //view camera
     float theta=-3.141529*0.1;
     mat4 sdview= mat4(cos (theta), 0.0, - sin(theta), 0.0,
                       0.0,         1.0,          0.0, 0.0,
                       sin (theta), 0.0,   cos(theta), 0.0,
                       0.0,         0.0,         -6.0, 1.0);
    //normal
    outNormal      = normalize((model * vec4(inNormal,0.0)).xyz);	
   
#ifdef EnableShadow
    //camera
	outShadowCoord =  bias * projection * sdview * model  * vec4(inPosition, 1.0) ;
    //light
	gl_Position    =  projection *  view * model * vec4(inPosition, 1.0);
    #else
    //light
	gl_Position    =  projection * sdview *  model  * vec4(inPosition, 1.0);
#endif

}
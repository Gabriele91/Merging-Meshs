
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 size;

void main(){
    
    vec4 center = view * model * gl_in[0].gl_Position;
    
    vec4 p[4];
    p[0]=projection * center + vec4(  size.x,   size.y, 0, 0);
    p[1]=projection * center + vec4(  size.x,  -size.y, 0, 0);
    p[2]=projection * center + vec4( -size.x,   size.y, 0, 0);
    p[3]=projection * center + vec4( -size.x,  -size.y, 0, 0);
    
    for(int i=0;i!=4;++i){
        gl_Position = p[i];
        EmitVertex();
    }
 }
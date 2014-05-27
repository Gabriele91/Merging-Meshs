
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 size;

void main(){
    
    vec4 center = projection * view * model * gl_in[0].gl_Position;
    
    vec4 p[4];
    p[0]=center + vec4(  size.x,   size.y, 0.0f, 0.0f);
    p[1]=center + vec4(  size.x,  -size.y, 0.0f, 0.0f);
    p[2]=center + vec4( -size.x,   size.y, 0.0f, 0.0f);
    p[3]=center + vec4( -size.x,  -size.y, 0.0f, 0.0f);
    
    for(int i=0;i!=4;++i){
        gl_Position = p[i];
        EmitVertex();
    }
 }
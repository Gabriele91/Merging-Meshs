//color ouput
out vec4 oColor;
//shader input
in vec2 vCoords;
in vec4 vColor;
//from program
uniform sampler2D textureFont;
uniform vec4 scissor;
uniform vec4 viewport;

void main()
{
    if(scissor.x > gl_FragCoord.x) discard;
    if(scissor.z < gl_FragCoord.x) discard;
    if((viewport.w-scissor.y) < gl_FragCoord.y) discard;
    if((viewport.w-scissor.w) > gl_FragCoord.y) discard;
    oColor = vColor * texture(textureFont,vCoords);
}
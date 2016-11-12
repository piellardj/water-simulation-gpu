#version 130


uniform float intensity=0.2;

out vec4 fragColor;


void main()
{
    float dist = 2 * length(gl_PointCoord.xy-vec2(0.5));
    fragColor = vec4(intensity);// * (1 - dist);
}

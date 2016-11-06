#version 130

uniform sampler2D oldPositions;
uniform vec2 bufferSize;

uniform vec2 coords;
uniform float strength;
uniform float radius;

out vec4 fragColor;


__UTILS__


void main()
{
    vec2 coordOnBuffer = gl_FragCoord.xy / bufferSize;
    
    float pos = colorToPos(texture(oldPositions, coordOnBuffer));
    
    vec2 dCoord = coords - gl_FragCoord.xy;
    float distance = length(dCoord);
    if (distance < radius) {
        distance = distance / radius * 3.1415;
        pos -= POS_RANGE * strength * (cos(distance) + 1) / 4;
    }
    
    fragColor = posToColor(pos);
}

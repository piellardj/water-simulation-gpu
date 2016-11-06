#version 130

uniform sampler2D oldBuffer;
uniform vec2 bufferSize;

uniform vec2 coords;
uniform float strength;
uniform float radius;

out vec4 fragColor;


__UTILS__


/* Pushes the water following a 2D cos curve */
void main()
{
    vec2 coordOnBuffer = gl_FragCoord.xy / bufferSize;
    
    vec2 dCoord = coords*bufferSize - gl_FragCoord.xy;
    float distance = length(dCoord);
    
    float r = distance / radius * 3.1415;
    float dPos = -POS_RANGE * strength * (cos(r) + 1) / 4;
    dPos *= step(distance, radius); //no perturbation outside the disk
    
    vec4 color = texture(oldBuffer, coordOnBuffer);
    float pos = vecToValue(color.rg, POS_RANGE);
    color.rg = valueToVec(pos + dPos, POS_RANGE);
    //color.rg = valueToVec(pos + 0.1, POS_RANGE);
    
    fragColor = color;
}

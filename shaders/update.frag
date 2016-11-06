#version 130

uniform sampler2D oldBuffer;
uniform vec2 bufferSize;

uniform float dt;

uniform float c = 20.0; //coherence
uniform float k = 0.7;//.8; //raideur
uniform float f = 0.995; //atténuation

out vec4 fragColor;


__UTILS__


void main()
{
    vec2 coordOnBuffer = gl_FragCoord.xy / bufferSize;
    vec2 pixelSize = 1.0 / bufferSize;
    vec4 color = texture(oldBuffer, coordOnBuffer);
    
    /* Fetch current state */
    float pos = vecToValue(color.rg, POS_RANGE);
    float vel = vecToValue(color.ba, VEL_RANGE);
    
    /* Update velocity */
    vel += -dt * k * pos; //vertical spring
    
    float neighbours = vecToValue(texture(oldBuffer, coordOnBuffer + vec2(+1,0)*pixelSize).rg, POS_RANGE) +
                       vecToValue(texture(oldBuffer, coordOnBuffer + vec2(0,+1)*pixelSize).rg, POS_RANGE) +
                       vecToValue(texture(oldBuffer, coordOnBuffer + vec2(-1,0)*pixelSize).rg, POS_RANGE) +
                       vecToValue(texture(oldBuffer, coordOnBuffer + vec2(0,-1)*pixelSize).rg, POS_RANGE);
    neighbours *= 0.25;
    
    vel += dt * c * (neighbours - pos); //surface tension
    vel *= f; //attenuation
    
    
    /* Update position */
    pos += dt * vel;
    
    fragColor = vec4(valueToVec(pos, POS_RANGE),
                     valueToVec(vel, VEL_RANGE));
}

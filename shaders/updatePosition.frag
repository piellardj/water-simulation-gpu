#version 130

uniform sampler2D velocities;
uniform sampler2D oldPositions;
uniform vec2 bufferSize;

uniform float dt;

out vec4 fragColor;


__UTILS__


void main()
{
    vec2 coordOnBuffer = gl_FragCoord.xy / bufferSize;
    
    float pos = colorToPos(texture(oldPositions, coordOnBuffer));
    float vel = colorToPos(texture(velocities, coordOnBuffer));
    
    pos += dt * vel;
    
    fragColor = posToColor(pos);
}

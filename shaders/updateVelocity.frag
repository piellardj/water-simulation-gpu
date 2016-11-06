#version 130

uniform sampler2D oldVelocities;
uniform sampler2D positions;
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
    
    float pos = colorToPos(texture(positions, coordOnBuffer));
    float vel = colorToVel(texture(oldVelocities, coordOnBuffer));
    
    vel += -dt * k * pos; //ressort vertical
    
    float neighbours = colorToPos(texture(positions, coordOnBuffer + vec2(1,0)*pixelSize)) +
                       colorToPos(texture(positions, coordOnBuffer + vec2(0,1)*pixelSize)) +
                       colorToPos(texture(positions, coordOnBuffer + vec2(-1,0)*pixelSize)) +
                       colorToPos(texture(positions, coordOnBuffer + vec2(0,-1)*pixelSize));
    neighbours *= 0.25;
    
    vel += dt * c * (neighbours - pos);
    
    fragColor = velToColor(vel * f);
}

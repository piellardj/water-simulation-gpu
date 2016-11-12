#version 130


uniform sampler2D oldGrid;
uniform vec2 cellSize;

uniform float dt;

uniform float c = 20.0; //surface tension
uniform float k = 0.2; // vertical spring's stiffness
uniform float f = 0.995; //friction

out vec4 fragColor;


__UTILS__


/* Updates the fragment position and velocity.
 * Each fragment is supposed to be a grid cell.
 */
void main()
{
    /* Normalized coords */
    vec2 coordsOnGrid = gl_FragCoord.xy * cellSize;
    vec4 cellColor = texture(oldGrid, coordsOnGrid);
    
    /* Fetch current state */
    float pos = vecToValue(cellColor.rg, POS_RANGE);
    float vel = vecToValue(cellColor.ba, VEL_RANGE);
    
    /* Update velocity */
    vel += -dt * k * pos; //vertical spring
    
    float neighboursPos = vecToValue(texture(oldGrid, coordsOnGrid + vec2(cellSize.x, 0)).rg, POS_RANGE) +
                          vecToValue(texture(oldGrid, coordsOnGrid - vec2(cellSize.x, 0)).rg, POS_RANGE) +
                          vecToValue(texture(oldGrid, coordsOnGrid + vec2(0,cellSize.y)).rg, POS_RANGE) +
                          vecToValue(texture(oldGrid, coordsOnGrid - vec2(0,cellSize.y)).rg, POS_RANGE);
    neighboursPos *= 0.25;
    
    vel += dt * c * (neighboursPos - pos); //surface tension
    vel *= f; //attenuation
    
    /* Update position */
    pos += dt * vel;
    
    
    fragColor = vec4(valueToVec(pos, POS_RANGE),
                     valueToVec(vel, VEL_RANGE));
}

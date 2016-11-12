#version 130


uniform sampler2D oldGrid;
uniform vec2 cellSize;

// both coords and radius are in normalized grid coordinates
uniform vec2 coords;
uniform float radius;

// expected in [-1, 1]
uniform float extremum;

out vec4 fragColor;


__UTILS__


/* Expects a parameter in [0,1].
 * Garanteed bumpFunction(0) = 1,
 *           bumpFuncion(1) = 0
 */
float bumpFunction (float param)
{
    /* Cosine */
    float value = (cos(param*3.141592654) + 1) / 2;
    return value * step(param, 1);
}

/* Pushes the water around coords following a 2D cos curve.
 * Each fragment is supposed to be a grid cell.
 */
void main()
{
    /* Normalized coords */
    vec2 coordsOnGrid = gl_FragCoord.xy * cellSize;
    vec4 cellColor = texture(oldGrid, coordsOnGrid);
    
    /* First compute the displacement */
    float distance = length(coords - coordsOnGrid);
    float param = distance / radius;
    float dHeight = 0.5 * POS_RANGE * extremum * bumpFunction(param);
    
    /* Then add it to the current height */
    float height = vecToValue(cellColor.rg, POS_RANGE);
    height += dHeight;
    cellColor.rg = valueToVec(height, POS_RANGE);
    
    fragColor = cellColor;
}

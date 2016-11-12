#version 130


uniform sampler2D grid;
uniform vec2 cellSize;

out vec4 fragColor;


__UTILS__


/* Returns height relative to the '0' level for an amplitude of 1.
 * Is garanteed to be in [-0.5,0.5]
 */
float computeRelativeHeight (const vec2 coords)
{
    return vecToValue(texture(grid, coords).rg, POS_RANGE) / POS_RANGE;
}

/* Returns normalized normal for an amplitude of 1,
 * computed as the cross product of the partial derivatives.
 */
vec3 computeNormal (const vec2 coords)
{
    vec3 partialX = vec3(2*cellSize.x, 0, 0);
    partialX.z = computeRelativeHeight(coords + vec2(cellSize.x,0)) -
                 computeRelativeHeight(coords - vec2(cellSize.x,0));
    
    vec3 partialY = vec3(0, 2*cellSize.y, 0);
    partialY.z = computeRelativeHeight(coords + vec2(0, cellSize.y)) -
                 computeRelativeHeight(coords - vec2(0, cellSize.y));
    
    return normalize(cross(partialX, partialY));
}

void main()
{
    vec2 coordsOnBuffer = gl_FragCoord.xy * cellSize;
    
    vec3 normal = computeNormal(coordsOnBuffer);
    float height = computeRelativeHeight(coordsOnBuffer);
    
    fragColor = vec4(normal*0.5 + 0.5, height + 0.5);
}

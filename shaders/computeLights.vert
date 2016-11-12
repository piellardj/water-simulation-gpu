#version 130


uniform sampler2D heightmap;
uniform vec2 cellSize; //cell size on heightmap

uniform float amplitude=0.2; //waves amplitude
uniform float waterLevel=0.5; //distance from the '0' and the ground

uniform float eta=0.8;

uniform vec3 normalizedLightDir = normalize(vec3(-1,-1,-5));

attribute vec2 pos;


__UTILS__


/* Extracts relative height from the pixel color on the heightmap.
 * Returns a height in [-amplitude/2, amplitude/2]
 */
float computeRelativeHeight (const vec4 color)
{
    return (color.a - 0.5) * amplitude;
}

/* Extracts absolute height from the pixel color on the heightmap.
 */
float computeHeight (const vec4 color)
{
    return waterLevel + computeRelativeHeight(color);
}

/* Extracts normal from the pixel color on the heightmap.
 * Returns normalized normal.
*/
vec3 computeNormal (const vec4 color)
{
    vec3 normal = color.rgb - 0.5;
    normal.xy *= amplitude;
    return normalize(normal);
}


void main()
{
    vec4 heightColor = texture(heightmap, pos);
    
    float vertHeight = computeHeight(heightColor);
    vec3 normalizedNormal = computeNormal(heightColor);
    
    // ray from the water surface to the bottom
    vec3 refractedRay = refract(normalizedLightDir, normalizedNormal, eta);
    refractedRay *= -vertHeight / refractedRay.z;
    
    // Some rays may land outside the box, we wrap them into it
    vec2 coordsOnTiles = mod(pos.xy + refractedRay.xy, 1.0);
    vec2 coordsOnScreen = coordsOnTiles*2-1;//mod(coordsOnTiles*2-1, 1.0);
    
    gl_Position = vec4(coordsOnScreen,0,1);//coordsOnScreen, 0, 1);
}

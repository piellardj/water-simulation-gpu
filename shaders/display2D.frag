#version 130


uniform sampler2D heightmap;
uniform sampler2D groundTexture;

uniform float amplitude=0.2; //waves amplitude
uniform float waterLevel=0.5; //distance from the '0' and the ground

uniform float eta=0.8;
uniform vec4 waterColor = vec4(0,0,1,0);
uniform float viewDistance = 2.0;

uniform vec3 normalizedLightDir = normalize(vec3(-1,-1,-5));

in vec2 coordsOnHeightmap;
out vec4 fragColor;


__UTILS__


/* Extracts relative height from the pixel color on the heightmap.
 * Returns a height in [-amplitude/2, amplitude/2]
 */
float computeRelativeHeight (const vec4 color)
{
    return 0.0;//(color.a - 0.5) * amplitude;
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

/* Computes specularity factor
*/
vec4 computeSpecularity (const vec3 normalizedNormal,
                         const vec3 normalizedFromEye)
{
    vec3 normalizedReflected = normalize(reflect(normalizedLightDir, normalizedNormal));
    
    vec4 sparkleColor = vec4(1);
    float sparkleFactor = dot(normalizedReflected, -normalizedFromEye);
    
    return sparkleColor * smoothstep(.98, 1, sparkleFactor);
}

/* Computes surface color: refracted ground + water opacity factor
*/
vec4 computeSurfaceColor (const vec3 position,
                          const vec3 normalizedNormal,
                          const vec3 normalizedFromEye)
{
    /* Refracted ray going from the position to the ground */
    vec3 refracted = refract(normalizedFromEye, normalizedNormal, eta);
    
    if (refracted.z >= 0)
        return waterColor;
    
    refracted *= -position.z / refracted.z;
    
    vec2 coordsOnGround= coordsOnHeightmap + refracted.xy;
    vec4 tilesColor = texture(groundTexture, coordsOnGround);
    
    float opacityFactor = clamp(length(refracted) / viewDistance, 0, 1);
    return mix(tilesColor, waterColor, opacityFactor);
}

void main()
{
    vec4 heightColor = texture(heightmap, coordsOnHeightmap);
    
    float height = computeHeight(heightColor);
    
    vec3 fragPosition = vec3(coordsOnHeightmap, height);
    const vec3 normalizedFromEye = normalize(vec3(0,0,-1));
    vec3 normalizedNormal = computeNormal(heightColor);
    
    fragColor = computeSurfaceColor(fragPosition, normalizedNormal, normalizedFromEye) +
                computeSpecularity(normalizedNormal, normalizedFromEye);
}

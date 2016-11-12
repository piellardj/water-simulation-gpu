#version 130


uniform sampler2D heightmap;
uniform sampler2D groundTexture;
uniform sampler2D lightsTexture;

uniform float amplitude=0.2; //waves amplitude
uniform float waterLevel=0.5; //distance from the '0' and the ground

uniform float eta=0.8;
uniform vec4 waterColor = vec4(0,0,1,0);
uniform float viewDistance = 2.0;

uniform vec3 normalizedLightDir = normalize(vec3(-1,-1,-5));

uniform vec3 eyeWorldPos;


in vec3 fragWorldPos;
in vec3 fragWorldNormal;
in vec3 fromEye;

out vec4 fragColor;


/* Returns height relative to the '0' level.
 * Is garanteed to be in [-amplitude/2, amplitude/2]
 */
float computeRelativeHeight (const vec4 color)
{
    return (color.a - 0.5) * amplitude;
}

float computeHeight (const vec4 color)
{
    return waterLevel + computeRelativeHeight(color);
}

/* Mixes ground texture and lights texture */
vec4  computeGroundColor (const vec2 pos)
{
    if (pos.x > 1 || pos.x < 0 || pos.y > 1 || pos.y < 0)
        return vec4(0);
    return texture(groundTexture, pos) + texture(lightsTexture, pos);
}

/* Fresnel coefficient, describes the proportion of refracted and reflected.
*/
float computeFresnel (const vec3 normalizedNormal,
                      const vec3 normalizedFromEye)
{
//    float F0 = (1 - eta) / (1 + eta);
//    F0 = F0 * F0;
    float F0 = 0.5;
    
    return mix(pow(1 - dot(normalizedNormal,-normalizedFromEye), 5), 1, F0);
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
    vec2 coordsOnHeightmap = position.xy;
    
    /* Refracted ray going from the position to the ground */
    vec3 refracted = refract(normalizedFromEye, normalizedNormal, eta);
    if (refracted.z >= 0)
        return waterColor;
    
    refracted *= -position.z / refracted.z;
    vec2 coordsOnGround= coordsOnHeightmap + refracted.xy;
    vec4 groundColor = computeGroundColor(coordsOnGround);
    
    float opacityFactor = clamp(length(refracted) / viewDistance, 0, 1);
    vec4 surfaceColor = mix(groundColor, waterColor, opacityFactor);
    
    vec4 skyColor = waterColor;
    float fresnel = computeFresnel(normalizedNormal, normalizedFromEye);
    
    return mix(surfaceColor, skyColor, fresnel);
}

void main()
{
    vec4 heightColor = texture(heightmap, fragWorldPos.xy);
    
    float waterHeight = computeHeight(heightColor);
    if (fragWorldPos.z > waterHeight) {
        discard;
    }
    
    vec3 normalizedFromEye = normalize(fragWorldPos - eyeWorldPos);
    vec3 normalizedNormal = normalize(fragWorldNormal);
    
    fragColor = computeSurfaceColor(fragWorldPos, normalizedNormal, normalizedFromEye) +
                computeSpecularity(normalizedNormal, normalizedFromEye);
}

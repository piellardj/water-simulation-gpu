#version 130

uniform sampler2D buffer;
uniform vec2 bufferSize;

uniform sampler2D tilesTexture;
uniform vec4 waterColor = vec4(0,0,1,0);
uniform float maxViewingDistance = 2.0;

out vec4 fragColor;


__UTILS__


void main()
{
    vec2 coordOnBuffer = gl_FragCoord.xy / bufferSize;
    vec2 pixelSize = 1 / bufferSize;
    
    
    /* Height and normal acquisition */
    // height: normalized, 0 = bottom, 1 = max 
    float height = vecToValue(texture(buffer, coordOnBuffer).rg, POS_RANGE);
    height = height / POS_RANGE + 0.5;

    vec3 normal;
    normal.x = vecToValue(texture(buffer, coordOnBuffer + pixelSize*vec2(1,0)).rg, POS_RANGE) -
               vecToValue(texture(buffer, coordOnBuffer + pixelSize*vec2(-1,0)).rg, POS_RANGE);
    normal.y = vecToValue(texture(buffer, coordOnBuffer + pixelSize*vec2(0,1)).rg, POS_RANGE) -
               vecToValue(texture(buffer, coordOnBuffer + pixelSize*vec2(0,-1)).rg, POS_RANGE);
    normal.z = 1;
    normal = normalize(normal);
    
    vec3 I = vec3(0,0,-1);
    vec3 refracted = refract(I, normal, 1.4);
    vec3 reflected = reflect(I, normal);
    
    
    /* Sparkles in a given direction */
    vec4 sparkleColor = vec4(1);
    vec3 sparkleDir = normalize(vec3(1,1,4));
    float sparkleFactor = dot(reflected, sparkleDir);
    vec4 sparkle = sparkleColor * smoothstep(.95, 1, sparkleFactor);
    
    
    /* Computing of surface color (refraction + water opacity)*/
    vec2 coordOnBottom = coordOnBuffer + refracted.xy * height;
    coordOnBottom *= 4;
    vec4 tilesColor = texture(tilesTexture, coordOnBottom);
    float opacity = clamp(height / maxViewingDistance, 0, 1);
    vec4 surfaceColor = mix(tilesColor, waterColor, opacity);
    
    
    fragColor = sparkle + surfaceColor;
}

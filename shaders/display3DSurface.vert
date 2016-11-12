#version 130


uniform mat4 MVP; //proj * view * model product, precomputed on CPU
uniform vec3 eyeWorldPos;

uniform sampler2D heightmap;

uniform float amplitude=0.2; //waves amplitude
uniform float waterLevel=0.5; //distance from the '0' and the ground

// expected to be in [0,1]x[0,1]
attribute vec2 coordsOnHeightmap;

out vec3 fragWorldPos;
out vec3 fromEye;


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

void main()
{
    vec4 heightColor = texture(heightmap, coordsOnHeightmap);
    
    float height = computeHeight(heightColor);
    
    fragWorldPos = vec3(coordsOnHeightmap, height);
    fromEye = fragWorldPos - eyeWorldPos;
    
    gl_Position = MVP * vec4(fragWorldPos, 1);
}

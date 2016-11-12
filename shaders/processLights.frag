#version 130


uniform sampler2D rawLightsTexture;
uniform vec2 textureSize;

uniform float shift=0;
uniform float stretch=1;

out vec4 fragColor;


/* Post-processes the rawLightsTexture and then adds it the ground texture.
 * The post-processing crops then stretches the lights range to adjust contrast.
*/
void main()
{
    vec2 coords = gl_FragCoord.xy / textureSize;
    
    vec4 lightsColor = texture(rawLightsTexture, coords);
    lightsColor = max(vec4(0), (lightsColor - shift)) * stretch;
    
    fragColor = lightsColor;
}

#version 130


uniform mat4 MVP; //proj * view * model product, precomputed on CPU

attribute vec3 vertex; // expected to be in [0,1]
attribute vec3 normal;

out vec3 fragWorldPos;
out vec3 fragWorldNormal;
out vec3 fromEye;


void main()
{
    fragWorldPos = vertex;
    fragWorldNormal = normal;
    
    gl_Position = MVP * vec4(fragWorldPos, 1);
}

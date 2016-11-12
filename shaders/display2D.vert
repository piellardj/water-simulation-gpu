#version 130


//screen corner: (-1,-1) bottom left, (1,1) top right
attribute vec2 corner;

// corresponding coordinates on heightmap
out vec2 coordsOnHeightmap;


void main()
{
    gl_Position = vec4(corner, 0, 1);
    coordsOnHeightmap = corner * 0.5 + 0.5;
}

#version 130


out vec4 fragColor;


__UTILS__


/* Initializes the cell to be still */
void main()
{
    fragColor = vec4(valueToVec(0.0, POS_RANGE),
                     valueToVec(0.0, VEL_RANGE));
}

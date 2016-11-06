/* These functions help storing float values into a texture's color channels.
   One float takes two color channels -> 256*256 = 65526 possible values.

   To take full advantage of the two channels, we make assumtpions on
   the range of the storable float values. The smaller the range is, the
   more precise the storage will be.
    - storable speed is between [-0.5;0.5] * MAX_SPEED
    - storable position is between [-0.5;0.5] * MAX_POSITION

    If a value exceeds the expected range, we clamp it.

    To store a float value, we scale it to fit in [0, 65535],
    then project it on the base 256.
*/


const float POS_RANGE = 10.0;
const float VEL_RANGE = 10.0;


/* Converts value stored in two color channels
   to float value in [0, 65535] */
float fromBase256 (const vec2 digits)
{
    return dot(vec2(255.0*256.0, 255.0), digits);
}

/* Converts float value in [0, 65535]
   to value storable in two color channels */
vec2 toBase256 (float value)
{
    value = clamp (value, 0.0, 65535.0);
    
    float strongComponent = floor(value / 256.0);
    float weakComponent = value - 256.0 * strongComponent;
    
    return vec2(strongComponent, weakComponent) / 255.0;
}


/* Gives value in [-RANGE,RANGE] */
float vecToValue (const vec2 vec, const float RANGE)
{
    /* Read normalized value then maps it */
    float value = fromBase256(vec) / 65535.0;
    return (value - 0.5) * RANGE;
}

/* Expects value in [-RANGE,RANGE] */
vec2 valueToVec (float value, const float RANGE)
{
    /* Map the value to [0,65535] then store it */
    value = 65535.0 * ((value / RANGE) + 0.5);
    return toBase256(value);
}


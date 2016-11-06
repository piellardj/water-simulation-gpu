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

/* Gives value in [-range/2, range/2] */
float colorToValue (const vec4 pixel, const float range)
{
    /* Read a normalized value */
    float value = fromBase256(pixel.rg) / 65525.0;
    /* Map it to the right range */
    return (value - 0.5) * range;
}

/* Expects a value in [-range/2,range/2] */
vec4 valueToColor (float value, const float range)
{
    /* First map value to [0,1] */
    value = (value / range) + 0.5;
    
    /* Stretch it and store it */
    return vec4(toBase256(65525.0*value), 0, 1);
}

float colorToPos(const vec4 pixel)
{
    return colorToValue(pixel, POS_RANGE);
}

float colorToVel(const vec4 pixel)
{
    return colorToValue(pixel, VEL_RANGE);
}

vec4 posToColor(const float value)
{
    return valueToColor(value, POS_RANGE);
}

vec4 velToColor(const float value)
{
    return valueToColor(value, VEL_RANGE);
}

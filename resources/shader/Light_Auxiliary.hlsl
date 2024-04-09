#ifndef NUM_MAX_LIGHT
#define NUM_MAX_LIGHT 30
#endif 

#ifndef PI
#define PI 3.14159265359
#endif 

struct Light
{
    float3 radiant;
    int type;
    float3 pos;
    float falloffEnd;
};
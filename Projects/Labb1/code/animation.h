
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#define MAGIC 'NA01'

struct Nax3Header
{
    uint magic;
    uint numClips;
    uint numKeys;
};

struct Nax3Clip
{
    ushort numCurves;
    ushort startKeyIndex;
    ushort numKeys;
    ushort keyStride;
    ushort keyDuration; 
    uchar preInfinityType;          // CoreAnimation::InfinityType::Code
    uchar postInfinityType;         // CoreAnimation::InfinityType::Code
    ushort numEvents;
    char name[50];                  // add up to 64 bytes size for Nax3Clip
};

struct Nax3AnimEvent
{
    char name[47];
    char category[15];
    ushort keyIndex;
};

struct Nax3Curve
{
    uint firstKeyIndex;
    uchar isActive;                 // 0 or 1
    uchar isStatic;                 // 0 or 1
    uchar curveType;                // CoreAnimation::CurveType::Code
    uchar _padding;                 // padding byte
    float staticKeyX;
    float staticKeyY;
    float staticKeyZ;
    float staticKeyW;
};

struct AnimationGroup
{
    Nax3Clip clip;
    Nax3AnimEvent *events;
    Nax3Curve *curves;
};

enum curveCode
{
    Translation,    //> keys in curve describe a translation
    Scale,          //> keys in curve describe a scale
    Rotation,       //> keys in curve describe a rotation quaternion
    Color,          //> keys in curve describe a color
    Velocity,       //> keys describe a linear velocition
    Float4,         //> generic 4D key
};

    
struct key
{
    float x, y, z, w;
};

class model;

class animation
{
private:
    AnimationGroup *groups;
    key *keys;
    int nClips;
    int activeGroup;
    int frame;
    float timeElapsed;
    int loopCount;
public:
    animation() { frame = 0; timeElapsed = 0.f; loopCount = 0; activeGroup = -1; };
    void parseNAX3(char*);
    key slerp(key*, key*, float);
    void startAnimation(int i, int j) { activeGroup = i; loopCount = j; };
    bool playAnimation(model*, float);
};

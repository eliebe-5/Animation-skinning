#include "Projects/Labb1/code/math.h"
#include "Projects/Labb1/code/matvec.h"
#include <vector>
#include <string>

#define MAGICNVX 'NVX2'

struct Nvx2Header
{
	uint magic;
	uint numGroups;
	uint numVertices;
	uint vertexWidth;
	uint numIndices;
	uint numEdges;
	uint vertexComponentMask;
};

struct Nvx2Group
{
	uint firstVertex;
	uint numVertices;
	uint firstTriangle;
	uint numTriangles;
	uint firstEdge;
	uint numEdges;
};

enum VertexComponent
{
    Position, Normal, TexCoord1, TexCoord2, ColorV, Tangent, Binormal, SkinWeights, SkinJIndices,
    Float2, Float3, Float4V, Short2, Byte4N, UByte4N, UByte4
};

enum N2VertexComponent
    {
        N2Coord        = (1<<0),      // 3 floats
        N2Normal       = (1<<1),      // 3 floats
        N2NormalB4N   = (1<<2),      // 4 unsigned bytes, normalized
        N2Uv0          = (1<<3),      // 2 floats
        N2Uv0S2        = (1<<4),      // 2 shorts, 4.12 fixed point
        N2Uv1          = (1<<5),      // 2 floats
        N2Uv1S2        = (1<<6),      // 2 shorts, 4.12 fixed point
        N2Uv2          = (1<<7),      // 2 floats
        N2Uv2S2        = (1<<8),      // 2 shorts, 4.12 fixed point
        N2Uv3          = (1<<9),      // 2 floats
        N2Uv3S2        = (1<<10),     // 2 shorts, 4.12 fixed point
        N2Color        = (1<<11),     // 4 floats
        N2ColorUB4N    = (1<<12),     // 4 unsigned bytes, normalized
        N2Tangent      = (1<<13),     // 3 floats
        N2TangentB4N  = (1<<14),     // 4 unsigned bytes, normalized
        N2Binormal     = (1<<15),     // 3 floats
        N2BinormalB4N = (1<<16),     // 4 unsigned bytes, normalized
        N2Weights      = (1<<17),     // 4 floats
        N2WeightsUB4N  = (1<<18),     // 4 unsigned bytes, normalized
        N2JIndices     = (1<<19),     // 4 floats
        N2JIndicesUB4  = (1<<20),     // 4 unsigned bytes

        N2NumVertexComponents = 21,
        N2AllComponents = ((1<<N2NumVertexComponents) - 1),
};

struct VertexGroup
{
    VertexComponent sem;
    VertexComponent fmt;
    uint index;
    VertexGroup(VertexComponent s, VertexComponent f, uint i) { sem = s; fmt = f; index = i;  };
};

class joint
{
public:
    matrix4D basepose;
    //m4x4 basepose;
    std::vector<int> children;
    int parent;
    std::string name;
};

class model
{
private:
    
    
public:
    joint* joints;
    joint* activeJoints;
    joint* animJoints;
    joint* builtJoints;
    std::vector<float> VB;
    int VBL;

    int* frag;
    
    int nrJoints;
    void reset() { for(int i = 0; i < nrJoints; i++){ activeJoints[i] = joints[i]; } };
    void generateTransform(int, matrix4D);
    void generateTransform2(int, matrix4D);
    //void generateTransform(int, m4x4);
    
    void generateModel(char*);
    void moveJoint(int, matrix4D);
    void moveJoint(int, float*);
    void setJointRot(int, float*);
    void setJointPos(int, float*);
    float* getJointPos(int);
    void setJointTransform(int, matrix4D);
    matrix4D getJointPose(int);
    float* getVB(){ return &VB[0]; };
    int getVBL() { return VB.size(); };
    //void moveJoint(int, m4x4);
    void drawModel();
};

class mesh
{
private:
    std::vector<float> testVB;
    std::vector<float> testUV;
    std::vector<char> testNorm;
    std::vector<char> testTangent;
    std::vector<char> testBinorm;
    std::vector<unsigned char> testWeights;
    std::vector<unsigned char> testSkinI;
    std::vector<VertexGroup> vertexComp;
public:
    void parseNvx2(char *path);
    std::vector<float> getVB() { return testVB; };
    std::vector<unsigned char> getW() { return testWeights; };
    std::vector<unsigned char> getI() { return testSkinI; };
    std::vector<float> getUV() { return testUV; };
    std::vector<char> getNorm() { return testNorm; };
    std::vector<char> getTangent() { return testTangent; };
    std::vector<char> getBinorm() { return testBinorm; };
};

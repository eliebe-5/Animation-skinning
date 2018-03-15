#include "Projects/Labb1/code/model.h"
#include <tinyxml.h>
#include <cstdio>
#include <vector>
#include <sstream>
#include <string>
#include <GL/glut.h>

void mesh::parseNvx2(char *path)
{
    FILE *f;
    char *buffer;
    int filelen;

    f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    filelen = ftell(f);
    rewind(f);

    buffer = new char[filelen];

    fread(buffer, filelen, 1, f);
    fclose(f);

    Nvx2Header *head = (Nvx2Header*) buffer;

    printf("Number of groups: %d\n", head->numGroups);

    if(head->magic != MAGICNVX)
    {
        printf("ERROR: Invalid fileformat (magic number does not match)\n");
        return;
    }

    buffer += sizeof(Nvx2Header);

    for(uint i = 0; i < head->numGroups; i++)
    {
        Nvx2Group* group = (Nvx2Group*) buffer;
        buffer += sizeof(Nvx2Group);

        printf("%d\n", head->numVertices);
        
        for(uint j = 0; j < N2NumVertexComponents; j++)
        {
            VertexComponent sem;
            VertexComponent fmt;
            uint index;

            if (head->vertexComponentMask & (1<<j))
            {
                switch (1<<j)
                {
                case N2Coord:        sem = VertexComponent::Position;     fmt = VertexComponent::Float3; break;
                case N2Normal:       sem = VertexComponent::Normal;       fmt = VertexComponent::Float3; break;
                case N2NormalB4N:    sem = VertexComponent::Normal;       fmt = VertexComponent::Byte4N; break;
                case N2Uv0:          sem = VertexComponent::TexCoord1;    fmt = VertexComponent::Float2; index = 0; break;
                case N2Uv0S2:        sem = VertexComponent::TexCoord1;    fmt = VertexComponent::Short2; index = 0; break;
                case N2Uv1:          sem = VertexComponent::TexCoord2;    fmt = VertexComponent::Float2; index = 1; break;
                case N2Uv1S2:        sem = VertexComponent::TexCoord2;    fmt = VertexComponent::Short2; index = 1; break;
                    /*
                      case N2Uv2:          sem = VertexComponent::TexCoord;     fmt = VertexComponent::Float2; index = 2; break;
                      case N2Uv2S2:        sem = VertexComponent::TexCoord;     fmt = VertexComponent::Short2; index = 2; break;
                      case N2Uv3:          sem = VertexComponent::TexCoord;     fmt = VertexComponent::Float2; index = 3; break;
                      case N2Uv3S2:        sem = VertexComponent::TexCoord;     fmt = VertexComponent::Short2; index = 3; break;
                    */
                case N2Color:        sem = VertexComponent::ColorV;        fmt = VertexComponent::Float4V; break;
                case N2ColorUB4N:    sem = VertexComponent::ColorV;        fmt = VertexComponent::UByte4N; break;
                case N2Tangent:      sem = VertexComponent::Tangent;      fmt = VertexComponent::Float3; break;
                case N2TangentB4N:   sem = VertexComponent::Tangent;      fmt = VertexComponent::Byte4N; break;
                case N2Binormal:     sem = VertexComponent::Binormal;     fmt = VertexComponent::Float3; break;
                case N2BinormalB4N:  sem = VertexComponent::Binormal;     fmt = VertexComponent::Byte4N; break;
                case N2Weights:      sem = VertexComponent::SkinWeights;  fmt = VertexComponent::Float4V; break;
                case N2WeightsUB4N:  sem = VertexComponent::SkinWeights;  fmt = VertexComponent::UByte4N; break;
                case N2JIndices:     sem = VertexComponent::SkinJIndices; fmt = VertexComponent::Float4V; break;
                case N2JIndicesUB4:  sem = VertexComponent::SkinJIndices; fmt = VertexComponent::UByte4; break;
                default:
                    printf("Invalid Nebula2 VertexComponent in Nvx2StreamReader::SetupVertexComponents");
                    sem = VertexComponent::Position;
                    fmt = VertexComponent::Float3;
                    break;
                }
                vertexComp.push_back(VertexGroup(sem, fmt, index));
            }
            
        }
    }

    for(uint i = 0; i < head->numVertices; i++)
    {
        for(uint j = 0; j < vertexComp.size(); j++)
        {
            float *a = (float *) buffer;
            short *b = (short *) buffer;
            char *c = (char *) buffer;
            unsigned char *d = (unsigned char *) buffer;
            
            switch(vertexComp[j].fmt)
            {
            case VertexComponent::Float3:
                buffer += sizeof(float)*3;
                

                if(vertexComp[j].sem == VertexComponent::Position)
                {
                    testVB.push_back(a[0]);
                    testVB.push_back(a[1]);
                    testVB.push_back(a[2]);
                }
                
                break;
            case VertexComponent::Float2:
                buffer += sizeof(float)*2;

                if(vertexComp[j].sem == VertexComponent::TexCoord1)
                {
                    testUV.push_back(a[0]);
                    testUV.push_back(a[1]);
                }
                
                break;
            case VertexComponent::Float4V:
                buffer += sizeof(float)*4;
                //printf("%f : %f : %f : %f\n", a[0], a[1], a[2], a[3]);
                break;
            case VertexComponent::Short2:
                
                buffer += sizeof(short)*2;
                //printf("%d : %d\n", b[0], b[1]);
                break;
            case VertexComponent::Byte4N:
                
                buffer += sizeof(char)*4;
                

                if(vertexComp[j].sem == VertexComponent::Normal)
                {
                    testNorm.push_back(c[0]);
                    testNorm.push_back(c[1]);
                    testNorm.push_back(c[2]);
                    testNorm.push_back(c[3]);
                }

                if(vertexComp[j].sem == VertexComponent::Tangent)
                {
                    testTangent.push_back(c[0]);
                    testTangent.push_back(c[1]);
                    testTangent.push_back(c[2]);
                    testTangent.push_back(c[3]);
                }

                if(vertexComp[j].sem == VertexComponent::Binormal)
                {
                    testBinorm.push_back(c[0]);
                    testBinorm.push_back(c[1]);
                    testBinorm.push_back(c[2]);
                    testBinorm.push_back(c[3]);
                }
                
                break;
            case VertexComponent::UByte4N:
                
                buffer += sizeof(unsigned char)*4;
                
                if(vertexComp[j].sem == VertexComponent::SkinWeights)
                {
                    testWeights.push_back(d[0]);
                    testWeights.push_back(d[1]);
                    testWeights.push_back(d[2]);
                    testWeights.push_back(d[3]);
                    //printf("%d : %d : %d : %d\n", d[0], d[1], d[2], d[3]);
                }
                break;
            case VertexComponent::UByte4:
                buffer += sizeof(unsigned char)*4;
                //printf("%d : %d : %d : %d\n", d[0], d[1], d[2], d[3]);

                if(vertexComp[j].sem == VertexComponent::SkinJIndices)
                {
                    testSkinI.push_back(d[0]);
                    testSkinI.push_back(d[1]);
                    testSkinI.push_back(d[2]);
                    testSkinI.push_back(d[3]);
                }
                
                break;
            }
        }
    }
    
    for(uint i = 0; i < head->numIndices; i++)
    {
        int *a = (int *) buffer;
        buffer += sizeof(int);
    }
    
}

void model::generateModel(char* path)
{
    
    std::vector<string> names;
    std::vector<string> parent;
    std::vector<string> position;
    std::vector<string> rotation;
    std::vector<string> scale;
    string fragment;
            
    
    TiXmlDocument doc(path);
    bool load_OK = doc.LoadFile();
    if(load_OK)
        {
            TiXmlHandle hDoc(&doc);
            TiXmlElement *pRoot, *pParm;
            pRoot = doc.FirstChildElement("Nebula3")->FirstChildElement("Model")->FirstChildElement("CharacterNodes")->FirstChildElement("CharacterNode");
            if(pRoot)
                {
            
                    pParm = pRoot->FirstChildElement("Joint");
                    while(pParm)
                        {
                            names.push_back(pParm->Attribute("name"));
                            parent.push_back(pParm->Attribute("parent"));
                            position.push_back(pParm->Attribute("position"));
                            rotation.push_back(pParm->Attribute("rotation"));
                            scale.push_back(pParm->Attribute("scale"));

                            pParm = pParm->NextSiblingElement("Joint");
                        }
                }
            pRoot = doc.FirstChildElement("Nebula3")->FirstChildElement("Model")->FirstChildElement("Skins")->FirstChildElement("Skin")->FirstChildElement("Fragment");
            if(pRoot)
                {
            
                    pParm = pRoot->FirstChildElement("Joints");
                    while(pParm)
                        {
                            fragment = pParm->GetText();
                            printf("%s\n", fragment.c_str());
                            pParm = pParm->NextSiblingElement("Joint");
                        }
                }
            
        }
    else
        printf("Error, no XML was loaded\n");

    joints = new joint[names.size()];
    nrJoints = names.size();

    std::stringstream ffrag(fragment);
    std::string splitfrag;
        
        
    std::vector<std::string> splitsfrag;
        
    while(std::getline(ffrag, splitfrag, ','))
    {
        splitsfrag.push_back(splitfrag);
    }

    frag = new int[splitsfrag.size()];
    
    for(int i = 0; i < splitsfrag.size(); i++)
    {
        frag[i] = std::stoi(splitsfrag[i]);
    }
    
    for(unsigned int i = 0; i < names.size(); i++)
        {

            joints[i].name = names[i].c_str();
            int p = std::stoi(parent[i]);
            if(p != -1)
                joints[p].children.push_back(i);
            joints[i].parent = p;
        
        
            std::stringstream fpos(position[i]);
            std::string splitpos;
        
        
            std::vector<std::string> splitspos;
        
            while(std::getline(fpos, splitpos, ','))
                {
                    splitspos.push_back(splitpos);
                }

            std::stringstream frot(rotation[i]);
            std::string splitrot;
        
        
            std::vector<std::string> splitsrot;
        
            while(std::getline(frot, splitrot, ','))
                {
                    splitsrot.push_back(splitrot);
                }

            std::stringstream fsc(scale[i]);
            std::string splitsc;
        
        
            std::vector<std::string> splitssc;
        
            while(std::getline(fsc, splitsc, ','))
                {
                    splitssc.push_back(splitsc);
                }
        
            float qx = std::stof(splitsrot[0]);
            float qy = std::stof(splitsrot[1]);
            float qz = std::stof(splitsrot[2]);
            float qw = std::stof(splitsrot[3]);
            
  
            joints[i].basepose.setVal(1, 1, (1 - 2*qy*qy - 2*qz*qz));
            joints[i].basepose.setVal(1, 2, (2*qx*qy - 2*qz*qw));
            joints[i].basepose.setVal(1, 3, (2*qx*qz + 2*qy*qw));
            joints[i].basepose.setVal(2, 1, (2*qx*qy + 2*qz*qw));
            joints[i].basepose.setVal(2, 2, (1 - 2*qx*qx - 2*qz*qz));
            joints[i].basepose.setVal(2, 3, (2*qy*qz - 2*qx*qw));
            joints[i].basepose.setVal(3, 1, (2*qx*qz - 2*qy*qw));
            joints[i].basepose.setVal(3, 2, (2*qy*qz + 2*qx*qw));
            joints[i].basepose.setVal(3, 3, (1 - 2*qx*qx - 2*qy*qy));

            joints[i].basepose.setVal(1, 4, std::stof(splitspos[0]));
            joints[i].basepose.setVal(2, 4, std::stof(splitspos[1]));
            joints[i].basepose.setVal(3, 4, std::stof(splitspos[2]));
        
            joints[i].basepose.setVal(4, 1, 0);
            joints[i].basepose.setVal(4, 2, 0);
            joints[i].basepose.setVal(4, 3, 0);
  
/*
            joints[i].basepose.m00 = (1 - 2*qy*qy - 2*qz*qz);
            joints[i].basepose.m01 = (2*qx*qy - 2*qz*qw);
            joints[i].basepose.m02 = (2*qx*qz + 2*qy*qw);
            joints[i].basepose.m10 = (2*qx*qy + 2*qz*qw);
            joints[i].basepose.m11 = (1 - 2*qx*qx - 2*qz*qz);
            joints[i].basepose.m12 = (2*qy*qz - 2*qx*qw);
            joints[i].basepose.m20 = (2*qx*qz - 2*qy*qw);
            joints[i].basepose.m21 = (2*qy*qz + 2*qx*qw);
            joints[i].basepose.m22 = (1 - 2*qx*qx - 2*qy*qy);

            joints[i].basepose.m03 = std::stof(splitspos[0])/5;
            joints[i].basepose.m13 = std::stof(splitspos[1])/5;
            joints[i].basepose.m23 = std::stof(splitspos[2])/5;
        
            joints[i].basepose.m30 = 0;
            joints[i].basepose.m31 = 0;
            joints[i].basepose.m32 = 0;
            
*/
            
            //std::stof(splitssc[2]));
        
        }
    activeJoints = new joint[names.size()];
    builtJoints = new joint[names.size()];
   
    for(uint i = 0; i < names.size(); i++)
    {
        activeJoints[i] = joints[i];
        builtJoints[i] = joints[i];
    }

    matrix4D id;
    generateTransform2(0, id);
    
}

void model::generateTransform(int index, matrix4D trans)
{

    //glPushMatrix();
    
    matrix4D parent = trans;
    //m4x4 parent = trans;
    
    if(activeJoints[index].parent != -1)
    {
        activeJoints[index].basepose = trans*activeJoints[index].basepose;
    }

    for(uint i = 0; i < activeJoints[index].children.size(); i++)
    {
        generateTransform(activeJoints[index].children[i], activeJoints[index].basepose);
    }
 
    /*matrix4D temp = activeJoints[index].basepose.makeTran();
    
    temp.setVal(4, 1, temp.getVal(4, 1)/5);
    temp.setVal(4, 2, temp.getVal(4, 2)/5);
    temp.setVal(4, 3, temp.getVal(4, 3)/5);
    
    glLoadMatrixf(temp.getMat());
    
    glutWireSphere(0.02,10,10);

    glPopMatrix();
    

    if(activeJoints[index].parent != -1 && activeJoints[index].parent != 0)
    {
  
        float startX = trans.getVal(1, 4)/5;
        float startY = trans.getVal(2, 4)/5;
        float endX = parent.getVal(1, 4)/5;
        float endY = parent.getVal(2, 4)/5;

        glBegin(GL_LINES);
        glVertex2f(startX, startY);
        glVertex2f(endX, endY);
        glEnd();
        }*/
}

void model::generateTransform2(int index, matrix4D trans)
{
    matrix4D parent = trans;
    //m4x4 parent = trans;
    
    if(builtJoints[index].parent != -1)
    {
        trans = trans*builtJoints[index].basepose;
        //trans = mult_m4x4(trans, joints[index].basepose);
    }

    for(uint i = 0; i < builtJoints[index].children.size(); i++)
    {
        //builtJoints[index].basepose.printMat();
        generateTransform2(builtJoints[index].children[i], trans);
    }

    builtJoints[index].basepose = trans;
    
}

void model::moveJoint(int index, matrix4D translation)
{
    activeJoints[index].basepose = activeJoints[index].basepose*translation;
    //joints[index].basepose = mult_m4x4(joints[index].basepose, translation);
}

void model::moveJoint(int index, float* quat)
{

    matrix4D translation;

    float qx = quat[0];
    float qy = quat[1];
    float qz = quat[2];
    float qw = quat[3];
    
    translation.setVal(1, 1, (1 - 2*qy*qy - 2*qz*qz));
    translation.setVal(1, 2, (2*qx*qy - 2*qz*qw));
    translation.setVal(1, 3, (2*qx*qz + 2*qy*qw));
    translation.setVal(2, 1, (2*qx*qy + 2*qz*qw));
    translation.setVal(2, 2, (1 - 2*qx*qx - 2*qz*qz));
    translation.setVal(2, 3, (2*qy*qz - 2*qx*qw));
    translation.setVal(3, 1, (2*qx*qz - 2*qy*qw));
    translation.setVal(3, 2, (2*qy*qz + 2*qx*qw));
    translation.setVal(3, 3, (1 - 2*qx*qx - 2*qy*qy));

    activeJoints[index].basepose = activeJoints[index].basepose*translation;
    //joints[index].basepose = mult_m4x4(joints[index].basepose, translation);
}

void model::setJointRot(int index, float* quat)
{
    matrix4D translation;

    float qx = quat[0];
    float qy = quat[1];
    float qz = quat[2];
    float qw = quat[3];
    
    activeJoints[index].basepose.setVal(1, 1, (1 - 2*qy*qy - 2*qz*qz));
    activeJoints[index].basepose.setVal(1, 2, (2*qx*qy - 2*qz*qw));
    activeJoints[index].basepose.setVal(1, 3, (2*qx*qz + 2*qy*qw));
    activeJoints[index].basepose.setVal(2, 1, (2*qx*qy + 2*qz*qw));
    activeJoints[index].basepose.setVal(2, 2, (1 - 2*qx*qx - 2*qz*qz));
    activeJoints[index].basepose.setVal(2, 3, (2*qy*qz - 2*qx*qw));
    activeJoints[index].basepose.setVal(3, 1, (2*qx*qz - 2*qy*qw));
    activeJoints[index].basepose.setVal(3, 2, (2*qy*qz + 2*qx*qw));
    activeJoints[index].basepose.setVal(3, 3, (1 - 2*qx*qx - 2*qy*qy));

}

void model::setJointPos(int index, float* quat)
{
    matrix4D translation;

    float qx = quat[0];
    float qy = quat[1];
    float qz = quat[2];

    activeJoints[index].basepose.setVal(1, 4, qx);
    activeJoints[index].basepose.setVal(2, 4, qy);
    activeJoints[index].basepose.setVal(3, 4, qz);
    
}

void model::setJointTransform(int index, matrix4D mat)
{
    activeJoints[index].basepose = mat;
}

float* model::getJointPos(int index)
{
    float *pos = new float[4];
    pos[0] = activeJoints[index].basepose.getVal(1, 4);
    pos[1] = activeJoints[index].basepose.getVal(2, 4);
    pos[2] = activeJoints[index].basepose.getVal(3, 4);
    pos[3] = activeJoints[index].basepose.getVal(4, 4);
    return pos;
}

matrix4D model::getJointPose(int index)
{
    return joints[index].basepose;
}

void model::drawModel()
{
    VB.clear();
    matrix4D id;

    matrix4D rot;

    activeJoints[14].basepose =  activeJoints[14].basepose * rot;
    joints[14].basepose =  joints[14].basepose * rot;
    
    generateTransform(0, id);
    //generateTransform(0, identity_m4x4());
}

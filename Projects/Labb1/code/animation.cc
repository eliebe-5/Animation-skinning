#include "Projects/Labb1/code/animation.h"
#include <stdio.h>
#include "Projects/Labb1/code/model.h"
#include "Projects/Labb1/code/math.h"
#include <cmath>

void animation::parseNAX3(char *path)
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
    
    Nax3Header *head = (Nax3Header*) buffer;

    if(head->magic != MAGIC)
    {
        printf("ERROR: Invalid fileformat (magic number does not match)\n");
        return;
    }

    groups = new AnimationGroup[head->numClips];

    buffer += sizeof(Nax3Header);
    nClips = head->numClips;
    for(uint i = 0; i < head->numClips; i++)
    {
        Nax3Clip *clip = (Nax3Clip*) buffer;
        buffer += sizeof(Nax3Clip);
        printf("clip name: %s\n", clip->name);
        
        groups[i].curves = new Nax3Curve[clip->numCurves];
        
        for(int j = 0; j < clip->numEvents; j++)
            buffer += sizeof(Nax3AnimEvent);
        
        for(int j = 0; j < clip->numCurves; j++)
        {
            Nax3Curve *curve = (Nax3Curve*) buffer;

            groups[i].curves[j] = *curve;
            buffer += sizeof(Nax3Curve);
        }

        groups[i].clip = *clip;
    }

    keys = (key*)buffer;

    printf("KeyStride: %d\nKeyDuration: %d\n\n", groups[0].clip.keyStride, groups[0].clip.keyDuration );
    
    for(int i = 0; i < groups[0].clip.numCurves; i++)
    {
        char* type;
        switch(groups[0].curves[i].curveType)
        {
        case(Translation):
            type = (char *)"Translation";
            break;
        case(Scale):
            type = (char *)"Scale";
            break;
        case(Rotation):
            type = (char *)"Rotation";
            break;
        case(Color):
            type = (char *)"Color";
            break;
        case(Velocity):
            type = (char *)"Velocity";
            break;
        }
    }
      
}

float dotProd(key o, key t)
{
    float res;

    res = o.x*t.x + o.y*t.y + o.z*t.z + o.w*t.w;
    
    return res;
}

key animation::slerp(key* orig, key* k, float t)
{

    key result;

    float kLen = sqrt(k->x*k->x + k->y*k->y + k->z*k->z + k->w*k->w);

    if(kLen == 0)
        return *orig;
    
    k->x = k->x/kLen;
    k->y = k->y/kLen;
    k->z = k->z/kLen;
    k->w = k->w/kLen;

    float dot = dotProd(*orig, *k);

    if(fabs(dot) > 0.999999995)
    {

        result.x = orig->x + t*(k->x - orig->x);
        result.y = orig->y + t*(k->y - orig->y);
        result.z = orig->z + t*(k->z - orig->z);
        result.w = orig->w + t*(k->w - orig->w);
        
        return result;
    }

    if (dot < 0.0f) {
        k->x = -k->x;
        k->y = -k->y;
        k->z = -k->z;
        k->w = -k->w;
        dot = -dot;
    }

    if(dot > 1)
        dot = 1;
    if(dot < -1)
        dot = -1;

    float theta_0 = acos(dot);
    float theta = theta_0*t;

    key k2;
    k2.x = k->x - orig->x*dot;
    k2.y = k->y - orig->y*dot;
    k2.z = k->z - orig->z*dot;
    k2.w = k->w - orig->w*dot;

    float k2Len = sqrt(k2.x*k2.x + k2.y*k2.y + k2.z*k2.z + k2.w*k2.w);

    k2.x = k2.x/k2Len;
    k2.y = k2.y/k2Len;
    k2.z = k2.z/k2Len;
    k2.w = k2.w/k2Len;

    result.x = orig->x*cos(theta) + k2.x*sin(theta);
    result.y = orig->y*cos(theta) + k2.y*sin(theta);
    result.z = orig->z*cos(theta) + k2.z*sin(theta);
    result.w = orig->w*cos(theta) + k2.w*sin(theta);
    
    return result;
}

key lerp(key a, key b, float time)
{
    key res;
    res.x = a.x + time*(b.x - a.x);
    res.y = a.y + time*(b.y - a.y);
    res.z = a.z + time*(b.z - a.z);
    res.w = a.w + time*(b.w - a.w);
    return res;
}

bool animation::playAnimation(model* m, float dt)
{
    
    if(activeGroup == -1)
    {
    }
    else
    {
    
        timeElapsed+=dt;
    
        if(groups[activeGroup].clip.keyDuration - timeElapsed < 0)
        {
            frame++;
            if(frame > groups[activeGroup].clip.numKeys - 1)
            {
            
                frame = 0;
                loopCount--;
            
                if(loopCount < 1 && !groups[activeGroup].clip.postInfinityType)
                {
                    timeElapsed = 0;
                    activeGroup = -1;
                    return false;
                }
                else if(loopCount < 1 && groups[activeGroup].clip.postInfinityType)
                {
                    loopCount = 100;
                
                }
            }
            timeElapsed = timeElapsed - groups[activeGroup].clip.keyDuration;
        }

        int nextFrame = frame > groups[activeGroup].clip.numKeys-2 ? 0 : frame + 1;

        for(int i = 0; i < m->nrJoints; i++)
        {
            matrix4D basepose = m->getJointPose(i);

            matrix4D jointInv = basepose.makeInv();
        
            key trans;
            key rotation;
            key scale;
        
            rotation = slerp(
                &keys[groups[activeGroup].curves[i*4 + 1].firstKeyIndex + groups[activeGroup].clip.keyStride * frame],
                &keys[groups[activeGroup].curves[i*4 + 1].firstKeyIndex + groups[activeGroup].clip.keyStride * nextFrame],
                timeElapsed/groups[activeGroup].clip.keyDuration);
        
            trans = lerp(
                keys[groups[activeGroup].curves[i*4].firstKeyIndex + groups[activeGroup].clip.keyStride * frame],
                keys[groups[activeGroup].curves[i*4].firstKeyIndex + groups[activeGroup].clip.keyStride * nextFrame],
                timeElapsed/groups[activeGroup].clip.keyDuration);
        
            scale = lerp(
                keys[groups[activeGroup].curves[i*4 + 2].firstKeyIndex + groups[activeGroup].clip.keyStride * frame],
                keys[groups[activeGroup].curves[i*4 + 2].firstKeyIndex + groups[activeGroup].clip.keyStride * nextFrame],
                timeElapsed/groups[activeGroup].clip.keyDuration);
  
            matrix4D scaling;
            matrix4D translation;
            matrix4D rot;

            matrix4D transform;

            float qx = rotation.x;
            float qy = rotation.y;
            float qz = rotation.z;
            float qw = rotation.w;
        
            scaling.setVal(1, 1, scale.x);
            scaling.setVal(2, 2, scale.y);
            scaling.setVal(3, 3, scale.z);
        
            translation.setVal(1, 4, trans.x);
            translation.setVal(2, 4, trans.y);
            translation.setVal(3, 4, trans.z);
        
            rot.setVal(1, 1, (1 - 2*qy*qy - 2*qz*qz));
            rot.setVal(1, 2, (2*qx*qy - 2*qz*qw));
            rot.setVal(1, 3, (2*qx*qz + 2*qy*qw));
            rot.setVal(2, 1, (2*qx*qy + 2*qz*qw));
            rot.setVal(2, 2, (1 - 2*qx*qx - 2*qz*qz));
            rot.setVal(2, 3, (2*qy*qz - 2*qx*qw));
            rot.setVal(3, 1, (2*qx*qz - 2*qy*qw));
            rot.setVal(3, 2, (2*qy*qz + 2*qx*qw));
            rot.setVal(3, 3, (1 - 2*qx*qx - 2*qy*qy));
        
            transform = translation * rot * scaling;
        
            m->setJointTransform(i, transform);

        
        
        }    
    }
    m->animJoints = new joint[m->nrJoints];

    for(int i = 0; i < m->nrJoints; i++)
    {
        m->animJoints[i] = m->activeJoints[i];
    }
    
    return true;
}

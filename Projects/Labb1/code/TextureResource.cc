#pragma once
#include "Projects/Labb1/code/TextureResource.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "Projects/Labb1/code/stb_image.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef unsigned int U32;
typedef unsigned char U8;

#define MakeFourCC(ch0, ch1, ch2, ch3) \
    ((U32)(U8)(ch0) | ((U32)(U8)(ch1) << 8) | \
     ((U32)(U8)(ch2) << 16) | ((U32)(U8)(ch3) << 24 ))
#define FOURCC_DXT1 (MakeFourCC('D','X','T','1'))
#define FOURCC_DX10 (MakeFourCC('D','X','1','0'))
#define FOURCC_DXT3 (MakeFourCC('D','X','T','3'))
#define FOURCC_DXT4 (MakeFourCC('D','X','T','4'))
#define FOURCC_DXT5 (MakeFourCC('D','X','T','5'))
#define FOURCC_DXT6 (MakeFourCC('D','X','T','6'))
#define FOURCC_DXT7 (MakeFourCC('D','X','T','7'))
#define FOURCC_DXT8 (MakeFourCC('D','X','T','8'))
#define FOURCC_DXT9 (MakeFourCC('D','X','T','9'))

struct DDS_PIXELFORMAT {
    unsigned int dwSize;
    unsigned int dwFlags;
    unsigned int dwFourCC;
    unsigned int dwRGBBitCount;
    unsigned int dwRBitMask;
    unsigned int dwGBitMask;
    unsigned int dwBBitMask;
    unsigned int dwABitMask;
};

struct DDSHeaderEXTD
{
//    unsigned int magic;
    unsigned int dxgiformat;
    unsigned int resourceDim;
    unsigned int miscFlag;
    unsigned int arraySize;
    unsigned int miscFlags2;
};
struct DDSHeader
{
    unsigned int magic;
    unsigned int size;
    unsigned int flags;
    unsigned int height;
    unsigned int width;
    unsigned int linearSize;
    unsigned int depth;
    unsigned int mipMapCount;
    unsigned int reserved1[11];
    DDS_PIXELFORMAT pfmt;
};

void TextureResource::generateHandle()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TextureResource::loadImage(char* path)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int x, y, n;
	unsigned char* image = stbi_load(path, &x, &y, &n, 4);

	if (image == nullptr)
		throw(std::string("Failed to load texture"));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	stbi_image_free(image);
}

void TextureResource::loadImageTwo(unsigned char* color, int x, int y)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, color);
	glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned char* TextureResource::getImage(char* path)
{
	int x, y, n;
	unsigned char* image = stbi_load(path, &x, &y, &n, 3);

	if (image == nullptr)
		throw(std::string("Failed to load texture"));
    
	return image;
}

size_t getFilesizeb(const char* filename) {
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

void TextureResource::loadDDS(char* path, GLuint program, int type)
{
    void* texPTR;
    size_t fsize = getFilesizeb(path);

    int isOpen = open(path, O_RDONLY, 0);

    texPTR = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, isOpen, 0);

    DDSHeader* head = (DDSHeader*)texPTR;
    DDSHeaderEXTD* extHead;

    texPTR += head->size + sizeof(uint);

    if (head->pfmt.dwFourCC == FOURCC_DXT5)
    {
        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &normalID);
        glBindTexture(GL_TEXTURE_2D, normalID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        tnLoc  = glGetUniformLocation(program, "normSampler");
        glUniform1i(tnLoc, 1);
        unsigned int blockSize = 16;
        unsigned int offset = 0;

        int oh = head->height;
        int ow = head->width;

        for (unsigned int level = 0; level < head->mipMapCount && (head->width || head->height); ++level)
        {
            unsigned int size = ((ow+3)/4)*((oh+3)/4)*blockSize;
            glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, ow, oh,
                                   0, size, texPTR + offset);

            offset += size;
            oh  /= 2;
            ow /= 2;
        }
    }
    
    if(head->pfmt.dwFourCC == FOURCC_DX10)
    {
        extHead = (DDSHeaderEXTD*)texPTR;
        texPTR += sizeof(DDSHeaderEXTD);

        if (type == 0) {
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            tLoc = glGetUniformLocation(program, "texSampler");
            glUniform1i(tLoc, 0);
        } else
        {
            glActiveTexture(GL_TEXTURE3);
            glGenTextures(1, &specID);
            glBindTexture(GL_TEXTURE_2D, specID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            tsLoc = glGetUniformLocation(program, "specSampler");
            glUniform1i(tsLoc, 0);
        }

        unsigned int blockSize = 16;
        unsigned int offset = 0;

        int oh = head->height;
        int ow = head->width;

        for(int i = 0; i < head->mipMapCount && (head->width || head->height); i++)
        {
            unsigned int size = ((ow+3)/4)*((oh+3)/4)*blockSize;
            glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, ow, oh,
                                   0, size, texPTR + offset);

            offset += size;
            oh /= 2;
            ow /= 2;
        }
    }
}

void TextureResource::preRender()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glGenerateMipmap(GL_TEXTURE_2D);
	glGenerateTextureMipmap(texture);
}


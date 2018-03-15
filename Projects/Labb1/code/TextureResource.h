#pragma once

#include "engine/core/app.h"
#include <string>
#include "engine/config.h"

using namespace std;

class TextureResource
{
private:
	GLuint texture;
    GLuint normalID;
    GLuint specID;
    GLint tnLoc;
    GLint tLoc;
    GLint tsLoc;
public:
	TextureResource() {};
	~TextureResource() {};
	void generateHandle();
	void loadImage(char *);
	void loadImageTwo(unsigned char *, int, int);
	void preRender();
    void loadDDS(char*, GLuint, int);
    unsigned char* getImage(char *);
};

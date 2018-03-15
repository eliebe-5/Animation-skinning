//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "engine/config.h"
#include "Projects/Labb1/code/exampleapp.h"
#include "Projects/Labb1/code/model.h"
#include "Projects/Labb1/code/animation.h"
#include "Projects/Labb1/code/TextureResource.h"

#include <cstring>

#include <chrono>


const GLchar* vs =
    "#version 430\n"
    "layout(location=0) in vec3 pos;\n"
    "layout(location=1) in vec4 weights;\n"
    "layout(location=2) in uvec4 weightIndices;\n"
    "layout(location=3) in vec2 uv;\n"
    "layout(location=4) in vec3 norm;\n"
    "layout(location=0) out vec2 UV;\n"
    "layout(location=1) out vec3 NORM;\n"
    "uniform mat4 matrix;\n"
    "uniform mat4 joints[255];\n"
    "void main()\n"
    "{\n"
    " vec4 normWeights = weights; \n//= weights / dot(weights, vec4(1.0));\n"
    " mat4x4 joint = joints[weightIndices[0]] * normWeights[0] +\n"
    "                joints[weightIndices[1]] * normWeights[1] +\n"
    "                joints[weightIndices[2]] * normWeights[2] +\n"
    "                joints[weightIndices[3]] * normWeights[3];\n"
    "	gl_Position = matrix * joint * (vec4(pos, 1));\n"
    " UV = uv;\n"
    " NORM = norm;\n"
    "}\n";

const GLchar* ps =
    "#version 430\n"
    "layout(location=0) in vec2 uv;\n"
    "layout(location=1) in vec3 NORM;\n"
    "out vec3 Color;\n"
    "uniform sampler2D texSampler;\n"
    "void main()\n"
    "{\n"
    "	Color = texture(texSampler,uv).rgb;\n"
    "}\n";

const GLchar* vs2 =
    "#version 430\n"
    "layout(location=0) in vec3 pos;\n"
    "layout(location=1) in vec4 weights;\n"
    "layout(location=2) in uvec4 weightIndices;\n"
    "layout(location=3) in vec2 uv;\n"
    "layout(location=4) in vec4 norm;\n"
    "layout(location=5) in vec4 tangent;\n"
    "layout(location=6) in vec4 binormal;\n"
    "layout(location=0) out vec2 UV;\n"
    "layout(location=1) out vec3 NORM;\n"
    "layout(location=2) out vec3 vertPos;\n"
    "layout(location=3) out vec4 lightpos;\n"
    "layout(location=4) out vec4 speccolor;\n"
    "uniform mat4 matrix;\n"
    "uniform mat4 joints[255];\n"
    "void main()\n"
    "{\n"
    " vec4 normWeights = weights; \n//= weights / dot(weights, vec4(1.0));\n"
    " mat4x4 joint = joints[weightIndices[0]] * normWeights[0] +\n"
    "                joints[weightIndices[1]] * normWeights[1] +\n"
    "                joints[weightIndices[2]] * normWeights[2] +\n"
    "                joints[weightIndices[3]] * normWeights[3];\n"
    " vec4 vertexNormal_cameraspace = matrix * joint * normalize(norm);\n"
    " vec4 vertexTangent_cameraspace = matrix * joint * normalize(tangent);\n"
    " vec4 vertexBitangent_cameraspace = matrix * joint * normalize(binormal);\n"
    " mat3 TBN = transpose(mat3("
    "        vec3(vertexTangent_cameraspace),"
    "        vec3(vertexBitangent_cameraspace),"
    "        vec3(vertexNormal_cameraspace)));\n"
    " vec4 vp4 = matrix * joint * (vec4(pos, 1));\n"
    " vertPos = (vec3(vp4) / vp4.w);\n"
    " gl_Position = vp4;\n"
    " UV = uv;\n"
    " NORM = vec3(vertexNormal_cameraspace);\n"
    " lightpos = vec4(TBN*vec3(0, 0, -1), 1.0);\n"
    " speccolor = vec4(1, 1, 1, 1.0);\n"
    "}\n";

const GLchar* ps2 =
    "#version 430\n"
    "layout(location=0) in vec2 uv;\n"
    "layout(location=1) in vec3 NORM;\n"
    "layout(location=2) in vec3 vertPos;\n"
    "layout(location=3) in vec4 lightPos;\n"
    "layout(location=4) in vec4 specColor;\n"
    "out vec3 Color;\n"
    
    "uniform sampler2D texSampler;\n"
    "uniform sampler2D specSampler;\n"
    "uniform sampler2D normSampler;\n"

    "const vec3 diffuseColor = vec3(0.1, 0.1, 0.1);\n"
    "const float shininess = 2.0;\n"
    "const float screenGamma = 2.2;\n"
    "void main()\n"
    "{\n"
    "     vec3 normal = normalize(texture(normSampler, uv ).rgb * 2.0 - 1.0);\n"
    "    vec3 lightDir = normalize(vec3(lightPos) - vertPos);\n"
    " \n"
    "    float lambertian = max(dot(lightDir,normal), 0.0);\n"
    "    float specular = 10;\n"
    "if(lambertian > 0.0)\n"
    "    {  \n"
    "        vec3 viewDir = normalize(-vertPos);\n"
    " \n"
    "        vec3 halfDir = normalize(lightDir - viewDir);\n"
    "        float specAngle = max(dot(halfDir, normal), 0.0);\n"
    "            specular = pow(specAngle, shininess);\n"
    " \n"
    "    }\n"
    "   \n"
    "    vec3 colorLinear = lambertian * diffuseColor + specular * normalize(texture(specSampler, uv ).rgb);\n"
    "    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));\n"
    "    Color = texture(texSampler,uv).rgb * colorGammaCorrected;\n"
    "}\n";

model m;
animation a;
mesh mel;

matrix4D scale;
matrix4D rot;

TextureResource text;

GLint loc;
GLint Jloc[255];
GLint indloc;

bool activeJoints[10];

int vbsize;

struct VBStruct
{
    float v[3];
    unsigned char w[4];
    unsigned char i[4];
    float uv[2];
    char n[4];
    char t[4];
    char bn[4];
//    float w[4];
//    unsigned int i[4];
};

using namespace Display;
namespace Example
{

//------------------------------------------------------------------------------
/**
 */
ExampleApp::ExampleApp()
{

    char *p = (char *)"Projects/Labb1/Unit_Footman.constants";
    char *p2 = (char *)"Projects/Labb1/Unit_Footman.nax3";
    char *p3 = (char *)"Projects/Labb1/Unit_Footman.nvx2";
    m.generateModel(p);
    a.parseNAX3(p2);
    mel.parseNvx2(p3);
	// empty
}

//------------------------------------------------------------------------------
/**
 */
ExampleApp::~ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
 */
bool
ExampleApp::Open()
{
    
    for(int i = 0; i < 10; i++)
        activeJoints[i] = false;
    
	App::Open();
	this->window = new Display::Window;
	window->SetKeyPressFunction([this](int32 key, int32 scancode, int32 action, int32 mods)
                                {

                                    //if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
                                    //a.playAnimation(&m);
                                    if (key > 47 && key < 58 && (action == GLFW_PRESS))
                                    {
                                        a.startAnimation(key - 48, 1);
                                        //activeJoints[key - 48] = !activeJoints[key - 48];
                                    }
                                    if (key == GLFW_KEY_ESCAPE && (action == GLFW_PRESS))
                                    {
                                        this->window->Close();
                                    }
                                    if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
                                    {
                                        matrix4D trans;
                                       
                                        trans.setAsRotationZ(5);

                                        //m4x4 trans;
                                        //trans = rot_m4x4(0, 0, 0.2);
                                        
                                        for(int i = 0; i < 10; i++)
                                        {
                                            if(activeJoints[i])
                                                m.moveJoint(i, trans);
                                        }

                                    }
                                    if( key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
                                    {
                                        
                                        matrix4D trans;
                                       
                                        trans.setVal(1, 4, 0.01);
                                        trans.setVal(2, 4, 0.f);
                                        trans.setVal(3, 4, 0.f);
                                        

                                        //m4x4 trans;
                                        //trans = translation_m4x4(0.01, 0.f, 0.f);
                                        
                                        m.moveJoint(0, trans);
                                        
                                    }
                                    if( key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
                                    {
                                        
                                        matrix4D trans;
                                        
                                        trans.setVal(1, 4, -0.01);
                                        trans.setVal(2, 4, 0.f);
                                        trans.setVal(3, 4, 0.f);
                                        
                                        //m4x4 trans;
                                        //trans = translation_m4x4(-0.01, 0.f, 0.f);
                                        
                                        m.moveJoint(0, trans);
                                    }
                                    if( key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
                                    {
                                        
                                        matrix4D trans;
                                        
                                        trans.setVal(1, 4, 0.f);
                                        trans.setVal(2, 4, 0.01);
                                        trans.setVal(3, 4, 0.f);
                                        

                                        
                                        //m4x4 trans;
                                        //trans = translation_m4x4(0.f, 0.01, 0.f);

                                        m.moveJoint(0, trans);
                                    }
                                    if( key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
                                    {
                                        
                                        matrix4D trans;
                                        
                                        trans.setVal(1, 4, 0.f);
                                        trans.setVal(2, 4, -0.01);
                                        trans.setVal(3, 4, 0.f);
                                        
                                        //m4x4 trans;
                                        //trans = translation_m4x4(0.f, -0.01, 0.f);
                                        
                                        m.moveJoint(0, trans);
                                    }
                                });

    window->SetMouseMoveFunction([this](float64 ym, float64 xm)
                                 {
                                     matrix4D x;
                                     matrix4D y;
                                     x.setAsRotationX(xm);
                                     y.setAsRotationY(-ym);
                                     
                                     rot = x*y;

                                 });

	GLfloat buf[] =
        {
            -0.5f,	-0.5f,	-1,			// pos 0
            1,		0,		0,		1,	// color 0
            0,		0.5f,	-1,			// pos 1
            0,		1,		0,		1,	// color 0
            0.5f,	-0.5f,	-1,			// pos 2
            0,		0,		1,		1	// color 0
        };

	if (this->window->Open())
    {

		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// setup vertex shader
		this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLint length = std::strlen(vs2);
		glShaderSource(this->vertexShader, 1, &vs2, &length);
		glCompileShader(this->vertexShader);

		// get error log
		GLint shaderLogSize;
		glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
        {
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
        }

		// setup pixel shader
		this->pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		length = std::strlen(ps2);
		glShaderSource(this->pixelShader, 1, &ps2, &length);
		glCompileShader(this->pixelShader);

		// get error log
		shaderLogSize;
		glGetShaderiv(this->pixelShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
        {
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->pixelShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
        }

		// create a program object
		this->program = glCreateProgram();
		glAttachShader(this->program, this->vertexShader);
		glAttachShader(this->program, this->pixelShader);
		glLinkProgram(this->program);
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
        {
			GLchar* buf = new GLchar[shaderLogSize];
			glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
			printf("[PROGRAM LINK ERROR]: %s", buf);
			delete[] buf;
        }

        std::vector<float> ve = mel.getVB();
        std::vector<unsigned char> we = mel.getW();
        std::vector<unsigned char> in = mel.getI();
        std::vector<float> uve = mel.getUV();
        std::vector<char> ne = mel.getNorm();
        std::vector<char> te = mel.getTangent();
        std::vector<char> bne = mel.getBinorm();
        
        VBStruct *VBO = new VBStruct[ve.size() / 3];

        int j = 0;
        int k = 0;
        int l = 0;
        
        for(int i = 0; i < ve.size()/3; i++)
        {
            VBO[i].v[0] = ve[j];
            VBO[i].v[1] = ve[j + 1];
            VBO[i].v[2] = ve[j + 2];
            
            VBO[i].w[0] = we[k];
            VBO[i].w[1] = we[k + 1];
            VBO[i].w[2] = we[k + 2];
            VBO[i].w[3] = we[k + 3];

            VBO[i].i[0] = in[k];
            VBO[i].i[1] = in[k + 1];
            VBO[i].i[2] = in[k + 2];
            VBO[i].i[3] = in[k + 3];

            VBO[i].uv[0] = uve[l];
            VBO[i].uv[1] = uve[l + 1];

            VBO[i].n[0] = ne[k];
            VBO[i].n[1] = ne[k + 1];
            VBO[i].n[2] = ne[k + 2];
            VBO[i].n[3] = ne[k + 3];

            VBO[i].t[0] = te[k];
            VBO[i].t[1] = te[k + 1];
            VBO[i].t[2] = te[k + 2];
            VBO[i].t[3] = te[k + 3];

            VBO[i].bn[0] = bne[k];
            VBO[i].bn[1] = bne[k + 1];
            VBO[i].bn[2] = bne[k + 2];
            VBO[i].bn[3] = bne[k + 3];

            j+=3;
            k+=4;
            l+=2;
        }

        vbsize = ve.size() / 3;
        
		// setup vbo
	    glGenBuffers(1, &this->triangle);
		glBindBuffer(GL_ARRAY_BUFFER, this->triangle);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(VBStruct) * ve.size() / 3, VBO, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        loc = glGetUniformLocation(this->program, "matrix");

        for(int i = 0; i < 255; i++)
        {
            std::string var = std::string("joints[") + std::to_string(i) + std::string("]");
            Jloc[i] = glGetUniformLocation(this->program, var.c_str());
        }

        scale.setVal(1, 1, 0.2);
        scale.setVal(2, 2, 0.2);
        scale.setVal(3, 3, 0.2);

        glEnable(GL_DEPTH_TEST);

        char *p = (char *)"Projects/Labb1/Footman_Diffuse.dds";
        char *pn = (char *)"Projects/Labb1/Footman_Normal.dds";
        char *ps = (char *)"Projects/Labb1/Footman_Specular.dds";

        glUseProgram(this->program);
        text.loadDDS(p, this->program, 0);
        text.loadDDS(pn, this->program, 0);
        text.loadDDS(ps, this->program, 1);
        
		return true;
    }
	return false;
}

//------------------------------------------------------------------------------
/**
 */

void
ExampleApp::Run()
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    
    float dt = 0.01;
    while (this->window->IsOpen())
    {

        
        start = std::chrono::system_clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->window->Update();
        
        glMatrixMode(GL_MODELVIEW);

        glUseProgram(0);
        a.playAnimation(&m, dt);
        m.drawModel();
        
        glFlush();
        
        // do stuff
		glBindBuffer(GL_ARRAY_BUFFER, this->triangle);
		glUseProgram(this->program);

        glUniformMatrix4fv(loc, 1, 0, (scale*rot).getMat());
        
        for(int i = 0; i < m.nrJoints; i++)
        {
            matrix4D inv = m.builtJoints[m.frag[i]].basepose.makeInv();

            matrix4D sample = m.activeJoints[m.frag[i]].basepose;

            matrix4D tot =  sample * inv;

            glUniformMatrix4fv(Jloc[i], 1, GL_FALSE, tot.makeTran().getMat());
        }
        
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VBStruct), NULL);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VBStruct), (GLvoid*)offsetof(VBStruct, w));
        glVertexAttribIPointer(2, 4, GL_UNSIGNED_BYTE, sizeof(VBStruct), (GLvoid*)offsetof(VBStruct, i));
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VBStruct), (GLvoid*)offsetof(VBStruct, uv));
        glVertexAttribPointer(4, 4, GL_BYTE, GL_FALSE, sizeof(VBStruct), (GLvoid*)offsetof(VBStruct, n));
        glVertexAttribPointer(5, 4, GL_BYTE, GL_FALSE, sizeof(VBStruct), (GLvoid*)offsetof(VBStruct, t));
        glVertexAttribPointer(6, 4, GL_BYTE, GL_FALSE, sizeof(VBStruct), (GLvoid*)offsetof(VBStruct, bn));
        glDrawArrays(GL_TRIANGLES, 0, vbsize);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		this->window->SwapBuffers();
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        dt = elapsed_seconds.count() * 1000;

        m.reset();
    }
}

} // namespace Example

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void PrintMatrix(glm::mat4 mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			printf("%f", mat[j][i]);
			if (j < 3) printf(", ");
		}
		printf("\n");
	}
}

#pragma region GLSL错误处理函数

bool checkOpenGLError()
{
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		cout << "glError:" << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printProgramLog(int program)
{
	int len = 0;
	int chWritten = 0;
	char * log;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char *)malloc(len);
		glGetProgramInfoLog(program, len, &chWritten, log);
		cout << "ProgramInfoLog:" << log << endl;
		free(log);
	}
}

void printShaderLog(GLuint shader)
{
	int len = 0;
	int chWritten = 0;
	char * log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0)
	{
		log = (char *)malloc(len);
		glGetShaderInfoLog(shader, len, &chWritten, log);
		cout << "Shader info Log:" << log << endl;
		free(log);
	}
}
#pragma endregion

#pragma region 读取GLSL源码
string readShaderSource(const char * path)
{
	string content;
	ifstream fileStream(path, ios::in);
	string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}
#pragma endregion

#pragma region 创建渲染程序
//创建渲染程序对象的函数
GLuint createShaderProgram()
{
	//声明错误信息
	GLint isLinked;
	GLint isVertCompiled;
	GLint isFragCompiled;

	//GLSL源代码
	string strVertShader = readShaderSource("MyVertShader.glsl");
	string strFragShader = readShaderSource("MyFragShader.glsl");

	const char * vshaderSource = strVertShader.c_str();
	const char * fshaderSource = strFragShader.c_str();


	//创建空着色器对象
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	//载入着色器源码并编译
	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &isVertCompiled);
	if (isVertCompiled != 1)
	{
		cout << "Vertex Compile Failed!" << endl;
		printShaderLog(vShader);
	}
	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &isFragCompiled);
	if (isFragCompiled != 1)
	{
		cout << "Frag Compile Failed!" << endl;
		printShaderLog(fShader);
	}
	//创建渲染程序并附加着色器。然后请求GLSL编译器确保兼容性。
	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked != 1)
	{
		cout << "Link Failed" << endl;
		printProgramLog(vfProgram);
	}

	return vfProgram;
}
#pragma endregion

#pragma region 纹理读取

GLuint loadTexture(const char * texImagePath)
{
	GLuint textureID;
	textureID = SOIL_load_OGL_texture(texImagePath, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (textureID == 0) cout << "couldn't find texture file" << texImagePath << endl;
	// MipMap
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	//各向异性过滤
	if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
	{
		GLfloat anisoset = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisoset);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisoset);
	}
	//ClampOrRepeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureID;
}

#pragma endregion


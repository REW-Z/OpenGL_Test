#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Utils.hpp"
#include "ImportedModel.h"
#include "Camera.hpp"
using namespace std;

//输入
float mousePosX;
float mousePosY;
float mouseX;
float mouseY;
bool keydown_W;
bool keydown_S;
bool keydown_A;
bool keydown_D;



//声明程序和顶点
#define numVAOs 1
#define numVBOs 3
GLuint renderingProgram;//声明渲染程序对象
GLuint vao[numVAOs];
GLuint vbo[numVBOs];



//摄像机
Camera cam;


//物体模型
glm::vec3 modelPos;
ImportedModel myModel("C:\\Users\\dell-pc\\Desktop\\test.obj");

//纹理图像
GLuint myTexture;// = loadTexture("C:\\Users\\dell-pc\\Desktop\\test.jpg");//不能在这里读取



//******************dislay变量空间******************
//(不要在渲染过程display中分配)
GLuint mLoc, vLoc, mvLoc, projLoc, nLoc;
GLuint ambLoc, dirLightDirLoc, dirLightColorLoc, mDiffLoc, mSpecLoc, mGlosLoc;
int width, height;
float aspect;
glm::mat4 mMat, vMat, mvMat, pMat, invTrMat;
//**************************************************

//光源属性（环境光+方向光）
vec4 ambientColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
vec3 lightDir = vec3(1.0f, -1.0f, 1.0f);

//材质属性
vec4 materialDiff = vec4(0.5, 0.5, 0.5, 1.0);//漫反射颜色
vec4 materialSpec = vec4(0.5, 0.5, 0.5, 1.0); //高光反射颜色
float materialGloss = 100.0f;//光泽度


//UPDATE
void update()
{
	if (keydown_W)
	{
		cam.e += normalize(cam.g) * 0.1f;
	}
	if (keydown_S)
	{
		cam.e -= normalize(cam.g) * 0.1f;
	}
	if (keydown_A)
	{
		cam.e -= normalize(cam.gxt()) * 0.1f;
	}
	if (keydown_D)
	{
		cam.e += normalize(cam.gxt()) * 0.1f;
	}

	if (abs(mouseX) > 0.0001f)
	{
		printf("%f", mouseX);
		printf("\n");
	}

	mat4 rotMatX;
	mat4 rotMatY;
	rotMatX = glm::rotate(mat4(1.0f), mouseX * -0.01f, vec3(0.0f, 1.0f, 0.0f));
	cam.t = rotMatX * vec4(cam.t, 0.0f);
	cam.g = rotMatX * vec4(cam.g, 0.0f);

	rotMatY = glm::rotate(mat4(1.0f), mouseY * -0.01f, cam.gxt());
	cam.t = rotMatY * vec4(cam.t, 0.0f);
	cam.g = rotMatY * vec4(cam.g, 0.0f);


	mousePosX += mouseX;
	mousePosY += mouseY;
	mouseX = 0.0f;
	mouseY = 0.0f;
}

//按键回调
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			keydown_W = true;
			break;
		case GLFW_KEY_S:
			keydown_S = true;
			break;
		case GLFW_KEY_A:
			keydown_A = true;
			break;
		case GLFW_KEY_D:
			keydown_D = true;
			break;
		default:
			break;
		}
	}
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			keydown_W = false;
			break;
		case GLFW_KEY_S:
			keydown_S = false;
			break;
		case GLFW_KEY_A:
			keydown_A = false;
			break;
		case GLFW_KEY_D:
			keydown_D = false;
			break;
		default:
			break;
		}
	}
}
//鼠标回调
static void curse_poscallback(GLFWwindow *window, double x, double y)
{
	if (mousePosX == 0.0 && mousePosY == 0.0)
	{
		mousePosX = (float)x;
		mousePosY = (float)y;
		mouseX = 0.0f;
		mouseY = 0.0f;
	}
	
	mouseX = (float)x - mousePosX;
	mouseY = (float)y - mousePosY;
}



void setVAOVBOs()
{
	std::vector<glm::vec3> vert = myModel.getVertices();
	std::vector<glm::vec2> tex = myModel.getTextureCoords();
	std::vector<glm::vec3> norm = myModel.getNormals();


	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	for (int i = 0; i < myModel.getNumVertices(); i++)
	{
		pvalues.push_back((vert[i]).x);
		pvalues.push_back((vert[i]).y);
		pvalues.push_back((vert[i]).z);
		tvalues.push_back((tex[i]).s);
		tvalues.push_back((tex[i]).t);
		nvalues.push_back((norm[i]).x);
		nvalues.push_back((norm[i]).y);
		nvalues.push_back((norm[i]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
}

//初始化
void init(GLFWwindow * window) 
{ 
	//指定渲染程序
	renderingProgram = createShaderProgram();
	//模型和摄像机位置
	cam.e = glm::vec3(0.0f, 0.0f, 8.0f);
	modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
	//顶点信息传入
	setVAOVBOs();
	myTexture = loadTexture("C:\\Users\\dell-pc\\Desktop\\default.jpg");
	//P透视矩阵
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	cam.aspectRatio = aspect;
	cam.fov = 60.0f;
	//pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);//1.0472radians = 60degree
	pMat = cam.GetMatrixP();
}

//渲染
void display(GLFWwindow * window, double currentTime)
{
	//UPDATE
	update();

	//背景色
	glClearColor(0.0, 0.0, 0.0, 1.0);//设置清除色
	glClear(GL_COLOR_BUFFER_BIT); //GL_COLOR_BUFFER_BIT包含了渲染后像素的颜色缓冲区。

	//清除深度缓冲
	glClear(GL_DEPTH_BUFFER_BIT);

	//启用程序
	glUseProgram(renderingProgram);


	//获取一致变量位置并赋值
	mLoc = glGetUniformLocation(renderingProgram, "m_matrix");
	vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");
	//vMat = glm::translate(glm::mat4(1.0f), -cam.e);//矩阵构建
	vMat = cam.GetMatrixV();
	mMat = glm::translate(glm::mat4(1.0f), modelPos);
	mvMat = vMat * mMat;
	invTrMat = transpose(inverse(mvMat));//逆转置矩阵

	ambLoc = glGetUniformLocation(renderingProgram, "ambient");
	dirLightDirLoc = glGetUniformLocation(renderingProgram, "light.dir");
	dirLightColorLoc = glGetUniformLocation(renderingProgram, "light.color");
	mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mGlosLoc = glGetUniformLocation(renderingProgram, "material.gloss");


	glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	/*glUniform4fv(ambLoc, 1, glm::value_ptr(ambientColor));
	glUniform4fv(dirLightColorLoc, 1, glm::value_ptr(lightColor));
	glUniform3fv(dirLightDirLoc, 1, glm::value_ptr(lightDir));
	glUniform4fv(mDiffLoc, 1, glm::value_ptr(materialDiff));
	glUniform4fv(mSpecLoc, 1, glm::value_ptr(materialSpec));
	glUniform1fv(mGlosLoc, 1, &materialGloss);*/
	
	glProgramUniform4fv(renderingProgram, ambLoc, 1, glm::value_ptr(ambientColor));
	glProgramUniform4fv(renderingProgram, dirLightColorLoc, 1, glm::value_ptr(lightColor));
	glProgramUniform3fv(renderingProgram, dirLightDirLoc, 1, glm::value_ptr(lightDir));
	glProgramUniform4fv(renderingProgram, mDiffLoc, 1, glm::value_ptr(materialDiff));
	glProgramUniform4fv(renderingProgram, mSpecLoc, 1, glm::value_ptr(materialSpec));
	glProgramUniform1fv(renderingProgram, mGlosLoc, 1, &materialGloss);


	



	//传递顶点属性-顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//传递顶点属性-顶点纹理坐标
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//传递顶点属性-顶点法线
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	//激活纹理单元
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myTexture);

	//设置
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	//绘制图形
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
}


int main(void)
{
	//初始化GLFW
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	//实例化窗口(4.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//窗口选项：主版本号4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//窗口选项：次版本号3
	GLFWwindow * window = glfwCreateWindow(1280, 720, "TestWindow_Chapter2", NULL, NULL);

	//将将GLFW窗口与当前OpenGL上下文联系起来
	glfwMakeContextCurrent(window);

	//初始化GLEW
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	//设置交换缓冲区前等待的帧数为1（开启垂直同步）  ==>使用glfwSwapBuffers()交换缓冲区
	glfwSwapInterval(0);

	//设置回调
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, curse_poscallback);

	//初始化窗口
	init(window);

	//update(display) //一个简单的渲染循环。
	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());//渲染
		glfwSwapBuffers(window);//交换缓冲区(会等待Interval)
		glfwPollEvents();//处理窗口事件
	}

	//退出
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}
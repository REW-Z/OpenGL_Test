

#include <typeinfo>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include "Utils.h"
#include "Component.h"
#include "Camera.hpp"
#include "Light.hpp"
#include "Model.h"
#include "GameObject.h"
using namespace std;


//----------------------Pass----------------------
void passOne(void);
void passTwo(void);

//----------------------输入----------------------
float mousePosX;
float mousePosY;
float mouseX;
float mouseY;
bool keydown_W;
bool keydown_S;
bool keydown_A;
bool keydown_D;



//----------------------顶点数组对象和顶点缓冲区----------------------
#define numVAOs 1
#define numMaxMeshes 99
#define numVBOs 4
GLuint renderingProgram1, renderingProgram2;//声明渲染程序对象
GLuint vao[numVAOs];
GLuint vbo[numMaxMeshes][numVBOs];



//----------------------摄像机----------------------
Camera cam;

//----------------------光源属性（环境光+方向光）----------------------

vec4 ambientColor;
DirectionalLight light;

//----------------------物体和模型----------------------
vector<Mesh> meshes;

list<GameObject *> gamesObjects;

//----------------------纹理图像----------------------
GLuint tex1, tex2;
GLuint texEmpty;


//----------------------世界坐标----------------------

glm::vec3 origin(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

//----------------------dislay变量空间----------------------
//(不要在渲染过程display中分配)
GLuint mLoc, vLoc, mvLoc, projLoc, nLoc, sLoc;
GLuint ambLoc, dirLightDirLoc, dirLightColorLoc, mDiffLoc, mSpecLoc, mGlosLoc;
int width, height;
float aspect;
glm::mat4 mMat, vMat, mvMat, pMat, invTrMat;



//----------------------阴影相关----------------------
int scSizeX, scSizeY;
GLuint shadowTex, shadowBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

//----------------------材质属性----------------------

vec4 materialDiff = vec4(0.5, 0.5, 0.5, 1.0);//漫反射颜色
vec4 materialSpec = vec4(0.5, 0.5, 0.5, 1.0); //高光反射颜色
float materialGloss = 100.0f;//光泽度

//*************************************************************************************************
//*************************************************************************************************

#pragma region 场景初始化

void sceneInit()
{
	//光源
	ambientColor = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	light.dir = normalize(vec3(1.0f, -1.0f, 1.0f));
	light.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);


	//摄像机位置
	cam.e = glm::vec3(0.0f, 0.0f, 8.0f);

	//添加物体
	GameObject * obj0 = new GameObject();
	obj0->addComponent(&(meshes[0]));
	obj0->name = "Object0";
	obj0->transform->position = vec3(0.0f, 3.0f, 0.0f);
	obj0->transform->scale = vec3(0.25f, 0.25f, 0.25f);
	
	GameObject * obj1 = new GameObject();
	obj1->addComponent(&(meshes[1]));
	obj1->name = "Object1";


	gamesObjects.push_back(obj0);
	gamesObjects.push_back(obj1);

}

#pragma endregion


#pragma region UPDATE和控制

//UPDATE
void update()
{
	//光源旋转
	mat4 rotM = rotate(mat4(1.0f), 0.002f, up);
	light.dir = rotM * vec4(light.dir, 0.0f);

	//*物体旋转
	for (list<GameObject *>::iterator it = gamesObjects.begin(); it != gamesObjects.end(); it++)
	{
		vec3 rot = (*(*it)).transform->rotation;
		//(*(*it)).transform->rotation = vec3(rot.x + 0.01f, rot.y + 0.01f, rot.z);
	}

	//移动控制
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
#pragma endregion

void importMeshes()
{
	//预先读取模型和贴图数据
	Mesh mesh("C:\\Users\\dell-pc\\Desktop\\untitled.obj");
	meshes.push_back(mesh);
	Mesh mesh2("C:\\Users\\dell-pc\\Desktop\\test.obj");
	meshes.push_back(mesh2);
}

void setupVertexBuffers()
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		std::vector<int> inds = meshes[i].getIndicates();

		std::vector<glm::vec3> vert = meshes[i].getVertices();
		std::vector<glm::vec2> tex = meshes[i].getTextureCoords();
		std::vector<glm::vec3> norm = meshes[i].getNormals();

		std::vector<float> pvalues;
		std::vector<float> tvalues;
		std::vector<float> nvalues;

		for (int vi = 0; vi < meshes[i].getNumVertices(); vi++)
		{
			pvalues.push_back((vert[vi]).x);
			pvalues.push_back((vert[vi]).y);
			pvalues.push_back((vert[vi]).z);
			tvalues.push_back((tex[vi]).s);
			tvalues.push_back((tex[vi]).t);
			nvalues.push_back((norm[vi]).x);
			nvalues.push_back((norm[vi]).y);
			nvalues.push_back((norm[vi]).z);
		}

		glGenVertexArrays(1, vao);
		glBindVertexArray(vao[0]);
		glGenBuffers(numVBOs, vbo[i]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i][0]);
		glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i][1]);
		glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i][2]);
		glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[i][3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * 4, &inds[0], GL_STATIC_DRAW);

		//buffer id
		meshes[i].bufferId = i;
	}
}

void setupShadowBuffers(GLFWwindow* window)
{
	//b矩阵用于将光源空间变换到纹理贴图空间
	b = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f);
	glfwGetFramebufferSize(window, &width, &height);
	scSizeX = width;
	scSizeY = height;

	glGenFramebuffers(1, &shadowBuffer);

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
		scSizeX, scSizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// may reduce shadow border artifacts
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

//初始化
void init(GLFWwindow * window) 
{ 
	//默认纹理
	texEmpty = loadTexture("C:\\Users\\dell-pc\\Desktop\\default.jpg");
	tex1 = texEmpty; tex2 = texEmpty;
	//指定渲染程序
	renderingProgram1 = createShaderProgram("shadowVertShader.glsl", "shadowFragShader.glsl");
	renderingProgram2 = createShaderProgram("MyVertShader.glsl", "MyFragShader.glsl");
	//设置缓冲区
	importMeshes();//载入模型
	setupVertexBuffers();//顶点缓冲区
	setupShadowBuffers(window);//阴影帧缓冲区

	//P透视矩阵
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	cam.aspectRatio = aspect;
	pMat = cam.GetMatrixP();
	cam.fov = 60.0f;

	sceneInit();
	//TEST
}

//渲染
void display(GLFWwindow * window, double currentTime)
{
	//UPDATE
	update();

	//清除颜色缓存和深度缓冲
	glClearColor(0.0, 0.0, 0.0, 1.0);//设置清除色
	glClear(GL_COLOR_BUFFER_BIT); //GL_COLOR_BUFFER_BIT包含了渲染后像素的颜色缓冲区。
	glClear(GL_DEPTH_BUFFER_BIT);


	//光源空间的变换矩阵构建
	lightVmatrix = glm::lookAt(cam.e, cam.e + light.dir, up);
	lightPmatrix = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);


	//**********  PASS 1  ******************************
	//使用自定义的阴影帧缓冲区，并把阴影纹理附着在上面
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);
	

	glDrawBuffer(GL_NONE);		//关闭绘制颜色
	glEnable(GL_DEPTH_TEST);		//开启深度测试
	glEnable(GL_POLYGON_OFFSET_FILL);	// 开启深度偏移
	glPolygonOffset(2.0f, 4.0f);		//  深度偏移

	passOne();

	glDisable(GL_POLYGON_OFFSET_FILL);	// 关闭深度偏移
	//***************************************************

	//**********  PASS 2  ******************************
	//使用显示的帧缓冲区
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);			//纹理单元0-绑定阴影纹理
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	//已移到Pass中。。。
	//glActiveTexture(GL_TEXTURE1);			//纹理单元1-绑定物体漫反射贴图
	//glBindTexture(GL_TEXTURE_2D, tex1);
	//glActiveTexture(GL_TEXTURE2);			//纹理单元1-绑定物体法线贴图
	//glBindTexture(GL_TEXTURE_2D, tex2);


	glDrawBuffer(GL_FRONT);//重新开启绘制颜色

	passTwo();
	//***************************************************
}

//pass 1
void passOne(void)
{
	glUseProgram(renderingProgram1);


	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	for (list<GameObject *>::iterator it = gamesObjects.begin(); it != gamesObjects.end(); it++)
	{
		GameObject * gameObject = *it;
		if (gameObject->getComponent(enum_mesh) != nullptr)
		{
			Mesh * mesh = dynamic_cast<Mesh *>(gameObject->getComponent(enum_mesh));

			// 绘制当前模型
			mMat = gameObject->transform->getMatrixM();

			shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
			sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP");
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

			glBindBuffer(GL_ARRAY_BUFFER, vbo[mesh->bufferId][0]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);


			//glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[mesh->bufferId][3]);
			size_t partCounts = mesh->getMeshParts().size();
			for (size_t j = 0; j < partCounts; j++)
			{
				size_t lastCount = 0;
				lastCount += (j == 0 ? 0 : mesh->getMeshParts()[j - 1].inds.size());
				glDrawElements(GL_TRIANGLES, mesh->getMeshParts()[j].inds.size(), GL_UNSIGNED_INT, (void *)(4 * lastCount));
				glDrawElements(GL_TRIANGLES, mesh->getNumIndicates(), GL_UNSIGNED_INT, (void*)(4 * lastCount));
			}
		}
		

	}
}

//pass 2
void passTwo(void)
{
	glUseProgram(renderingProgram2);


	//设置
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	for (list<GameObject *>::iterator it = gamesObjects.begin(); it != gamesObjects.end(); it++)
	{
		GameObject * gameObject = *it;
		if (gameObject->getComponent(enum_mesh) != nullptr)
		{
			Mesh * mesh = dynamic_cast<Mesh *>(gameObject->getComponent(enum_mesh));

			// 绘制当前模型
			vMat = cam.GetMatrixV();
			mMat = gameObject->transform->getMatrixM();
			mvMat = vMat * mMat;
			invTrMat = transpose(inverse(mvMat));//逆转置矩阵
			shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;//b矩阵用于将光源空间变换到纹理贴图空间

			mLoc = glGetUniformLocation(renderingProgram2, "m_matrix");
			vLoc = glGetUniformLocation(renderingProgram2, "v_matrix");
			mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
			projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
			nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
			sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");


			ambLoc = glGetUniformLocation(renderingProgram2, "ambient");
			dirLightDirLoc = glGetUniformLocation(renderingProgram2, "light.dir");
			dirLightColorLoc = glGetUniformLocation(renderingProgram2, "light.color");
			mDiffLoc = glGetUniformLocation(renderingProgram2, "material.diffuse");
			mSpecLoc = glGetUniformLocation(renderingProgram2, "material.specular");
			mGlosLoc = glGetUniformLocation(renderingProgram2, "material.gloss");


			glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
			glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));
			glUniform4fv(ambLoc, 1, glm::value_ptr(ambientColor));
			glUniform4fv(dirLightColorLoc, 1, glm::value_ptr(light.color));
			glUniform3fv(dirLightDirLoc, 1, glm::value_ptr(light.dir));
			glUniform4fv(mDiffLoc, 1, glm::value_ptr(materialDiff));
			glUniform4fv(mSpecLoc, 1, glm::value_ptr(materialSpec));
			glUniform1fv(mGlosLoc, 1, &materialGloss);

			/*glProgramUniform4fv(renderingProgram, ambLoc, 1, glm::value_ptr(ambientColor));
			glProgramUniform4fv(renderingProgram, dirLightColorLoc, 1, glm::value_ptr(lightColor));
			glProgramUniform3fv(renderingProgram, dirLightDirLoc, 1, glm::value_ptr(lightDir));
			glProgramUniform4fv(renderingProgram, mDiffLoc, 1, glm::value_ptr(materialDiff));
			glProgramUniform4fv(renderingProgram, mSpecLoc, 1, glm::value_ptr(materialSpec));
			glProgramUniform1fv(renderingProgram, mGlosLoc, 1, &materialGloss);*/


			//传递顶点属性-顶点位置
			glBindBuffer(GL_ARRAY_BUFFER, vbo[mesh->bufferId][0]);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			//传递顶点属性-顶点纹理坐标
			glBindBuffer(GL_ARRAY_BUFFER, vbo[mesh->bufferId][1]);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			//传递顶点属性-顶点法线
			glBindBuffer(GL_ARRAY_BUFFER, vbo[mesh->bufferId][2]);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);



			//绘制
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[mesh->bufferId][3]);
			size_t partCounts = mesh->getMeshParts().size();
			for (size_t j = 0; j < partCounts; j++)
			{
				if (mesh->getMeshParts()[j].material.hasDiffTex)
				{
					tex1 = mesh->getMeshParts()[j].material.textureDiffuse;
				}
				else
				{
					tex1 = texEmpty;
				}
				if (mesh->getMeshParts()[j].material.hasBumpTex)
				{
					tex2 = mesh->getMeshParts()[j].material.textureBump;
				}
				else
				{
					tex2 = texEmpty;
				}

				glActiveTexture(GL_TEXTURE0);			//纹理单元0-绑定阴影纹理
				glBindTexture(GL_TEXTURE_2D, shadowTex);
				glActiveTexture(GL_TEXTURE1);			//纹理单元1-绑定物体漫反射贴图
				glBindTexture(GL_TEXTURE_2D, tex1);
				glActiveTexture(GL_TEXTURE2);			//纹理单元1-绑定物体法线贴图
				glBindTexture(GL_TEXTURE_2D, tex2);

				size_t lastCount = 0;
				lastCount += (j == 0 ? 0 : mesh->getMeshParts()[j - 1].inds.size());
				glDrawElements(GL_TRIANGLES, mesh->getMeshParts()[j].inds.size(), GL_UNSIGNED_INT, (void *)(4 * lastCount));
			}
		}
	}
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
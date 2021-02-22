#define DEBUG

#include <ctime>
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

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
//--------------------Mono-----------------------

//...


//---------------------Time----------------------
#define GAME_TIME_SCALE 0.01f

int debug_count;
int debug_count2;
clock_t realTime, simulationTime;


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
bool keydown_Q;
bool keydown_E;



//----------------------顶点数组对象和顶点缓冲区----------------------
#define numVAOs 1
#define numMaxMeshes 100
#define numVBOs 5
GLuint renderingProgramCubemap, renderingProgram1, renderingProgram2;//声明渲染程序对象
GLuint vao[numVAOs];
GLuint vbo[numMaxMeshes][numVBOs];



//----------------------摄像机----------------------
Camera cam;

//----------------------光源属性（环境光+方向光）----------------------

vec4 envionmentAmbient;
DirectionalLight light;

//----------------------物体和模型----------------------
vector<Mesh> meshes;

list<GameObject *> gamesObjects;

//----------------------纹理图像----------------------
GLuint tex1, tex2;
GLuint texSkybox;
GLuint texEmpty;


//----------------------世界坐标----------------------

glm::vec3 origin(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

//----------------------dislay变量空间----------------------
//(不要在渲染过程display中分配)
GLuint mLoc, vLoc, mvLoc, projLoc, nLoc, sLoc;
GLuint envAmbLoc, dirLightDirLoc, dirLightColorLoc, mAmbLoc, mDiffLoc, mSpecLoc, mNsLoc;
int width, height;
GLuint widthLoc, heightLoc;
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
	envionmentAmbient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	light.dir = normalize(vec3(1.0f, -1.0f, 1.0f));
	light.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);


	//摄像机位置
	cam.e = glm::vec3(0.0f, 0.0f, 8.0f);

	//添加物体
	GameObject * obj0 = new GameObject();
	obj0->addComponent(&(meshes[0]));
	obj0->name = "Object0";
	
	GameObject * obj1 = new GameObject();
	obj1->addComponent(&(meshes[1]));
	obj1->name = "Object1";

	GameObject * obj2 = new GameObject();
	obj2->addComponent(&(meshes[2]));
	obj2->name = "Object2";


	gamesObjects.push_back(obj0);
	gamesObjects.push_back(obj1);
	gamesObjects.push_back(obj2);

}

#pragma endregion

#pragma region UPDATE_FIXED_UPDATE_INPUT
//FIXED UPDATE
void fixed_update()
{

}
//UPDATE
void update()
{
	//光源旋转
	mat4 rotM = rotate(mat4(1.0f), 0.001f * GAME_TIME_SCALE , up);
	light.dir = rotM * vec4(light.dir, 0.0f);


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
	if (keydown_Q)
	{
		cam.e -= normalize(cam.t) * 0.1f;
	}
	if (keydown_E)
	{
		cam.e += normalize(cam.t) * 0.1f;
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
		case GLFW_KEY_Q:
			keydown_Q = true;
			break;
		case GLFW_KEY_E:
			keydown_E = true;
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
		case GLFW_KEY_Q:
			keydown_Q = false;
			break;
		case GLFW_KEY_E:
			keydown_E = false;
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
	Mesh mesh1("Resources\\plane.obj");
	meshes.push_back(mesh1);
	Mesh mesh2("Resources\\test.obj");
	meshes.push_back(mesh2);
	Mesh mesh3("Resources\\cubes.obj");
	meshes.push_back(mesh3);
}

void setupVertexBuffers()
{
	//天空盒网格
	float cubeVertexPositions[108] =
	{ -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo[99]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[99][0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);


	//其他网格
	for (size_t i = 0; i < meshes.size(); i++)
	{
		std::vector<int> inds = meshes[i].getIndicates();

		std::vector<glm::vec3> vert = meshes[i].getVertices();
		std::vector<glm::vec2> tex = meshes[i].getTextureCoords();
		std::vector<glm::vec3> norm = meshes[i].getNormals();
		std::vector<glm::vec3> tangent = meshes[i].getTangents();

		cout << "tanvec count:" << tangent.size() << "  normVec count: " << norm.size() << endl;


		std::vector<float> pvalues;
		std::vector<float> tvalues;
		std::vector<float> nvalues;
		std::vector<float> tanvalues;

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
			tanvalues.push_back((tangent[vi]).x);
			tanvalues.push_back((tangent[vi]).y);
			tanvalues.push_back((tangent[vi]).z);
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

		glBindBuffer(GL_ARRAY_BUFFER, vbo[i][3]);
		glBufferData(GL_ARRAY_BUFFER, tanvalues.size() * 4, &tanvalues[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[i][4]);
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
#ifdef DEBUG
	//...
#endif

	//默认纹理
	texSkybox = loadCubeMap("Resources\\cubeMap");
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);//开启无缝

	texEmpty = loadTexture("Resources\\default.jpg");
	tex1 = texEmpty; tex2 = texEmpty;
	//指定渲染程序
	renderingProgramCubemap = createShaderProgram("CubemapVertShader.glsl", "CubemapFragShader.glsl");
	renderingProgram1 = createShaderProgram("shadowVertShader.glsl", "shadowFragShader.glsl");
	renderingProgram2 = createShaderProgram("BlinnPhongVertShader.glsl", "BlinnPhongFragShader.glsl");
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
}

//渲染
void display(GLFWwindow * window, double currentTime)
{
	
	//清除颜色缓存和深度缓冲
	glClearColor(0.0, 0.0, 0.0, 1.0);//设置清除色
	glClear(GL_COLOR_BUFFER_BIT); //GL_COLOR_BUFFER_BIT包含了渲染后像素的颜色缓冲区。
	glClear(GL_DEPTH_BUFFER_BIT);

	//********** 天空盒 *********************************
	glUseProgram(renderingProgramCubemap);

	vMat = cam.GetMatrixV();

	vLoc = glGetUniformLocation(renderingProgramCubemap, "v_matrix");
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));

	projLoc = glGetUniformLocation(renderingProgramCubemap, "p_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[99][0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texSkybox);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);	// cube is CW, but we are viewing the inside
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glEnable(GL_DEPTH_TEST);
	//****************************************************
	
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
	glPolygonOffset(1.0f, 2.0f);		//  深度偏移

	passOne();

	glDisable(GL_POLYGON_OFFSET_FILL);	// 关闭深度偏移
	//***************************************************

	
	//**********  PASS 2  ******************************
	//使用显示的帧缓冲区
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);			//纹理单元0-绑定阴影纹理
	glBindTexture(GL_TEXTURE_2D, shadowTex);


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

			

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[mesh->bufferId][4]);
			
			glDrawElements(GL_TRIANGLES, mesh->getNumIndicates(), GL_UNSIGNED_INT, 0);// 渲染阴影不区分材质、部分
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


			envAmbLoc = glGetUniformLocation(renderingProgram2, "env_ambient");
			dirLightDirLoc = glGetUniformLocation(renderingProgram2, "light.dir");
			dirLightColorLoc = glGetUniformLocation(renderingProgram2, "light.color");

			widthLoc = glGetUniformLocation(renderingProgram2, "window_width");
			heightLoc = glGetUniformLocation(renderingProgram2, "window_height");
			


			glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
			glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
			glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
			glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
			glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));
			glUniform4fv(envAmbLoc, 1, glm::value_ptr(envionmentAmbient));
			glUniform4fv(dirLightColorLoc, 1, glm::value_ptr(light.color));
			glUniform3fv(dirLightDirLoc, 1, glm::value_ptr(light.dir));

			glUniform1iv(widthLoc, 1, &width);
			glUniform1iv(heightLoc, 1, &height);

			
			//事实上我们还是可以在glUseProgram之外绑定数据的——乃至直接在初始化时。这得益于glProgramUniform系列函数的引入，它比起往常的glUniform要多一个参数用来接收一个ShaderProgram的ID。
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

			//传递顶点属性-顶点切线
			glBindBuffer(GL_ARRAY_BUFFER, vbo[mesh->bufferId][3]);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(3);



			//激活环境纹理
			glActiveTexture(GL_TEXTURE1);			//纹理单元1-绑定环境纹理
			glBindTexture(GL_TEXTURE_CUBE_MAP, texSkybox);
			//绘制前绑定索引缓冲区
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[mesh->bufferId][4]);
			size_t partCounts = mesh->getMeshParts().size();
			GLuint lastCount = 0;

			for (size_t j = 0; j < partCounts; j++)
			{
				

				mAmbLoc = glGetUniformLocation(renderingProgram2, "material.ambient");
				mDiffLoc = glGetUniformLocation(renderingProgram2, "material.diffuse");
				mSpecLoc = glGetUniformLocation(renderingProgram2, "material.specular");
				mNsLoc = glGetUniformLocation(renderingProgram2, "material.Ns");
				glUniform4fv(mAmbLoc, 1, glm::value_ptr(mesh->getMeshParts()[j].material.ambient));
				glUniform4fv(mDiffLoc, 1, glm::value_ptr(mesh->getMeshParts()[j].material.diffuse));
				glUniform4fv(mSpecLoc, 1, glm::value_ptr(mesh->getMeshParts()[j].material.specular));
				glUniform1fv(mNsLoc, 1, &mesh->getMeshParts()[j].material.Ns);

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


				glActiveTexture(GL_TEXTURE2);			//纹理单元2-绑定物体漫反射贴图
				glBindTexture(GL_TEXTURE_2D, tex1);
				glActiveTexture(GL_TEXTURE3);			//纹理单元3-绑定物体法线贴图
				glBindTexture(GL_TEXTURE_2D, tex2);

				
				glDrawElements(GL_TRIANGLES, (GLuint)(mesh->getMeshParts()[j].inds.size()), GL_UNSIGNED_INT, (void *)(sizeof(GLuint) * lastCount));
				lastCount += (GLuint)(mesh->getMeshParts()[j].inds.size());
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
	GLFWwindow * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "REWGraphicsEngine", NULL, NULL);

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
		//FIXED UPDATE:固定更新
		realTime = clock();
		while (simulationTime < realTime)
		{
			fixed_update();
			simulationTime += 20l;
		}
		//UPDATE:
		update();
		//DISPLAY:
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

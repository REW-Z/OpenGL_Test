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
#include "Component.h"
using namespace std;
using namespace glm;

int Component::tid()
{
	return 0;
}

int Transform::tid()
{
	return 1;
}

Transform::Transform()
{
	position = vec3(0.0f, 0.0f, 0.0f);
	rotation = vec3(0.0f, 0.0f, 0.0f);
	scale = vec3(1.0f, 1.0f, 1.0f);
	parent = nullptr;
}

mat4 Transform::getMatrixM()
{
	mat4 mS = glm::scale(mat4(1.0f), scale);
	mat4 mRz = rotate(mat4(1.0f), (rotation.z * pi<float>()) / 180.0f, vec3(0.0f, 0.0f, 1.0f));
	mat4 mRx = rotate(mat4(1.0f), (rotation.x * pi<float>()) / 180.0f, vec3(1.0f, 0.0f, 0.0f));
	mat4 mRy = rotate(mat4(1.0f), (rotation.y * pi<float>()) / 180.0f, vec3(0.0f, 1.0f, 0.0f));
	mat4 mT = translate(mat4(1.0f), vec3(position));

	mat4 result = (mT * mRy * mRx * mRz * mS);
	if (parent != nullptr)
	{
		return (parent->getMatrixM()) * result;
	}
	else
	{
		return result;
	}
}



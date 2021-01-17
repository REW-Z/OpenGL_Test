#pragma once
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
using namespace glm;

class Component
{

};

class Transform : public Component
{
public:
	vec3 position;
	vec3 rotation;
	vec3 scale;
};
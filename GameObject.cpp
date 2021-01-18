#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2/SOIL2.h>
#include <cmath>
#include <vector>
#include <list>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "GameObject.h"

using namespace std;

GameObject::GameObject()
{
	Transform * t = new Transform();
	components.push_back(t);
	transform = t;
}

GameObject::~GameObject()
{
	delete transform;
}

Component * GameObject::getComponent(ComponentType type)
{
	Component * componentPtr = nullptr;
	for (list<Component *>::iterator it = components.begin(); it != components.end(); (it)++)
	{
		if ((*it)->tid() == (int)type)
		{
			componentPtr = *it;
		}
	}
	return componentPtr;
}

void GameObject::addComponent(Component * c)
{
	components.push_back(c);
}

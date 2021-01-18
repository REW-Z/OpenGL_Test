#pragma once
#include "Component.h"
#include "Model.h"

using namespace std;

class GameObject
{
private:
	list<Component *> components;

public:
	GameObject();
	~GameObject();

	const char * name;
	Transform * transform;

	Component * getComponent(ComponentType type);
	void addComponent(Component * c);
};


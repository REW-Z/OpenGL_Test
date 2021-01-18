#pragma once

using namespace glm;

enum ComponentType
{
	enum_component = 0,
	enum_transform = 1,
	enum_mesh = 2
};

class Component
{
public:
	virtual int tid();
};

class Transform : public Component
{
public:
	int tid();
	Transform();

	Transform * parent;

	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 getMatrixM();
};

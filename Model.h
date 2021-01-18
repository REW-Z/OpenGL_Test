#pragma once
#include <vector>


//-----------------------class define-----------------------

class Material
{
public:
	std::string name;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 emission;
	float gloss;

	bool hasDiffTex;
	bool hasBumpTex;
	GLuint textureDiffuse;
	GLuint textureBump;
};

class MeshPart
{
public:
	Material material;
	std::vector<int> inds;
};


// -------------------------class define-----------------------

class IndexlessModelImporter
{
private:
	std::vector<float> vertVals;
	std::vector<float> stVals;
	std::vector<float> normVals;

	std::vector<float> triangleVerts;
	std::vector<float> textureCoords;
	std::vector<float> normals;
public:
	IndexlessModelImporter();
	void parseOBJ(const char *filePath);
	int getNumVertices();
	std::vector<float> getVertices();
	std::vector<float> getTextureCoordinates();
	std::vector<float> getNormals();
};

class ModelImporter
{
private:
	std::vector<float> vertVals;
	std::vector<float> stVals;
	std::vector<float> normVals;

	std::vector<float> triangleVerts;
	std::vector<float> textureCoords;
	std::vector<float> normals;
	
	std::vector<int> inds;
	std::vector<MeshPart> meshParts;
public:
	ModelImporter();
	void parseOBJ(const char *filePath);
	void parseMTL(const char *filePath);
	int getNumVertices();
	std::vector<float> getVertices();
	std::vector<float> getTextureCoordinates();
	std::vector<float> getNormals();
	std::vector<int> getInds();
	std::vector<MeshPart> getMeshParts();
};




//-----------------------class define-----------------------
class IndexlessMesh: public Component
{
protected:
	int numVertices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normalVecs;
public:
	int tid();
	IndexlessMesh();
	IndexlessMesh(const char *filePath);

	int bufferId;

	int getNumVertices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTextureCoords();
	std::vector<glm::vec3> getNormals();
};

class Mesh: public Component
{
protected:
	int numVertices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normalVecs;
	std::vector<int> inds;
	std::vector<MeshPart> meshParts;
public:
	int tid();
	Mesh();
	Mesh(const char *filePath);

	int bufferId;

	int getNumVertices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTextureCoords();
	std::vector<glm::vec3> getNormals();
	std::vector<int> getIndicates();
	int getNumIndicates();
	std::vector<MeshPart> getMeshParts();
};






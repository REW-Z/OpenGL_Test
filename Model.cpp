#pragma once

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
#include <fstream>
#include <sstream>
#include <vector>
#include "Model.h"
#include "Utils.h"
using namespace std;
using namespace glm;

//---------------------------------------------------------------------

IndexlessModelImporter::IndexlessModelImporter() {}

void IndexlessModelImporter::parseOBJ(const char *filePath)
{
	float x, y, z;
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0)
		{
			stringstream ss(line.erase(0, 1));
			ss >> x; ss >> y; ss >> z;
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			stVals.push_back(x);
			stVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		if (line.compare(0, 2, "f ") == 0)
		{
			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++)
			{
				getline(ss, oneCorner, ' ');
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');

				int vertRef = 0;
				int tcRef = 0;
				int normRef = 0;

				if (v != "") vertRef = (stoi(v) - 1) * 3;
				if (t != "") tcRef = (stoi(t) - 1) * 2;
				if (n != "") normRef = (stoi(n) - 1) * 3;

				triangleVerts.push_back(vertVals[vertRef]);
				triangleVerts.push_back(vertVals[vertRef + 1]);
				triangleVerts.push_back(vertVals[vertRef + 2]);

				textureCoords.push_back(stVals[tcRef]);
				textureCoords.push_back(stVals[tcRef + 1]);

				normals.push_back(normVals[normRef]);
				normals.push_back(normVals[normRef + 1]);
				normals.push_back(normVals[normRef + 2]);
			}
		}
	}
	fileStream.close();
}
int IndexlessModelImporter::getNumVertices() { return (triangleVerts.size() / 3); }
std::vector<float> IndexlessModelImporter::getVertices() { return triangleVerts; }
std::vector<float> IndexlessModelImporter::getTextureCoordinates() { return textureCoords; }
std::vector<float> IndexlessModelImporter::getNormals() { return normals; }


//---------------------------------------------------------------------
ModelImporter::ModelImporter() { }

void ModelImporter::parseOBJ(const char *filePath)
{
	float x, y, z;
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	vector<ivec4> v_t_n;

	int indexPart = -1;
	vector<int> * pointer = nullptr;

	while (!fileStream.eof())
	{
		getline(fileStream, line);
		if (line.compare(0, 2, "v ") == 0)
		{
			stringstream ss(line.erase(0, 1));
			ss >> x; ss >> y; ss >> z;
			vertVals.push_back(x);
			vertVals.push_back(y);
			vertVals.push_back(z);
		}
		if (line.compare(0, 2, "vt") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y;
			stVals.push_back(x);
			stVals.push_back(y);
		}
		if (line.compare(0, 2, "vn") == 0)
		{
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			normVals.push_back(x);
			normVals.push_back(y);
			normVals.push_back(z);
		}
		if (line.compare(0, 7, "usemtl ") == 0)
		{
			MeshPart part;
			string name = line.erase(0, 7);
			Material mat = Material();
			mat.hasDiffTex = false;
			mat.hasBumpTex = false;
			mat.name = name;
			part.material = mat;
			meshParts.push_back(part); indexPart++;
			pointer = &meshParts[indexPart].inds;
		}
		if (line.compare(0, 2, "f ") == 0)
		{
			string oneCorner, v, t, n;
			stringstream ss(line.erase(0, 2));
			for (int i = 0; i < 3; i++)
			{
				getline(ss, oneCorner, ' ');
				stringstream oneCornerSS(oneCorner);
				getline(oneCornerSS, v, '/');
				getline(oneCornerSS, t, '/');
				getline(oneCornerSS, n, '/');

				int vertInd = 0;
				int tcInd = 0;
				int normInd = 0;

				if (v != "") vertInd = stoi(v);
				if (t != "") tcInd = stoi(t);
				if (n != "") normInd = stoi(n);

				//判断
				bool isExist = false;
				int ind = 0;
				for (size_t i = 0; i < v_t_n.size(); i++)
				{
					if (v_t_n[i].x != vertInd)
					{
						break;
					}
					if (v_t_n[i].y != tcInd)
					{
						break;
					}
					if (v_t_n[i].z == normInd)
					{
						isExist = true;
						ind = v_t_n[i].w;
					}
				}
				//根据判断结果
				if (!isExist)
				{
					triangleVerts.push_back(vertVals[(vertInd -1) * 3]);
					triangleVerts.push_back(vertVals[(vertInd - 1) * 3 + 1]);
					triangleVerts.push_back(vertVals[(vertInd - 1) * 3 + 2]);

					textureCoords.push_back(stVals[(tcInd - 1) * 2]);
					textureCoords.push_back(stVals[(tcInd - 1) * 2 + 1]);

					normals.push_back(normVals[(normInd - 1) * 3]);
					normals.push_back(normVals[(normInd - 1) * 3 + 1]);
					normals.push_back(normVals[(normInd - 1) * 3 + 2]);

					int lastIndex = v_t_n.size() - 1;
					v_t_n.push_back(ivec4(vertInd, tcInd, normInd, lastIndex + 1));

					inds.push_back(lastIndex + 1);
					(*pointer).push_back(lastIndex + 1);
				}
				else
				{
					inds.push_back(ind);
					(*pointer).push_back(ind);
				}
			}
		}
	}
	fileStream.close();
}
void ModelImporter::parseMTL(const char *filePath)
{
	string objPath = filePath;
	string mtlPath = objPath.substr(0, objPath.rfind(".")) + ".mtl";
	string dir = objPath.substr(0, objPath.rfind("\\"));

	ifstream fileStream(mtlPath, ios::in);
	string line = "";
	MeshPart * currentPart = nullptr;
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		if (line.compare(0, 7, "newmtl ") == 0)
		{
			string name = line.erase(0, 7);
			for (size_t i = 0; i < meshParts.size(); i++)
			{
				if (meshParts[i].material.name == name)
				{
					currentPart = &meshParts[i];
				}
			}
		}
		if (line.compare(0, 2, "Ka") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 3));
			ss >> x;
			ss >> y;
			ss >> z;
			(*currentPart).material.ambient = vec3(x, y, z);
		}
		if (line.compare(0, 2, "Kd") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			(*currentPart).material.diffuse = vec3(x, y, z);
		}
		if (line.compare(0, 2, "Ks") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			(*currentPart).material.specular = vec3(x, y, z);
		}
		if (line.compare(0, 2, "Ke") == 0)
		{
			float x, y, z;
			stringstream ss(line.erase(0, 2));
			ss >> x; ss >> y; ss >> z;
			(*currentPart).material.emission = vec3(x, y, z);
		}
		if (line.compare(0, 2, "Ns") == 0)
		{
			float Ns = stof(line.erase(0, 2));
			(*currentPart).material.gloss = (Ns / 1000.0f);
		}
		if (line.compare(0, 2, "Ni") == 0)
		{

		}
		if (line.compare(0, 2, "d ") == 0)
		{

		}
		if (line.compare(0, 6, "map_Kd") == 0)
		{
			string texpath = dir + "\\" + line.erase(0, 7);
			currentPart->material.textureDiffuse = loadTexture(texpath.c_str());
			currentPart->material.hasDiffTex = true;
		}
		if (line.compare(0, 8, "map_Bump") == 0)
		{
			string texpath = dir + "\\" + line.erase(0, 9);
			currentPart->material.textureBump = loadTexture(texpath.c_str());
			currentPart->material.hasBumpTex = true;
		}
	}
	fileStream.close();
}
int ModelImporter::getNumVertices() { return (triangleVerts.size() / 3); }
std::vector<float> ModelImporter::getVertices() { return triangleVerts; }
std::vector<float> ModelImporter::getTextureCoordinates() { return textureCoords; }
std::vector<float> ModelImporter::getNormals() { return normals; }
std::vector<int> ModelImporter::getInds() { return inds; }
std::vector<MeshPart> ModelImporter::getMeshParts() { return meshParts; }



//---------------------------------------------------------------------



IndexlessMesh::IndexlessMesh() {}

IndexlessMesh::IndexlessMesh(const char *filePath)
{
	IndexlessModelImporter modelImporter = IndexlessModelImporter();
	modelImporter.parseOBJ(filePath);
	numVertices = modelImporter.getNumVertices();
	std::vector<float> verts = modelImporter.getVertices();
	std::vector<float> tcs = modelImporter.getTextureCoordinates();
	std::vector<float> normals = modelImporter.getNormals();

	for (int i = 0; i < numVertices; i++)
	{
		vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		normalVecs.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
	}
}

int IndexlessMesh::getNumVertices() { return numVertices; }
std::vector<glm::vec3> IndexlessMesh::getVertices() { return vertices; }
std::vector<glm::vec2> IndexlessMesh::getTextureCoords() { return texCoords; }
std::vector<glm::vec3> IndexlessMesh::getNormals() { return normalVecs; }

//------------------------------------------------------------------------------

Mesh::Mesh() {}

Mesh::Mesh(const char * filePath)
{
	ModelImporter modelImporter = ModelImporter();

	modelImporter.parseOBJ(filePath);
	modelImporter.parseMTL(filePath);

	numVertices = modelImporter.getNumVertices();
	std::vector<float> verts = modelImporter.getVertices();
	std::vector<float> tcs = modelImporter.getTextureCoordinates();
	std::vector<float> normals = modelImporter.getNormals();
	
	for (int i = 0; i < numVertices; i++)
	{
		vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		normalVecs.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
	}
	inds = modelImporter.getInds();
	meshParts = modelImporter.getMeshParts();
}

int Mesh::getNumVertices() { return numVertices; }
std::vector<glm::vec3> Mesh::getVertices() { return vertices; }
std::vector<glm::vec2> Mesh::getTextureCoords() { return texCoords; }
std::vector<glm::vec3> Mesh::getNormals() { return normalVecs; }
std::vector<int> Mesh::getIndicates() { return inds; }
int Mesh::getNumIndicates() { return inds.size(); }
std::vector<MeshPart> Mesh::getMeshParts() { return meshParts; }
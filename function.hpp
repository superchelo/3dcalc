#pragma once
#include <string>
class Function {
private:

	int n;
	GLdouble* vertexArray;
	int* indiceArray;
	GLdouble* trigNormals;
	GLdouble* vertexNormals;
	int* vertexTrigIndex;
	GLfloat* colorArray;
	GLuint vertexID;
	GLuint indiceID;
	int vertSize;
	int trigCount;
	std::string func;
	bool genBuf;
public:
	Function();
	~Function();
	void buildVertexArray();
	void buildIndiceArray();
	void buildNormalArray();
	void initVBO();
	void draw();
	void getFunc(std::string f);
	void buildVertexNormals();
	void deleteVBO();
};
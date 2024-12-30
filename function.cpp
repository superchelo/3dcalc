#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string.h>
#include "function.hpp"
#include <exprtk.hpp>
#include <iostream>
Function::Function() {
	n = 100;
	vertSize = n * n;
	trigCount = 2*(n-1)*(n-1);
	vertexArray = new GLdouble[vertSize * 3];
	indiceArray = new int[trigCount * 3];
	trigNormals = new GLdouble[trigCount * 3];
	vertexNormals = new GLdouble[vertSize * 3];
	vertexTrigIndex = new int[trigCount * 6];
	colorArray = new GLfloat[vertSize * 3];
	std::fill_n(vertexTrigIndex, trigCount * 6, -1);
	vertexID = 0;
	indiceID = 0;
	genBuf = false;
}
Function::~Function() {
	delete[] vertexArray;
	delete[] indiceArray;
	delete[] trigNormals;
	delete[] vertexNormals;
	delete[] vertexTrigIndex;
	delete[] colorArray;
	glDeleteBuffers(1, &vertexID);
	glDeleteBuffers(1, &indiceID);
}
void Function::buildVertexArray() {
	typedef exprtk::symbol_table<GLdouble> symbol_table_t;
	typedef exprtk::expression<GLdouble>   expression_t;
	typedef exprtk::parser<GLdouble>       parser_t;

	GLdouble pi = GLdouble(3.141592653589793238462643383279502);
	GLdouble e = GLdouble(2.7182818284590452353602874713527);

	GLdouble x = -5;
	GLdouble y;
	GLdouble z = -5;
	GLdouble delta = 10 / (GLdouble)n;

	symbol_table_t symbol_table;
	symbol_table.add_variable("x", x);
	symbol_table.add_variable("z", z);
	symbol_table.add_constant("pi", pi);
	symbol_table.add_constant("e", e);
	expression_t expression;
	expression.register_symbol_table(symbol_table);

	parser_t parser;
	parser.compile(func, expression);
	vertexArray = new GLdouble[vertSize * 3];
	indiceArray = new int[trigCount * 3];
	trigNormals = new GLdouble[trigCount * 3];
	vertexNormals = new GLdouble[vertSize * 3];
	vertexTrigIndex = new int[trigCount * 6];
	colorArray = new GLfloat[vertSize * 3];
	std::fill_n(vertexTrigIndex, trigCount * 6, -1);


	int i = 0;
	for (x = -5; x < 4.9; x += delta) {
		for (z = -5; z < 4.9; z += delta) {
			y = expression.value();
			vertexArray[i] = x;
			vertexArray[i+1] = y;
			vertexArray[i+2] = z;
			
			colorArray[i] = 1.0;
			colorArray[i + 1] = 0.5;
			colorArray[i + 2] = 0.0;
			i += 3;
		}
	}
}
void Function::buildIndiceArray() {

	int i = 0;
	for(int x = 0; x < n-1; x++){
		for (int z = 0; z < n-1; z++) {

			indiceArray[i] = z + x * n;
			indiceArray[i + 1] = 1 + z + (1 + x) * n;
			indiceArray[i + 2] = z + (1 + x) * n;

			indiceArray[i + 3] = z + x * n;
			indiceArray[i + 4] = 1 + z + x * n;
			indiceArray[i + 5] = 1 + z + (1 + x) * n;

			i += 6;
		}
	}
	for (int i = 0; i < trigCount; i++) {
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 6; k++) {
				if (vertexTrigIndex[6*indiceArray[3*i+j]+k] == -1) {
					vertexTrigIndex[6*indiceArray[3 * i + j] + k] = i;
					break;
				}
			}
		}
	}
}
void Function::buildNormalArray() {
	for (int i = 0; i < trigCount; i++) {

		GLdouble x1 = vertexArray[3 * indiceArray[3*i+1]] - vertexArray[3 * indiceArray[3*i]];
		GLdouble y1 = vertexArray[3 * indiceArray[3*i+1]+1] - vertexArray[3 * indiceArray[3*i]+1];
		GLdouble z1 = vertexArray[3 * indiceArray[3*i+1]+2] - vertexArray[3 * indiceArray[3*i]+2];

		GLdouble x2 = vertexArray[3 * indiceArray[3*i+2]] - vertexArray[3 * indiceArray[3*i]];
		GLdouble y2 = vertexArray[3 * indiceArray[3*i+2]+1] - vertexArray[3 * indiceArray[3*i]+1];
		GLdouble z2 = vertexArray[3 * indiceArray[3*i+2]+2] - vertexArray[3 * indiceArray[3*i]+2];

		GLdouble cx = y1 * z2 - z1 * y2;
		GLdouble cy = z1 * x2 - x1 * z2;
		GLdouble cz = x1 * y2 - y1 * x2;

		GLdouble nx = cx / sqrt(cx * cx + cy * cy + cz * cz);
		GLdouble ny= cy / sqrt(cx * cx + cy * cy + cz * cz);
		GLdouble nz = cz / sqrt(cx * cx + cy * cy + cz * cz);

		trigNormals[3*i] = nx;
		trigNormals[3*i + 1] = ny;
		trigNormals[3*i + 2] = nz;
	}
}
void Function::buildVertexNormals() {
	for (int i = 0; i < vertSize; i++) {

		GLdouble x = 0;
		GLdouble y = 0;
		GLdouble z = 0;
		
		int j = 0;
		while (vertexTrigIndex[6 * i + j] != -1 && j < 6) {
			x += trigNormals[3*vertexTrigIndex[6 * i + j]];
			y += trigNormals[3*vertexTrigIndex[6 * i + j]+1];
			z += trigNormals[3*vertexTrigIndex[6 * i + j]+2];
			j++;
		}
		GLdouble nx = x / sqrt(x * x + y * y + z * z);
		GLdouble ny = y / sqrt(x * x + y * y + z * z);
		GLdouble nz = z / sqrt(x * x + y * y + z * z);

		vertexNormals[3 * i] = nx;
		vertexNormals[3 * i + 1] = ny;
		vertexNormals[3 * i + 2] = nz;
	}
}
void Function::initVBO() {

	genBuf = true;
	buildVertexArray();
	buildIndiceArray();
	buildNormalArray();
	buildVertexNormals();

	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, 3*vertSize*sizeof(GLdouble)*2 + 3 * vertSize * sizeof(GLfloat),0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * vertSize * sizeof(GLdouble), &vertexArray[0]);                            
	glBufferSubData(GL_ARRAY_BUFFER, 3 * vertSize * sizeof(GLdouble), 3 * vertSize * sizeof(GLdouble), &vertexNormals[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * vertSize * sizeof(GLdouble)*2, 3 * vertSize * sizeof(GLfloat), &colorArray[0]);

	glGenBuffers(1, &indiceID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*trigCount*sizeof(int), &indiceArray[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Function::deleteVBO() {
	if (genBuf) {
		delete[] vertexArray;
		delete[] indiceArray;
		delete[] trigNormals;
		delete[] vertexNormals;
		delete[] vertexTrigIndex;
		delete[] colorArray;
		glDeleteBuffers(1, &vertexID);
		glDeleteBuffers(1, &indiceID);
		genBuf = false;
	}
}

void Function::draw() {
	glPushMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceID);


	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glNormalPointer(GL_DOUBLE, 0, (void*)(3 * vertSize * sizeof(GLdouble)));
	glColorPointer(3, GL_FLOAT, 0, (void*)(2 * 3 * vertSize * sizeof(GLdouble)));
	glVertexPointer(3, GL_DOUBLE, 0, 0);
	
	glDrawElements(GL_TRIANGLES, trigCount * 3, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);            
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}
void Function::getFunc(std::string f) {
	func = f;
}

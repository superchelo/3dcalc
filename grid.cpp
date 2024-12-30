#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "grid.hpp"

Grid::Grid(float l) {
	length = l;
}
void Grid::drawGrid() {

	glPushMatrix();
	glTranslatef(0, 0, -length/2);
	glutSolidCylinder(0.05, length, 32, 32);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, length / 2, 0);
	glRotatef(90, 1, 0, 0);
	glutSolidCylinder(0.05, length, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-length / 2,0,0);
	glRotatef(90, 0, 1, 0);
	glutSolidCylinder(0.05, length, 32, 32);
	glPopMatrix();
}

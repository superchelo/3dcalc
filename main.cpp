#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "grid.hpp"
#include "function.hpp"
#include <iostream>
#include <sstream>
#include <time.h>

class Render {
private:
	GLint glutWindowWidth = 800;
	GLint glutWindowHeight = 600;
	GLdouble aspect = (GLdouble)glutWindowWidth / glutWindowHeight;

	GLfloat light_position[4] = { 4.0, 8.0, 8.0, 1.0 };
	GLfloat light_diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[4] = { 0.5, 0.5, 0.5, 1.0 };

	std::string buffer;

	GLdouble fov = 60.0;

	int currentButton = 0;
	int lastMouseX = 0;
	int lastMouseY = 0;

	float xzTheta = 0;
	float yAngle = 0;

	GLdouble eyeX = 10.0, eyeY = 4.0, eyeZ = 10;
	GLdouble radius = eyeZ;
	GLdouble zNear = 0.1, zFar = 40.0;

	Grid* grid = new Grid(10);
	Function* f = new Function();
	int initTime = time(NULL);
	int finalTime;
	int frameCount;


public:
	~Render() {
		delete grid;
		delete f;
	}
	void initOpenGL(){
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_COLOR_MATERIAL);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);

		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);
		glClearColor(0.365F, 0.69F, 1.0F, 0.0F);

		glViewport(0, 0, (GLsizei)glutWindowWidth, (GLsizei)glutWindowHeight);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, aspect, zNear, zFar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}

	void Window(GLuint w, GLuint h, int argc, char* argv[]) {
		glutInit(&argc, (char**)argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
		glutInitWindowSize(glutWindowWidth, glutWindowHeight);
		glutInitWindowPosition(50, 100);

		glutCreateWindow("3D Graphics Calculator");
		initOpenGL();
		if (glewInit() != GLEW_OK) {
			fprintf(stderr, "Failed to initialize GLEW");
		}		
	}

	void reshape3D(int w, int h){
		glutWindowWidth = (GLsizei)w;
		glutWindowHeight = (GLsizei)h;
		glViewport(0, 0, glutWindowWidth, glutWindowHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, aspect, zNear, zFar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}

	void display3D() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

		glPushMatrix();
		glRotatef(xzTheta, 0, 1, 0);
		glRotatef(yAngle, 1, 0, 0);
		drawGrid();
		glPopMatrix();
		showInfo();
		glutSwapBuffers();

		frameCount++;
		finalTime = time(NULL);
		if (finalTime - initTime > 0) {
			std::cout << "FPS: " << frameCount / (finalTime - initTime) << "\n";
			frameCount = 0;
			initTime = finalTime;
		}
	}

	void drawGrid() {
		grid->drawGrid();

		if (!buffer.empty()) {
			f->draw();
		}
		glutPostRedisplay();
	}

	void showInfo() {
		glRasterPos2f(-35, 12);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)"Press TAB to reset\nf(x,z) = ");
		glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)buffer.c_str());
	}

	void key(unsigned char key, int x, int y){
		switch (key)
		{
		case '	':
			buffer.erase();
			xzTheta = 0;
			yAngle = 0;
			break;
		// backspace
		case 8:
			if (buffer.length() > 0) {
				buffer.pop_back();
				f->deleteVBO();
				f->getFunc(buffer);
				f->initVBO();
			}
			break;
		}
		if (key != '	' && key != 8) {
			buffer.push_back((char)key);
			f->deleteVBO();
			f->getFunc(buffer);
			f->initVBO();
		}
		glutPostRedisplay();
	}

	void mouseButtonHandler3D(int button, int state, int x, int y){
		currentButton = button;
		lastMouseX = x;
		lastMouseY = y;
	}

	void mouseMotionHandler3D(int x, int y)
	{
		int dx = x - lastMouseX;
		int dy = y - lastMouseY;
		if (currentButton == GLUT_LEFT_BUTTON)
		{
			xzTheta += dx * 0.25;
		}
		if (currentButton == GLUT_RIGHT_BUTTON)
		{
			yAngle += dy * 0.5;
		}
		lastMouseX = x;
		lastMouseY = y;
		glutPostRedisplay();
	}
};
Render* render = new Render();

void display3D() {
	render->display3D();
}

void reshape3D(int w, int h) {
	render->reshape3D(w, h);
}

void mouseButtonHandler3D(int button, int state, int x, int y) {
	render->mouseButtonHandler3D(button, state, x, y);
}

void mouseMotionHandler3D(int x, int y) {
	render->mouseMotionHandler3D(x,y);
}

void key(unsigned char key, int x, int y) {
	render->key(key, x, y);
}

void initGlutCallBacks() {
	glutDisplayFunc(display3D);
	glutReshapeFunc(reshape3D);
	glutMouseFunc(mouseButtonHandler3D);
	glutMotionFunc(mouseMotionHandler3D);
	glutKeyboardFunc(key);
}


int main(int argc, char* argv[])
{
	render->Window(800, 600, argc, argv);
	initGlutCallBacks();
	glutMainLoop();

	return 0;
}

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <GL/glut.h>

#include "imageloader.h"
#include "md2model.h"

using namespace std;

const float FLOOR_TEXTURE_SIZE = 15.0f; //The size of each floor "tile"

float WIDTH = 400;
float HEIGHT = 400;

float _angleX = 30.0f;
float _angleY = 30.0f;
float pos_x, pos_y, pos_z;
float angle_x, angle_y, angle_z;


MD2Model* _model;
int _textureId;
//The forward position of the guy relative to an arbitrary floor "tile"
float _guyPos = 0;

void cleanup(){
	delete _model;
}


void processMousePassiveMotion(int x,int y){
  _angleX = (x/WIDTH)*180 + 90;
  _angleY = 22 - (y/HEIGHT)*45;
  glutPostRedisplay();
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanup();
			exit(0);
		case 'a':
			angle_x -= 1;
			break;
		case 'd':
			angle_x += 1;
			break;
	}


}


void SpecialKey (int key, int x, int y) {
    switch ( key ) {
	case GLUT_KEY_UP:
		if (_model != NULL) {
			_model->advance(0.025f);
		}
		_guyPos += 0.11f;
		while (_guyPos > FLOOR_TEXTURE_SIZE) {
			_guyPos -= FLOOR_TEXTURE_SIZE;
		}
		break;
	case GLUT_KEY_DOWN:
		if (_model != NULL) {
			_model->advance(-0.025f);
		}
		_guyPos -= 0.11f;
		while (_guyPos > FLOOR_TEXTURE_SIZE) {
			_guyPos -= FLOOR_TEXTURE_SIZE;
		}
		break;
	case GLUT_KEY_RIGHT:
		pos_x = pos_x - 0.1;
		break;
	case GLUT_KEY_LEFT:
		pos_x = pos_x + 0.1;
		break;
	}
    glutPostRedisplay();
}

//Makes the image into a texture, and returns the id of the texture
GLuint loadTexture(Image *image) {
	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 
				 0,
				 GL_RGB,
				 image->width, image->height,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 image->pixels);
	return textureId;
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	
	//Load the model
	_model = MD2Model::load("tallguy.md2");
	
	if (_model != NULL) {
		_model->setAnimation("run");
	}
	
	//Load the floor texture
	Image* image = loadBMP("road2.bmp");
	_textureId = loadTexture(image);
	delete image;
}

void handleResize(int w, int h) {
	WIDTH = w;
	HEIGHT = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -20.0f);
	glRotatef(_angleX, 0.0f, 1.0f, 0.0f);
	//glRotatef(_angleY, 1.0f, 0.0f, 0.0f);
	
	GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	
	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {-0.2f, 0.3f, -1, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	
	glColor3f(1,1,1);
	glVertex3f(0.0f, 0.0f, 1000.0f);
	glVertex3f(-1000.0f, 0.0f, -1000.0f);
	glBegin(GL_LINE);

	glEnd();
	glBegin(GL_TRIANGLES);
		glVertex3f(0.0f, 0.0f, 1000.0f);
		glVertex3f(-1000.0f, 0.0f, -1000.0f);
		glVertex3f(1000.0f, 0.0f, -1000.0f);
	glEnd();

	glBegin(GL_QUADS);
		glVertex3f(-1000.0f, 0.0f, -1000.0f);
		glVertex3f(-1000.0f, 0.0f, 1000.0f);
		glVertex3f(1000.0f, 0.0f, 1000.0f);
		glVertex3f(1000.0f, 0.0f, -1000.0f);
	glEnd();


	//Draw the guy
	if (_model != NULL){
		glPushMatrix();
		glTranslatef(pos_x, 4.5f, 0.0f);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.5f, 0.5f, 0.5f);
		_model->draw(angle_x);
		glPopMatrix();
	}
	
	//Draw the floor

	glTranslatef(-10.0f, -5.4f, 0.0f);
	//glRotatef(angle_x, 0.0f, 1.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(2000 / FLOOR_TEXTURE_SIZE, _guyPos / FLOOR_TEXTURE_SIZE);
		glVertex3f(-5000.0f, 0.0f, -5000.0f);

		glTexCoord2f(2000 / FLOOR_TEXTURE_SIZE, (2000 + _guyPos) / FLOOR_TEXTURE_SIZE);
		glVertex3f(-5000.0f, 0.0f, 5000.0f);

		glTexCoord2f(0.0f, (2000 + _guyPos) / FLOOR_TEXTURE_SIZE);
		glVertex3f(5000.0f, 0.0f, 5000.0f);

		glTexCoord2f(0.0f, _guyPos / FLOOR_TEXTURE_SIZE);
		glVertex3f(5000.0f, 0.0f, -5000.0f);
	glEnd();

	glutSwapBuffers();
}

void update(int value){
	if (_angleX > 360) {
		_angleX -= 360;
	}
	if (_angleY > 360) {
		_angleY -= 360;
	}

	
	//Advance the animation
//	if (_model != NULL) {
	//	_model->advance(0.025f);
	//}
	
	//Update _guyPos
	//_guyPos += 0.08f;
	while (_guyPos > FLOOR_TEXTURE_SIZE) {
		_guyPos -= FLOOR_TEXTURE_SIZE;
	}
	
	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow("Walking Man");
	initRendering();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialUpFunc(SpecialKey);
	glutPassiveMotionFunc(processMousePassiveMotion);
		glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}

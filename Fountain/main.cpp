/**********************************************************************

  Foutain + Water Simulation

  May, 7th, 2003

  This opengl sample was written by Philipp Crocoll
  Contact:
	philipp.crocoll@web.de
	www.codecolony.de

  Every comment would be appreciated.

  If you want to use parts of any code of mine:
	let me know and
	use it!

***********************************************************************

Keys to use
===========

  1-7    : Re-initialize scene with another shape of the fountain

  w,s	 : move camera forward/backward
  a,d    : turn camera right/left
  r,f    : move camera up/down
  x,y	 : turn camera up/down
  c,v	 : strafe left/right

  Esc	 : Exit



How does it work?
=================

The classes CCamera and COGLTexture are described in tutorials on
  www.codecolony.de/OpenGL

The files vectors.h/.cpp provide help with vector maths.

The class CPool was made from my SwimmingPool example (also on CodeColony).

The class CAirFountain was made from my Fountain Tutorial (online, too).

Changes in those classes:
 -> Fountain gets a "Pool Pointer". Each time a drop falls into the water
    (y < 0) I search for the pool's oscillator which is closest to the
	drops position. This oscillator is put down a bit (y-value is decreased).
 ->	When a fountain has many drops, a whole pool area is often put so strongly
    down that it is below the bowl and becomes invisible! This is unrealistic
	anyway, so the "AffectOscillator" method does not allow to put an oscillator
	too deep. The corresponding line of code should be replaced when using
	the pool for other purposes.
 -> The pool has a kind of damping (which is not physically correct). Otherwise
    the waves would become too strong after a while.

The method "RenderBowl" is not very interesting - it simply renders a bowl!

***********************************************************************/

#include <GL/glut.h>

#include <math.h>
#include <time.h>
#include <stdlib.h>


#include "pool.h"
#include "AirFountain.h"
#include "camera.h"  //This is my old camera, but it's easier to control
					 //for the user and the third rotation axis is not required here
#include "textures.h"


//lighting:
GLfloat LightAmbient[]=		{ 0.2f, 0.2f, 0.2f, 0.0f };
GLfloat LightDiffuse[]=		{ 0.8f, 0.8f, 0.8f, 0.0f };
GLfloat LightPosition[]=	{ 1.0f, -0.5f, -0.5f, 1.0f };


//Constants:
#define NUM_X_OSCILLATORS		170
#define NUM_Z_OSCILLATORS		170
#define OSCILLATOR_DISTANCE		0.015
#define OSCILLATOR_WEIGHT       0.0001

#define MAXX					(NUM_X_OSCILLATORS*OSCILLATOR_DISTANCE)
#define MAXZ					(NUM_Z_OSCILLATORS*OSCILLATOR_DISTANCE)

#define POOL_HEIGHT				0.3


//Camera object:
CCamera Camera;

//The "pool" which represents the water within the fountain bowl
CPool Pool;

//water outside the bowl is in the air:
CAirFountain AirFountain;

//Textures:
COGLTexture WaterTexture;  //the image does not contain a water texture,
						   //but it is applied to the water
COGLTexture RockTexture;
COGLTexture GroundTexture;




bool  g_bRain		   = true;
bool  g_bFillModePoints = true;
bool  g_bLighting		= true;


void KeyDown(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:	//ESC
		exit(0);
		break;
	case 'a':
		Camera.RotateY(5.0f);
		break;
	case 'd':
		Camera.RotateY(-5.0f);
		break;
	case 'w':
		Camera.MoveForwards(-0.15f ) ;
		break;
	case 's':
		Camera.MoveForwards( 0.15f ) ;
		break;
	case 'x':
		Camera.RotateX(5.0f);
		break;
	case 'y':
		Camera.RotateX(-5.0f);
		break;
	case 'c':
		Camera.StrafeRight(-0.05f);
		break;
	case 'v':
		Camera.StrafeRight(0.05f);
		break;
	case 'f':
		Camera.Move(F3dVector(0.0,-0.1,0.0));
		break;
	case 'r':
		Camera.Move(F3dVector(0.0,0.1,0.0));
		break;

	//*************************************
	//Several initialization calls:
	case '1':
		Pool.Reset();
		AirFountain.Delete();
		AirFountain.Initialize(3,8,35,76,90,0.5,0.11);
		break;
	case '2':
		Pool.Reset();
		AirFountain.Delete();
		AirFountain.Initialize(1,20,100,70,70,5.0,0.15);
		break;
	case '3':
		Pool.Reset();
		AirFountain.Initialize(1,20,200,85,85,10,0.1);
		break;
	case '4':
		Pool.Reset();
		AirFountain.Initialize(5,20,85,90,90,1.0,0.15);
		break;
	case '5':
		Pool.Reset();
		AirFountain.Initialize(2,20,50,40,70,1.5,0.2);
		break;
	case '6':
		Pool.Reset();
		AirFountain.Initialize(3,50,25,76,90,0.2,0.11);
		break;
	case '7':
		Pool.Reset();
		//AirFountain.Initialize(4,100,45,76,90,0.2,0.11);
		AirFountain.Initialize(5,50,65,76,90,0.5,0.11);
		break;


	}
}
GLfloat OuterRadius = 1.2;
GLfloat InnerRadius = 1.0;
GLint NumOfVerticesStone = 32;    //only a quarter of the finally used vertices
GLfloat StoneHeight = 0.5;
GLfloat WaterHeight = 0.45;
GLint ListNum;  //The number of the diplay list
struct SVertex
{
	GLfloat x,y,z;
};
float turn=0;

void RenderBowl(void)
{
    float bowlheight = 0.2 + POOL_HEIGHT;
	float bowlwidth = 0.2;

	float TexBorderDistance = bowlwidth / (MAXX+2*bowlwidth);
	GroundTexture.SetActive();

	glBegin(GL_QUADS);

		float minX = -4.0;
		float minZ = -4.0;
		float maxX = 8.0;
		float maxZ = 8.0;

		//******************
		//ground
		//******************
		glNormal3f(0.0f,1.0f,0.0);
		glTexCoord2f(0.0,0.0);
		glVertex3f(minX,0.0,minZ);
		glTexCoord2f(1.0,0.0);
		glVertex3f(maxX,0.0,minZ);
		glTexCoord2f(1.0,1.0);
		glVertex3f(maxX,0.0,maxZ);
		glTexCoord2f(0.0,1.0);
		glVertex3f(minX,0.0,maxZ);
	glEnd();
	glColor3f(.5,.5,.5);
	glBegin(GL_QUADS);

            /* Ceiling */
            glVertex3f(minX,6,minZ);
            glVertex3f(maxX,6,minZ);
            glVertex3f(maxX,6,maxZ);
            glVertex3f(minX,6,maxZ);
            /* Walls */
           /* glVertex3f(minX,-2,maxZ);
            glVertex3f(maxX,-2,maxZ);
            glVertex3f(maxX,6,maxZ);
            glVertex3f(minX,6,maxZ);*/

            glVertex3f(minX,0,minZ);
            glVertex3f(maxX,0,minZ);
            glVertex3f(maxX,6,minZ);
            glVertex3f(minX,6,minZ);

            glVertex3f(maxX,6,maxZ);
            glVertex3f(maxX,0,maxZ);
            glVertex3f(maxX,0,minZ);
            glVertex3f(maxX,6,minZ);

            glVertex3f(minX,6,maxZ);
            glVertex3f(minX,0,maxZ);
            glVertex3f(minX,0,minZ);
            glVertex3f(minX,6,minZ);
        glEnd();

    glRotatef(turn,0,1,0);
    //GLfloat white[] = {0.8f, 0.8f, 0.8f, 1.0f};
    //GLfloat cyan[] = {0.f, .8f, .8f, 1.f};
    //glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
    //glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    //GLfloat shininess[] = {200};
    //glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	RockTexture.SetActive();
	// change here for whole scene translation

	glPushMatrix();
    //glTranslatef(1.3,0.0,1.0);
    SVertex * Vertices = new SVertex[NumOfVerticesStone*4];  //allocate mem for the required vertices
	ListNum = glGenLists(1);
	for (GLint i = 0; i<NumOfVerticesStone; i++)
	{
		Vertices[i].x = cos(2.0 * PI / NumOfVerticesStone * i) * OuterRadius;
		Vertices[i].y = StoneHeight;  //Top
		Vertices[i].z = sin(2.0 * PI / NumOfVerticesStone * i) * OuterRadius;
	}

	for (int i = 0; i<NumOfVerticesStone; i++)
	{
		Vertices[i + NumOfVerticesStone*1].x = cos(2.0 * PI / NumOfVerticesStone * i) * InnerRadius;
		Vertices[i + NumOfVerticesStone*1].y = StoneHeight;  //Top
		Vertices[i + NumOfVerticesStone*1].z = sin(2.0 * PI / NumOfVerticesStone * i) * InnerRadius;
	}

	for (int i = 0; i<NumOfVerticesStone; i++)
	{
		Vertices[i + NumOfVerticesStone*2].x = cos(2.0 * PI / NumOfVerticesStone * i) * OuterRadius;
		Vertices[i + NumOfVerticesStone*2].y = 0.0;  //Bottom
		Vertices[i + NumOfVerticesStone*2].z = sin(2.0 * PI / NumOfVerticesStone * i) * OuterRadius;
	}

	for (int i = 0; i<NumOfVerticesStone; i++)
	{
		Vertices[i + NumOfVerticesStone*3].x = cos(2.0 * PI / NumOfVerticesStone * i) * InnerRadius;
		Vertices[i + NumOfVerticesStone*3].y = 0.0;  //Bottom
		Vertices[i + NumOfVerticesStone*3].z = sin(2.0 * PI / NumOfVerticesStone * i) * InnerRadius;
	}

	//glNewList(ListNum, GL_COMPILE);

		glBegin(GL_QUADS);
		//ground quad:
		//glTranslatef(0.0,0.0,4.0);
		//glColor3f(1.0,1.0,0);
//		glVertex3f(-OuterRadius*1.3,0.0,OuterRadius*1.3);
//		glVertex3f(-OuterRadius*1.3,0.0,-OuterRadius*1.3);
//		glVertex3f(OuterRadius*1.3,0.0,-OuterRadius*1.3);
//		glVertex3f(OuterRadius*1.3,0.0,OuterRadius*1.3);
		//stone:
		for (int j = 1; j < 3; j++)
		{
			if (j == 1) glColor3f(1.0,0.6,0.6);
			if (j == 2) glColor3f(0.4,0.2,0.2);
			int i;
			for (i = 0; i<NumOfVerticesStone-1; i++)
			{
			    glNormal3f(0,1,0);
			    glTexCoord2f(1,1);
				glVertex3fv(&Vertices[i+NumOfVerticesStone*j].x);
				glTexCoord2f(0,1);
				glVertex3fv(&Vertices[i].x);
				glTexCoord2f(1,0);
				glVertex3fv(&Vertices[i+1].x);
				glTexCoord2f(1,0);
				glVertex3fv(&Vertices[i+NumOfVerticesStone*j+1].x);
			}
			glVertex3fv(&Vertices[i+NumOfVerticesStone*j].x);
			glVertex3fv(&Vertices[i].x);
			glVertex3fv(&Vertices[0].x);
			glVertex3fv(&Vertices[NumOfVerticesStone*j].x);
		}
        // color of texture of water
		glColor3f(0.5,0.5,0.7);
		/*int i;
		WaterTexture.SetActive();
		for (i = 0; i<NumOfVerticesStone-1; i++)
		{
			glVertex3fv(&Vertices[i+NumOfVerticesStone*3].x);
			glVertex3fv(&Vertices[i+NumOfVerticesStone].x);
			glVertex3fv(&Vertices[i+NumOfVerticesStone+1].x);
			glVertex3fv(&Vertices[i+NumOfVerticesStone*3+1].x);
		}
		glVertex3fv(&Vertices[i+NumOfVerticesStone*3].x);
		glVertex3fv(&Vertices[i+NumOfVerticesStone].x);
		glVertex3fv(&Vertices[NumOfVerticesStone].x);
		glVertex3fv(&Vertices[NumOfVerticesStone*3].x);
*/
		glEnd();
		//The "water":
		//glTranslatef(0.0,WaterHeight - StoneHeight, 0.0);
		//glBindTexture(GL_TEXTURE_2D, ID);

		glEnable(GL_TEXTURE_2D);
        WaterTexture.SetActive();
        glPushMatrix();
        glTranslatef(0.0,-0.2,0.0);
		glBegin(GL_POLYGON);

		for (int i = 0; i<NumOfVerticesStone; i++)
		{
		    glNormal3f(0,1,0);
			glTexCoord2f(	0.5+cos(i/GLfloat(NumOfVerticesStone)*360.0*PI/180.0)/2.0,
							0.5-sin(i/GLfloat(NumOfVerticesStone)*360.0*PI/180.0)/2.0);

			glVertex3fv(&Vertices[i+NumOfVerticesStone].x);
		}
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);

	glEndList();
	glPopMatrix();
	/*float bowlheight = 0.2 + POOL_HEIGHT;
	float bowlwidth = 0.2;

	float TexBorderDistance = bowlwidth / (MAXX+2*bowlwidth);
	GroundTexture.SetActive();

	glBegin(GL_QUADS);

		float minX = -4.0;
		float minZ = -4.0;
		float maxX = 8.0;
		float maxZ = 8.0;
		//******************
		//ground
		//******************
		glNormal3f(0.0f,1.0f,0.0);
		glTexCoord2f(0.0,0.0);
		glVertex3f(minX,0.0,minZ);
		glTexCoord2f(1.0,0.0);
		glVertex3f(maxX,0.0,minZ);
		glTexCoord2f(1.0,1.0);
		glVertex3f(maxX,0.0,maxZ);
		glTexCoord2f(0.0,1.0);
		glVertex3f(minX,0.0,maxZ);
	glEnd();

	RockTexture.SetActive();

	glBegin(GL_QUADS);

		//******************
		//top
		//******************
		glNormal3f(0.0f,1.0f,0.0);

		glTexCoord2f(TexBorderDistance,TexBorderDistance);
		glVertex3f(0.0f,bowlheight,0.0);
		glTexCoord2f(1.0-TexBorderDistance,TexBorderDistance);
		glVertex3f(MAXX,bowlheight,0.0);
		glTexCoord2f(1.0-TexBorderDistance,0.0);
		glVertex3f(MAXX,bowlheight,-bowlwidth);
		glTexCoord2f(TexBorderDistance,0.0);
		glVertex3f(0.0f,bowlheight,-bowlwidth);

		glTexCoord2f(TexBorderDistance,0.0);
		glVertex3f(0.0f,bowlheight,-bowlwidth);
		glTexCoord2f(0.0,0.0);
		glVertex3f(-bowlwidth,bowlheight,-bowlwidth);
		glTexCoord2f(0.0,1.0-TexBorderDistance);
		glVertex3f(-bowlwidth,bowlheight,MAXZ);
		glTexCoord2f(TexBorderDistance,1.0-TexBorderDistance);
		glVertex3f(0.0f,bowlheight,MAXZ);
/*
		glTexCoord2f(1.0,0.0);
		glVertex3f(MAXX+bowlwidth,bowlheight,-bowlwidth);
		glTexCoord2f(1.0-TexBorderDistance,0.0);
		glVertex3f(MAXX,bowlheight,-bowlwidth);
		glTexCoord2f(1.0-TexBorderDistance,1.0-TexBorderDistance);
		glVertex3f(MAXX,bowlheight,MAXZ);
		glTexCoord2f(1.0,1.0-TexBorderDistance);
		glVertex3f(MAXX+bowlwidth,bowlheight,MAXZ);

		glTexCoord2f(1.0,1.0-TexBorderDistance);
		glVertex3f(MAXX+bowlwidth,bowlheight,MAXZ);
		glTexCoord2f(0.0,1.0-TexBorderDistance);
		glVertex3f(-bowlwidth,bowlheight,MAXZ);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-bowlwidth,bowlheight,MAXZ+bowlwidth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(MAXX+bowlwidth,bowlheight,MAXZ+bowlwidth);

		//******************
		//front
		//******************
		glNormal3f(0.0f,0.0f,1.0f);

		glTexCoord2f(TexBorderDistance,TexBorderDistance);
		glVertex3f(0.0f,bowlheight,0.0);
		glTexCoord2f(1.0-TexBorderDistance,TexBorderDistance);
		glVertex3f(MAXX,bowlheight,0.0);
		glTexCoord2f(1.0-TexBorderDistance,0.0);
		glVertex3f(MAXX,0.0f,0.0);
		glTexCoord2f(TexBorderDistance,0.0);
		glVertex3f(0.0f,0.0f,0.0);


		glTexCoord2f(0.0,1.0-TexBorderDistance);
		glVertex3f(-bowlwidth,bowlheight,MAXZ+bowlwidth);
		glTexCoord2f(1.0,1.0-TexBorderDistance);
		glVertex3f(MAXX+bowlwidth,bowlheight,MAXZ+bowlwidth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(MAXX+bowlwidth,0.0f,MAXZ+bowlwidth);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-bowlwidth,0.0f,MAXZ+bowlwidth);


		//******************
		//back
		//******************
		glNormal3f(0.0,0.0,-1.0f);

		glTexCoord2f(TexBorderDistance,TexBorderDistance);
		glVertex3f(0.0f,bowlheight,MAXZ);
		glTexCoord2f(1.0-TexBorderDistance,TexBorderDistance);
		glVertex3f(MAXX,bowlheight,MAXZ);
		glTexCoord2f(1.0-TexBorderDistance,0.0);
		glVertex3f(MAXX,0.0f,MAXZ);
		glTexCoord2f(TexBorderDistance,0.0);
		glVertex3f(0.0f,0.0f,MAXZ);

		glTexCoord2f(0.0,1.0-TexBorderDistance);
		glVertex3f(-bowlwidth,bowlheight,-bowlwidth);
		glTexCoord2f(1.0,1.0-TexBorderDistance);
		glVertex3f(MAXX+bowlwidth,bowlheight,-bowlwidth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(MAXX+bowlwidth,0.0f,-bowlwidth);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-bowlwidth,0.0f,-bowlwidth);


		//******************
		//side
		//******************
		glNormal3f(-1.0,0.0,0.0);

		glTexCoord2f(1.0-TexBorderDistance,TexBorderDistance);
		glVertex3f(MAXX,bowlheight,0.0);
		glTexCoord2f(1.0,1.0-TexBorderDistance);
		glVertex3f(MAXX,bowlheight,MAXZ);
		glTexCoord2f(1.0-TexBorderDistance,1.0-TexBorderDistance);
		glVertex3f(MAXX,0.0f,MAXZ);
		glTexCoord2f(1.0,TexBorderDistance);
		glVertex3f(MAXX,0.0f,0.0);


		glTexCoord2f(0.0,0.0);
		glVertex3f(-bowlwidth,bowlheight,-bowlwidth);
		glTexCoord2f(1.0,0.0);
		glVertex3f(-bowlwidth,bowlheight,MAXZ+bowlwidth);
		glTexCoord2f(1.0,TexBorderDistance);
		glVertex3f(-bowlwidth,0.0f,MAXZ+bowlwidth);
		glTexCoord2f(0.0,TexBorderDistance);
		glVertex3f(-bowlwidth,0.0f,-bowlwidth);

		glNormal3f(1.0,0.0,0.0);

		glTexCoord2f(1.0-TexBorderDistance,0.0);
		glVertex3f(0.0f,bowlheight,MAXZ);
		glTexCoord2f(TexBorderDistance,0.0);
		glVertex3f(0.0f,bowlheight,0.0);
		glTexCoord2f(TexBorderDistance,TexBorderDistance);
		glVertex3f(0.0f,0.0f,0.0);
		glTexCoord2f(1.0-TexBorderDistance,TexBorderDistance);
		glVertex3f(0.0f,0.0f,MAXZ);

		glTexCoord2f(1.0,1.0);
		glVertex3f(MAXX+bowlwidth,bowlheight,MAXZ+bowlwidth);
		glTexCoord2f(1.0,0.0);
		glVertex3f(MAXX+bowlwidth,bowlheight,-bowlwidth);
		glTexCoord2f(1.0-TexBorderDistance,0.0);
		glVertex3f(MAXX+bowlwidth,0.0f,-bowlwidth);
		glTexCoord2f(1.0-TexBorderDistance,1.0);
		glVertex3f(MAXX+bowlwidth,0.0f,MAXZ+bowlwidth);

	glEnd();*/



}

void DrawScene(void)
{

	//Render the pool
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	WaterTexture.SetActive();
	glPushMatrix();
		glTranslatef(0.0,POOL_HEIGHT,0.0);
		//Pool.Render();
	glPopMatrix();
    glPushMatrix();
	//Render the bowl


	RenderBowl();

    glPopMatrix();
    // postion of fountain is set
    glTranslatef(-1.25,0,-1.2);
	glDisable(GL_TEXTURE_2D);

	//Render the water in the air.
	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glColor4f(0.8,0.8,0.8,0.8);
	AirFountain.Render();

	glDisable(GL_BLEND);

}


void Display(void)
{
    turn = turn +10;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	Camera.Render();

	//glLightfv(GL_LIGHT0,GL_POSITION,LightPosition);
	//Turn two sided lighting on:
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	DrawScene();
	/*GLfloat position[] = {.5,2,0,1};
    GLfloat direction[] = {0,-2,0};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, 15);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);

    GLfloat position1[] = {-.5,2,0,1};
    GLfloat direction1[] = {0,-1,0};
    glLightfv(GL_LIGHT1, GL_POSITION, position1);
    glLighti(GL_LIGHT1, GL_SPOT_CUTOFF, 15);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction1);

    GLfloat position2[] = {1,1,0,1};
    GLfloat direction2[] = {0,-1,0};
    glLightfv(GL_LIGHT2, GL_POSITION, position2);
    glLighti(GL_LIGHT2, GL_SPOT_CUTOFF, 15);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction2);

    GLfloat position3[] = {.75,1,0,1};
    GLfloat direction3[] = {0,-2,0};
    glLightfv(GL_LIGHT3, GL_POSITION, position3);
    glLighti(GL_LIGHT3, GL_SPOT_CUTOFF, 15);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction3);*/

    // internal
   GLfloat position[] = {1,1,1.5,1};
    GLfloat direction[] = {-1,-1,-1.5};
    GLfloat diffuse[] = {0,1,0};
    GLfloat ambient[] = {0,1,0};
    GLfloat specular[] = {0,1,0};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, 8);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);
    glLightfv(GL_LIGHT0, GL_SPECULAR,specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT,ambient);
    glEnable(GL_LIGHT0);

    // internal
    GLfloat position1[] = {2,2,1,1};
    GLfloat direction1[] = {-1,-2,-1};
    GLfloat diffuse1[] = {0,0,1};
    GLfloat ambient1[] = {0,0,1};
    GLfloat specular1[] = {0,0,1};
    glLightfv(GL_LIGHT1, GL_POSITION, position1);
    glLighti(GL_LIGHT1, GL_SPOT_CUTOFF, 10);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction1);
    glLightfv(GL_LIGHT1, GL_SPECULAR,specular1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,diffuse1);
    glLightfv(GL_LIGHT1, GL_AMBIENT,ambient1);
    glEnable(GL_LIGHT1);

    // internal
    GLfloat position2[] = {2,2,1,1};
    GLfloat direction2[] = {0,-2,-1};
    GLfloat diffuse2[] = {1,0,0};
    GLfloat ambient2[] = {1,0,0};
    GLfloat specular2[] = {1,0,0};
    glLightfv(GL_LIGHT2, GL_POSITION, position2);
    glLighti(GL_LIGHT2, GL_SPOT_CUTOFF, 10);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, direction2);
    glLightfv(GL_LIGHT2, GL_SPECULAR,specular2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,diffuse2);
    glLightfv(GL_LIGHT2, GL_AMBIENT,ambient2);
    glEnable(GL_LIGHT2);

    // external
    GLfloat position3[] = {-2,-2,-1,1};
    GLfloat direction3[] = {3,1,1};
    GLfloat diffuse3[] = {.6,.6,0};
    GLfloat ambient3[] = {.6,.6,0};
    GLfloat specular3[] = {.6,.6,0};
    glLightfv(GL_LIGHT3, GL_POSITION, position3);
    glLighti(GL_LIGHT3, GL_SPOT_CUTOFF, 30);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, direction3);
    glLightfv(GL_LIGHT3, GL_SPECULAR,specular3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE,diffuse3);
    glLightfv(GL_LIGHT3, GL_AMBIENT,ambient3);
    glEnable(GL_LIGHT3);

    // external
    GLfloat position4[] = {2,-1,-1,1};
    GLfloat direction4[] = {-1,1,1};
    GLfloat diffuse4[] = {1,0,0};
    GLfloat ambient4[] = {1,0,0};
    GLfloat specular4[] = {1,0,0};
    glLightfv(GL_LIGHT4, GL_POSITION, position4);
    glLighti(GL_LIGHT4, GL_SPOT_CUTOFF, 30);
    glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, direction4);
    glLightfv(GL_LIGHT4, GL_SPECULAR,specular4);
    glLightfv(GL_LIGHT4, GL_DIFFUSE,diffuse4);
    glLightfv(GL_LIGHT4, GL_AMBIENT,ambient4);
    glEnable(GL_LIGHT4);

    //external ambient
   GLfloat position5[] =  {0,2,2,1};
    GLfloat direction5[] = {-3,-2,-2};
//    GLfloat position5[] =  {-4,6,-6,1};
 //   GLfloat direction5[] = {-3,-6,-6};
    GLfloat diffuse5[] = {0,0,1};
    GLfloat ambient5[] = {0,0,1};
    GLfloat specular5[] = {0,0,1};
    glLightfv(GL_LIGHT5, GL_POSITION, position5);
    glLighti(GL_LIGHT5, GL_SPOT_CUTOFF, 30);
    glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, direction5);
    glLightfv(GL_LIGHT5, GL_SPECULAR,specular5);
    glLightfv(GL_LIGHT5, GL_DIFFUSE,diffuse5);
    glLightfv(GL_LIGHT5, GL_AMBIENT,ambient5);
    glEnable(GL_LIGHT5);

    //external
/*    GLfloat position6[] =  {2,-1,-1,1};
    GLfloat direction6[] = {0,1,1};
    GLfloat diffuse6[] = {1,1,0};
    GLfloat ambient6[] = {1,1,0};
    GLfloat specular6[] = {1,1,0};
    glLightfv(GL_LIGHT6, GL_POSITION, position6);
    glLighti(GL_LIGHT6, GL_SPOT_CUTOFF, 7);
    glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, direction6);
    glLightfv(GL_LIGHT6, GL_SPECULAR,specular6);
    glLightfv(GL_LIGHT6, GL_DIFFUSE,diffuse6);
    glLightfv(GL_LIGHT6, GL_AMBIENT,ambient6);
    glEnable(GL_LIGHT6);*/

	glFlush();			///Finish rendering
	glutSwapBuffers();
}

void Reshape(int x, int y)
{
	if (y == 0 || x == 0) return;  //Nothing is visible then, so return
	//Set a new projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//Angle of view:40 degrees
	//Near clipping plane distance: 0.3
	//Far clipping plane distance: 50.0
	gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.3,50.0);
	glViewport(0,0,x,y);  //Use the whole window for rendering
	glMatrixMode(GL_MODELVIEW);

}

void Idle(void)
{
	//Do the physical calculation for one step:
	float dtime = 0.006;
	AirFountain.Update(dtime, &Pool);
	Pool.Update(dtime);

	//render the scene:
	Display();
}

int main (int argc, char **argv)
{
	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowSize(600,600);
	//Create a window with rendering context and everything else we need
	glutCreateWindow("Fountain with simulated water");
	//compute the vertices and indices
	Pool.Initialize(NUM_X_OSCILLATORS,NUM_Z_OSCILLATORS,OSCILLATOR_DISTANCE,OSCILLATOR_WEIGHT, 0.05, 4.0, 4.0);
	//init the airfountain: (look at KeyDown() to see more possibilities of initialization)
	//AirFountain.Initialize(3,8,35,76,90,0.5,0.11);


	//place it in the center of the pool:
	AirFountain.Position = F3dVector(NUM_X_OSCILLATORS*OSCILLATOR_DISTANCE/2.0f,
									 POOL_HEIGHT,
								 	 NUM_Z_OSCILLATORS*OSCILLATOR_DISTANCE/2.0f);
	//initialize camera:
	Camera.Move(F3dVector(NUM_X_OSCILLATORS*OSCILLATOR_DISTANCE / 2.0,3.0,NUM_X_OSCILLATORS*OSCILLATOR_DISTANCE+4.0));
	Camera.RotateX(-20);

	//Enable the vertex array functionality:
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);



	//Switch on solid rendering:
	g_bFillModePoints = false;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);


    	//Initialize lighting:
	g_bLighting = true;
	//glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	//glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	//glEnable(GL_LIGHT1);


    /*GLfloat specular[] = {1,0,0,1};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHT0);
    GLfloat specular[] = {0,0,1,1};
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glEnable(GL_LIGHT1);
    GLfloat specular[] = {-1,0,0,1};
    glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
    glEnable(GL_LIGHT2);
    GLfloat specular[] = {0,0,-1,1};
    glLightfv(GL_LIGHT3, GL_SPECULAR, specular);
    glEnable(GL_LIGHT3);*/
    //GLfloat position[] = {1,4,3,1};
    //GLfloat direction[] = {-1,-4,-3};
    //GLfloat specular[] = {1,1,1,1};

    //glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
   // glLightfv(GL_LIGHT3,GL_SPECULAR,specular);

    //glLightfv(GL_LIGHT3, GL_AMBIENT, LightAmbient);
	//glLightfv(GL_LIGHT3, GL_DIFFUSE, LightDiffuse);


    //glEnable(GL_LIGHT0);

    //glEnable(GL_LIGHT2);
    //glEnable(GL_LIGHT3);


    glEnable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);

	//Some general settings:
	glClearColor(0.2,0.2,0.7,0.0);
	glFrontFace(GL_CCW);   //Tell OGL which orientation shall be the front face
	glShadeModel(GL_SMOOTH);


	//Initialize blending:
	glEnable(GL_BLEND);
	glPointSize(3.0);
	glEnable(GL_POINT_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Load the textures:
	WaterTexture.LoadFromFile("water.bmp");
	RockTexture.LoadFromFile("rock.bmp");
	GroundTexture.LoadFromFile("ground.bmp");


	//initialize generation of random numbers:
	srand((unsigned)time(NULL));

	//Assign the two used Msg-routines
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyDown);
	glutIdleFunc(Idle);
	//Let GLUT get the msgs
	glutMainLoop();

	//Clean up:
	AirFountain.Delete();

	return 0;
}

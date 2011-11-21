#include <GL/glut.h>
#include <stdio.h>      // Header file for standard file i/o.
#include <stdlib.h>     // Header file for malloc/free.
#include <unistd.h>     // needed to sleep.



/* Image type - contains height, width, and data */
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;

};
typedef struct Image Image;
class COGLTexture
{
public:
/*	_AUX_RGBImageRec *Image;*/
	unsigned int GetID();
	void LoadFromFile(char *filename);
	int ImageLoad (char *filename,Image *Image);

	void SetActive();
	int GetWidth();
	int GetHeight();
private:
	int Width, Height;
	unsigned int ID;
};


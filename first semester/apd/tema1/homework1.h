#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define MAXPIXELVALUE 255
typedef unsigned char Pixel;

typedef struct Image{
	int type;
	unsigned int height;
	unsigned int width;
	unsigned int maxval;
	Pixel **pixels;
}image;

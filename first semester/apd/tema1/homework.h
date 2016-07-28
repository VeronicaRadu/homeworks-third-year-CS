
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>

typedef struct rgb
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} RGB, *pRGB, **ppRGB;

typedef unsigned char GRAY;

// typedef struct pixel
// {
// 	RGB  rgbValue;
// 	GRAY grayValue;
// } Pixel, *pPixel, **ppPixel;



/*
	A "magic number" for identifying the file type. A pgm image's magic number 
is the two characters "P5".
	Whitespace (blanks, TABs, CRs, LFs).
	A width, formatted as ASCII characters in decimal.
	Whitespace.
	A height, again in ASCII decimal.
	Whitespace.
	The maximum gray value (Maxval), again in ASCII decimal. Must be less than 
65536, and more than zero.
	A single whitespace character (usually a newline).
	A raster of Height rows, in order from top to bottom. Each row consists of 
Width gray values, in order from left to right. Each gray value is a number from
0 through Maxval, with 0 being black and Maxval being white. Each gray value is 
represented in pure binary by either 1 or 2 bytes. If the Maxval is less than 
256, it is 1 byte. Otherwise, it is 2 bytes. The most significant byte is first.
*/

typedef struct Image 
{
	unsigned int width;
	unsigned int height;
	unsigned int maxval;
	char pMagicNumber;

	GRAY **gPixels;
	ppRGB  cPixels; 	
} image, *pimage, **ppimage;

//__attribute__((packed, aligned(8)))
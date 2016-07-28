#include "homework1.h"

int num_threads;
int resolution;

int noMeters = 100;

//===========data structure related=============={
void initialize(image *im) 
{
	if(NULL == im)
	{
		return;
	}
	int i = 0;
	im->type 	= 5;
	im->height 	= resolution;
	im->width	= resolution;
	im->maxval	= MAXPIXELVALUE;

	im->pixels 	= calloc(im->height, sizeof(Pixel*));
	for(i = 0; i < im->height; i++)
	{
		im->pixels[i] = calloc(im->width, sizeof(Pixel));
		int j = 0;
		for(j = 0; j < im->width; j++)
		{
			im->pixels[i][j] = MAXPIXELVALUE;
		}
	}
}

void freeImg(image *im)
{
	if(NULL == im)
	{
		return;
	}
	int i = 0;
	for(i = 0; i < im->height; i++)
	{
		free(im->pixels[i]);
	}
	free(im->pixels);

}
//}==============================================

//=========render and logic======================{

double calculateDistance(int x, int y) 
{
    return abs(-1 * x + 2 * y) / sqrt(5);
}

void inline fillSquare(image *im, int ii, int jj, int scale)
{
	int i = 0;
	int j = 0;
	for (i = 0; i < scale; ++i)
	{
		for (j = 0; j < scale; ++j)
		{
			im->pixels[ii * scale + i][jj * scale + j] = 0;
		}
	}
}

void render(image *im) 
{
	int i = 0;
	int j = 0;
	// how many pixels represent a meter
	int scale = resolution / noMeters;
	omp_set_num_threads(num_threads);
	#pragma omp parallel for shared(scale, im, noMeters) private(i, j) collapse(2) 
	for (i = 0; i < noMeters; ++i)
	{
		for (j = 0; j < noMeters; ++j)
		{
			if(calculateDistance(i, j) < 3)
			{
				fillSquare(im, i, j, scale);
			}
		}
	}
}
//}==============================================


//=============IO operations====================={
// void cyclic_roll(int *a, int *b, int *c, int *d)
// {
//    int temp = *a;
//    *a = *b;
//    *b = *c;
//    *c = *d;
//    *d = temp;
// }

// void rotate90ClkWs(image *im)
// {
// 	int n = im->width;
// 	int i = 0;
// 	int j = 0;
// 	for(i = 0; i < n / 2; i++)
// 	{
//    		for(j = 0; j < (n + 1) / 2; j++)
//    		{
//        		cyclic_roll(&m[i][j], &m[n - 1 -j][i], &m[n - 1 - i][n - 1 - j], &m[j][n - 1 - i]);
//    		}
// 	}
// }


void writeData(const char * fileName, image *img) 
{
	if(NULL == fileName)
	{
		return;
	}

	if(NULL == img)
	{
		return;
	}

	FILE* pFile = NULL;
	pFile = fopen(fileName, "wr");
	if(NULL == pFile)
	{
		return;
	}

	fprintf(pFile, "P%d\n%d %d\n%d\n", img->type, img->width, img->height, img->maxval);
	
	int i = 0;
	int j = 0;


	for(i = img->width - 1; i >= 0; i--)
	{
		for(j = 0; j < img->height; j++)
			fwrite (&img->pixels[j][i], sizeof(Pixel), 1, pFile);
	}

	freeImg(img);
	img = NULL;

	fclose(pFile);
	pFile = NULL;
}
//}============================================


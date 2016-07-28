#include "homework.h"

int num_threads;
int resize_factor;

//#define DEBUG_FLAG

void initialize(image *img)
{
	if(NULL != img)
	{
		img->pMagicNumber 	= 0;
		img->width 		= 0;
		img->height 		= 0;
		img->maxval		= 0;
		img->gPixels		= NULL;
		img->cPixels		= NULL;
	}
}

void freeImg(image *img)
{
	#ifdef DEBUG_FLAG
	printf("free\n");
	#endif
	if(NULL == img)
	{
		return;
	}
	if(6 == img->pMagicNumber)
	{
		int i = 0;
		for(i = 0; i < img->height; i++)
		{
			if(NULL != img->cPixels[i])
			{
				free(img->cPixels[i]);
				img->cPixels[i] = NULL;
			}
		}
		free(img->cPixels);
		img->cPixels = NULL;
	}

	if(5 == img->pMagicNumber)
	{
		int i = 0;
		for(i = 0; i < img->height; i++)
		{
			if(NULL != img->gPixels[i])
			{
				free(img->gPixels[i]);
				img->gPixels[i] = NULL;
			}
		}
		free(img->gPixels);
		img->gPixels = NULL;
	}
	
	#ifdef DEBUG_FLAG
	printf("leave Free\n");
	#endif
}

void allocate(image *img)
{
	int i = 0;
	if(6 == img->pMagicNumber)//colors
	{
		img->cPixels = calloc(img->height, sizeof(pRGB));
		for(i = 0; i < img->height; i++)
		{
			img->cPixels[i] = calloc(img->width, sizeof(RGB));
		}
	}
	else if(5 == img->pMagicNumber)//gray
	{
		img->gPixels = calloc(img->height, sizeof(GRAY*));
		for(i = 0; i < img->height; i++)
		{
			img->gPixels[i] = calloc(img->width, sizeof(GRAY));
		}
	}
}

void readInput(const char * fileName, image *img) 
{
	if(NULL == fileName)
	{
		return;
	}

	if(NULL == img)
	{
		return;
	}

	initialize(img);

	FILE* fin = NULL;
	fin = fopen(fileName, "rb");
	if(NULL == fin)
	{
		return;
	}

	char magicNo[2];
	fscanf(fin, "%s\n", magicNo);
	fscanf(fin, "%d %d\n%d\n", &(img->width), &(img->height), &(img->maxval));
	img->pMagicNumber = magicNo[1] - '0';

	#ifdef DEBUG_FLAG
	printf("[P%d w=%d h=%d m=%hd]\n", 
		img->pMagicNumber, 
		img->width, 
		img->height, 
		img->maxval);
	#endif
	
	allocate(img);
	int i = 0;
	if(6 == img->pMagicNumber)// color
	{
		#ifdef DEBUG_FLAG
		printf("Color\n");
		#endif

		for(i = 0; i < img->height; i++)
		{
			fread (img->cPixels[i], sizeof(RGB), img->width, fin);
		}
	}
	else if (5 == img->pMagicNumber)//grayscale
	{
		#ifdef DEBUG_FLAG
		printf("Color\n");
		#endif

		for(i = 0; i < img->height; i++)
		{
			fread (img->gPixels[i], sizeof(GRAY), img->width, fin);
		}
	}

	fclose(fin);
	fin = NULL;
}

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

	#ifdef DEBUG_FLAG
	printf("writing [P%d w=%d h=%d m=%hd]\n", 
		img->pMagicNumber, 
		img->width, 
		img->height, 
		img->maxval);
	#endif
	
	fprintf(pFile, "P%d\n%d %d\n%d\n", 
		img->pMagicNumber, 
		img->width, 
		img->height, 
		img->maxval);
	
	int i = 0;
	if(6 == img->pMagicNumber)// color
	{
		#ifdef DEBUG_FLAG
		printf("Color\n");
		#endif

		for(i = 0; i < img->height; i++)
		{	
			fwrite (img->cPixels[i], sizeof(RGB), img->width, pFile);
		}

	}
	else if (5 == img->pMagicNumber)//grayscale
	{
		#ifdef DEBUG_FLAG
		printf("Gray\n");
		#endif

		for(i = 0; i < img->height; i++)
		{
			fwrite (img->gPixels[i], sizeof(GRAY), img->width, pFile);
		};
	}

	freeImg(img);
	img = NULL;

	//fprintf(pFile, "\n");
	fclose(pFile);
	pFile = NULL;
}

void inline gaussianKernel(image *original, int ii, int jj, image *result)
{
	if(NULL == original || NULL == result)
	{
		return;
	}

	unsigned int gaussKernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
	unsigned int kernSum = 16;
	
	int i = 0;
	int j = 0;

	if (5 == original->pMagicNumber)
	{
		unsigned int matSum = 0;
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
			{
				matSum += gaussKernel[i][j] * original->gPixels[ii * 3 + i][jj * 3 + j];
			}		
		}
		result->gPixels[ii][jj] = (unsigned char)(matSum / kernSum);
	}
	else if (6 == original->pMagicNumber)
	{
		unsigned int matSumR = 0;
		unsigned int matSumG = 0;
		unsigned int matSumB = 0;
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
			{
				matSumR += gaussKernel[i][j] * original->cPixels[ii * 3 + i][jj * 3 + j].r;
				matSumG += gaussKernel[i][j] * original->cPixels[ii * 3 + i][jj * 3 + j].g;
				matSumB += gaussKernel[i][j] * original->cPixels[ii * 3 + i][jj * 3 + j].b;
			}		
		}
		result->cPixels[ii][jj].r = (unsigned char)(matSumR / kernSum);
		result->cPixels[ii][jj].g = (unsigned char)(matSumG / kernSum);
		result->cPixels[ii][jj].b = (unsigned char)(matSumB / kernSum);		
	}

}

void inline averageChunk(image *original, int ssi, int ssj, int dji, int djj, 
						 image *result, int ii, int jj)
{
	if(NULL == original || NULL == result)
	{
		return;
	}

	int i = 0;
	int j = 0;
	unsigned int cnt = 0;
	if (5 == original->pMagicNumber)
	{
		unsigned int sum = 0;
		for(i = ssi; i < dji; i++)
		{
			for(j = ssj; j < djj; j++)
			{
				cnt++;
				sum += original->gPixels[i][j];
			}
		}
		result->gPixels[ii][jj] = sum / cnt;
	} 
	else if (6 == original->pMagicNumber)
	{
		unsigned int sumR = 0;
		unsigned int sumG = 0;
		unsigned int sumB = 0;
		for(i = ssi; i < dji; i++)
		{
			for(j = ssj; j < djj; j++)
			{
				cnt++;
				sumR += (unsigned int)original->cPixels[i][j].r;
				sumG += (unsigned int)original->cPixels[i][j].g;
				sumB += (unsigned int)original->cPixels[i][j].b;
			}
		}
		result->cPixels[ii][jj].r = (unsigned char)(sumR / cnt);
		result->cPixels[ii][jj].g = (unsigned char)(sumG / cnt);
		result->cPixels[ii][jj].b = (unsigned char)(sumB / cnt);

		#ifdef DEBUG_FLAG
		// printf("(%d|%d|%d)[%d,%d]\n", 
		// 	result->cPixels[ii][jj].r,
		// 	result->cPixels[ii][jj].g,
		// 	result->cPixels[ii][jj].b,
		// 	ii, 
		// 	jj);
		#endif
	}
}

void resize(image *in, image * out) 
{ 
	#ifdef DEBUG_FLAG
	printf("Resize->\n");
	#endif
	if(NULL == in || NULL == out)
	{
		return;
	}

	out->pMagicNumber 	= in->pMagicNumber;
	out->maxval 		= in->maxval;
	out->height			= in->height / resize_factor;
	out->width			= in->width	 / resize_factor;
	
	allocate(out);
	int i = 0;
	int j = 0;
	int chunk = in->height / num_threads;
	
	omp_set_num_threads(num_threads);
	//schedule(dynamic, 10) collapse(1) shared(in,out,resize_factor) 
	#pragma omp parallel for private(i,j) collapse(2) shared(chunk,resize_factor, in, out)
	for(i = 0; i < out->height; i++)
	{
		for(j = 0; j < out->width; j++)
		{
			if(3 == resize_factor)
			{
				gaussianKernel(in, i, j, out);
			}
			else if (resize_factor % 2 == 0) // average
			{
				int coltSSi = i * resize_factor;
				int coltSSj = j * resize_factor;
				int coltDJi = coltSSi + resize_factor;
				int coltDJj = coltSSj + resize_factor;
				averageChunk(in, coltSSi, coltSSj, coltDJi, coltDJj, out, i, j);
			}
		}
	}
	

	#ifdef DEBUG_FLAG
	printf("->Resize\n");
	#endif
}

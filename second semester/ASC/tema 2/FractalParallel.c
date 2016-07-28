#include "Fractal.h"
#include "Image.h"

#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <complex.h>
#include <math.h>
#include <pthread.h>


#define ANTIALIASING 2
#define MAX_ITER 25000
#define THREAD_CNT 8

static long double escape;
static long double escape_squared;

static long double log_escape;
static long double log_two;
static pthread_t thr_ids[THREAD_CNT];

/*ugly linear search is ugly*/
static ushort thread_index() {
    pthread_t me = pthread_self();
    for (ushort index = 0; index < THREAD_CNT; ++index) {
        if (thr_ids[index] == me)
            return index;
    }
	return 0;
}

struct Fractal *Fractal_Create(char *filename, int width, int height, long double complex position, long double zoom)
{
	struct Fractal *fractal = (struct Fractal *)malloc (sizeof(struct Fractal));
	struct Image *image = image_create(width, height, filename);

	fractal->image = image;
	fractal->width = width;
	fractal->height = height;
	fractal->position = position;
	fractal->zoom = zoom;

	// Initialise static variables
	// Initialise static variables
	escape = 1000.0l;
	escape_squared = escape * escape;

	log_escape = logl(escape);
	log_two = logl(2.0l);
	return fractal;
}

void Fractal_Destroy(struct Fractal *fractal)
{
	image_close(fractal->image);
	free(fractal);
}

inline static uint8_t Wrap(ushort num)
{
	num %= 510;
	return num + (ushort)(num / 255) * (510 - num * 2);
}

inline static uint iterate(double complex num)
{
	if (creal(num) == 0 && cimag(num) == 0)
		return 0;

	register  double crl = creal(num);
	register  double cim = cimag(num);
	register  double nrl = crl;
	register  double nim = cim;
	register  double length = 0;
	register short i = 0;

	while (length < escape_squared && ++i < MAX_ITER) {
		length = nrl * nrl - nim * nim + crl;
		nim = 2 * nrl * nim + cim;
		nrl = length;
		length = nrl * nrl + nim * nim;
	}

	if (length >= escape_squared) {
		double moo = i + 1.0l - (log(log(sqrt(length)) / log_escape) / log_two);
		return log(moo) * 175;
	}
	return 0;
}


void *partial_fratal_render(void *arg)
{
	struct Fractal *fractal = (struct Fractal *)arg;
	uint8_t *array = fractal->image->array;

	long double ratio = fractal->height;
	ratio /= fractal->width;
	typedef long double data_t;
	register data_t num_real;
	register data_t num_imag;
	register long double complex num;
	register data_t acc = 0.0l;
	register data_t offset_real;
	register data_t offset_imag;
	register long double complex offset;

	uint lines_per_thread = fractal->height / THREAD_CNT;
    uint id = thread_index();
    ushort start_idx = id * lines_per_thread;
    ushort stop_idx = start_idx + lines_per_thread;
    array += start_idx * fractal->width * 3;
	uint8_t *ptr = array;

	for (uint line = start_idx; line < stop_idx; line++) {
		for (uint colm = 0; colm < fractal->width; colm++) {
			num_real = ((2 * colm + 1.0l) / fractal->width - 1.0l);
			num_imag = ((2 * line + 1.0l) / fractal->height - 1.0l) * ratio;
			num = num_real + num_imag * I;
			num *= fractal->zoom;
			num += fractal->position;

			acc = 0.0l;

			// ay = 0, ax = 0
			offset_real = -0.5l / fractal->width;
			offset_imag = -0.5l / fractal->height * ratio;
			offset = offset_real + offset_imag * I;
			offset *= fractal->zoom;
			num += offset;
			acc += 2 * iterate(num);

			//ay = 0, ax = 1
			offset_real = 0.5l / fractal->width;
			offset_imag = -0.5l / fractal->height * ratio;
			offset = offset_real + offset_imag * I;
			offset *= fractal->zoom;
			num += offset;
			acc += iterate(num);

			// ay = 1, ax = 0
			offset_real = -0.5l / fractal->width;
			offset_imag = 0.5l / fractal->height * ratio;
			offset = offset_real + offset_imag * I;
			offset *= fractal->zoom;
			num += offset;
			//acc += iterate(num);

			//ay = 1, ax = 1
			offset_real = 0.5l / fractal->width;
			offset_imag = 0.5l / fractal->height * ratio;
			offset = offset_real + offset_imag * I;
			offset *= fractal->zoom;
			num += offset;
			acc += iterate(num);

			acc /= ANTIALIASING * ANTIALIASING;

			*ptr = Wrap(acc * 4.34532457l);
			ptr++;
			*ptr = Wrap(acc * 2.93324292l);
			ptr++;
			*ptr = Wrap(acc * 1.2273444l);
			ptr++;
		}
	}
	printf("finished chunk %d\n", id);
	return NULL;
}



void *Fractal_Render(void *arg)
{
    for (ushort thread_idx = 0; thread_idx < THREAD_CNT; ++thread_idx) {
        pthread_create(&thr_ids[thread_idx], NULL, &partial_fratal_render, arg);
    }
    for (ushort thread_idx = 0; thread_idx < THREAD_CNT; ++thread_idx) {
        pthread_join(thr_ids[thread_idx], NULL);
    }
    printf("Rendered");
	return NULL;
}

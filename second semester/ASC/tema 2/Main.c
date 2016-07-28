#include "Fractal.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main()
{
  	int startframe=100;
	int endframe=105;	
	//long double complex position = -1.401141611950751478232223636650572367261E+00 + 6.134219667583451483105922099438475822671E-06l * I;
	long double complex position = -5.048022770149227918004480667050731332623E-02l + -8.081444572574222508156131387769408272814E-01l * I;

	for (int frame = startframe; frame < endframe; frame++) {
		long double zoom = powl(0.95, frame);

		char *filename;
		asprintf(&filename, "Image-%u.ppm", frame + 1);

		struct Fractal *fractal = Fractal_Create(filename, 600 * 1, 400 * 1, position, zoom);

		Fractal_Render(fractal);

		puts("");

		Fractal_Destroy(fractal);

		free(filename);
	}

	return 0;
}

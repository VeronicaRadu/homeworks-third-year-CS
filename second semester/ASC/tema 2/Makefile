OPTIM=-O0
OPTS=-lm -std=gnu99 -Wall -Wextra -Werror -D_GNU_SOURCE $(OPTIM)
all:
	gcc $(OTPIM) -c $(OPTS) Fractal.c -o runner1
	gcc $(OTPIM) -c $(OPTS) Image.c -o runner2
	gcc $(OTPIM) -c $(OPTS) Main.c -o runner3
	gcc runner1 runner2 runner3 -o fractal $(OPTS)

parallel:
	gcc -c $(OPTS) FractalParallel.c -o runner1 -pthread
	gcc -c $(OPTS) Image.c -o runner2
	gcc -c $(OPTS) Main.c -o runner3
	gcc runner1 runner2 runner3 -o fractal $(OPTS) -pthread

clean:
	rm -f runner*
	rm -f fractal
	rm -f *.ppm

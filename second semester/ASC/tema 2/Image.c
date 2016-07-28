#include "Image.h"

#include <sys/mman.h>
#include <unistd.h>
#include <sys/fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static void readHeader(FILE *fp, int *width, int *height, const char *filename)
{
	char ch;
	int  maxval;
	size_t n = 4095;
	char *line = (char *)malloc(n * sizeof(char));

	if (fscanf(fp, "P%c\n", &ch) != 1 || ch != '6') {
		fprintf(stderr, "File %s is not in ppm raw format!\n", filename);
		exit(1);
	}

	ch = getc(fp);
	while (ch == '#')
	{
		getline(&line, &n, stdin);
		ch = getc(fp);
	}

	if (!isdigit(ch)) {
		fprintf(stderr, "Cannot read header information from %s!\n", filename);
		exit(1);
	}

	ungetc(ch, fp);

	fscanf(fp, "%d %d\n%d\n", width, height, &maxval);

	if (maxval != 255) {
		fprintf(stderr, "image %s is not true-color (24 bit)", filename);
		exit(1);
	}

	if (*width < 1 || *width > 4000 || *height < 1 || *height > 4000) {
		fprintf(stderr, "image %s contains unreasonable dimensions!\n", filename);
		exit(1);
	}
}

struct Image *image_load(const char *filename)
{
	struct Image *image = (struct Image *)malloc(sizeof(struct Image));
	int width, height;
	size_t num;

	if (!image) {
		return NULL;
	}
	FILE *fp = fopen(filename, "rb");

	if (!fp) {
		fprintf(stderr, "Error loading file %s\n", filename);
		free(image);
		exit(1);
	}
	readHeader(fp, &width, &height, filename);
	image->fd = 0;
	image->height = height;
	image->width  = width;
	image->array_size = width * height * 3;
	image->array = (uint8_t *)malloc(image->array_size);

	if (!image->array) {
		fprintf(stderr, "Memory error!\n");
		exit(1);
	}
	num = fread(image->array, 1, image->array_size, fp);
	if (num != image->array_size) {
		fprintf(stderr, "Cannot read image data from %s\n", filename);
		exit(1);
	}
	fclose(fp);

	return image;
}

struct Image *image_create(size_t width, size_t height, const char *filename)
{
	struct Image *image = (struct Image *)calloc(1, sizeof(struct Image));

	if (!image) {
		return NULL;
	}

	// Open file
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);

	if (fd == -1) {
		perror("Error opening file");
		free(image);
		return NULL;
	}

	// Create PPM image header
	char *header;
	int header_length = asprintf(&header, "P6\n%zd %zd\n255\n", width, height);

	if (header_length == -1) {
		perror("Failed to allocate header");
		close(fd);
		free(image);
		return NULL;
	}

	write(fd, header, header_length);
	free(header);
	header = NULL;

	// Expand file to the full image length
	size_t array_size = width * height * 3;
	size_t file_size = header_length + array_size;

	// Map file to memory
	char *map = (char *)malloc(file_size * sizeof(char));

	if (map == NULL) {
		perror("Error mapping file");
		close(fd);
		free(image);
		return NULL;
	}

	image->width = width;
	image->height = height;
	image->fd = fd;
	image->array_size = image->width * image->height * 3;
	image->file_size = header_length + array_size;
	image->map = map;
	image->array = (uint8_t *)map + header_length;
	return image;
}

void image_close(struct Image *image)
{
	write(image->fd, image->array, image->array_size);
	close(image->fd);
	free(image);
}

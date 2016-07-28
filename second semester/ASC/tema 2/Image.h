#pragma once

#include <string.h>
#include <stdint.h>

struct Image {
	size_t width;
	size_t height;
	size_t array_size;
	size_t file_size;
	uint8_t *array;
	char *map;
	int fd;
};

struct Image *image_create(size_t width, size_t height, const char *filename);
struct Image *image_load(const char *filename);
void image_close(struct Image *image);

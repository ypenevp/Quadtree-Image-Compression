#ifndef IMAGEIO_H
#define IMAGEIO_H

#include <stdio.h>
#include <stdlib.h>

#define MEMCHECK(ptr)                         \
    if (ptr == NULL)                          \
    {                                         \
        printf("Error allocating memory!!!\n"); \
        exit(1);                              \
    }

#define FILECHECK(ptr, filename)                              \
    if (ptr == NULL)                                          \
    {                                                         \
        printf("Error opening file '%s'!!!\n", filename);      \
        exit(1);                                              \
    }


typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Pixel;

typedef struct
{
    int width;
    int height;
    Pixel *data;
} Image;

Image initImage(int width, int height);
void releaseImage(Image *img);

Image readPPM(const char *filename);
void writePPM(const char *filename, Image *img);

#endif
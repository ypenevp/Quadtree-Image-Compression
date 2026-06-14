#include "imageIO.h"

Image initImage(int width, int height)
{
    Image img;
    img.width = width;
    img.height = height;
    img.data = NULL;

    if (width > 0 && height > 0)
    {
        img.data = calloc(width * height, sizeof(Pixel));
        MEMCHECK(img.data);
    }

    return img;
}

void releaseImage(Image *img)
{
    free(img->data);
    img->data = NULL;
    img->width = 0;
    img->height = 0;
}

////////////////////////////////////////////////////

Image readPPM(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    FILECHECK(file, filename);

    fgetc(file); // for "P"
    fgetc(file); // for "6"

    int width = 0, height = 0, maxColor = 0;
    fscanf(file, "%d %d %d", &width, &height, &maxColor);
    fgetc(file); // read last \n before binary pixels data

    Image img = initImage(width, height);
    fread(img.data, sizeof(Pixel), width * height, file);

    fclose(file);
    return img;
}

void writePPM(const char *filename, Image img)
{
    FILE *file = fopen(filename, "wb");
    FILECHECK(file, filename);

    fprintf(file, "P6\n%d %d\n255\n", img.width, img.height);

    fwrite(img.data, sizeof(Pixel), img.width * img.height, file);

    fclose(file);
}
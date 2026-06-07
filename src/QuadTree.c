#include "QuadTree.h"

QTNode *createNodeQT(Pixel color, bool isLeaf)
{
    QTNode *newNode = malloc(sizeof(QTNode));
    MEMCHECK(newNode);
    newNode->color = color;
    newNode->isLeaf = isLeaf;
    for (int i = 0; i < 4; i++){
        newNode->children[i] = NULL;
    }
    return newNode;
}

static Pixel averageColor(Image *img, int startX, int startY, int a, double *variance)
{
    // Shrink region to the real image boundaries
    int maxX = startX + a > img->width ? img->width : startX + a;
    int maxY = startY + a > img->height ? img->height : startY + a;

    int count = 0;
    double avgR = 0, avgG = 0, avgB = 0;
    double varR = 0, varG = 0, varB = 0;

    for (int y = startY; y < maxY; y++)
    {
        for (int x = startX; x < maxX; x++)
        {
            count++;
            Pixel p = img->data[y * img->width + x]; // iterate through 2D img in 1D array

            double diffR = p.r - avgR;
            avgR += diffR / count;
            varR += diffR * (p.r - avgR);

            double diffG = p.g - avgG;
            avgG += diffG / count;
            varG += diffG * (p.g - avgG);

            double diffB = p.b - avgB;
            avgB += diffB / count;
            varB += diffB * (p.b - avgB);
        }
    }

    if (count == 0)
    {
        *variance = 0;
        Pixel empty = {0, 0, 0};
        return empty;
    }

    // Calculate color variance using luminance weights, where green contributes the most
    *variance = (0.2126 * varR + 0.7152 * varG + 0.0722 * varB) / count; 

    // Convert double average color to 8-bit RGB pixel format
    Pixel avgPixel = {(unsigned char)avgR, (unsigned char)avgG, (unsigned char)avgB};
    return avgPixel;
}

static QTNode *buildNodeQT(Image *img, int startX, int startY, int a, double threshold, int minBlockSize)
{
    if (startX >= img->width || startY >= img->height){
        return NULL; // skip regions that lie completely outside the image bounds
    }

    double variance = 0;
    Pixel avgColor = averageColor(img, startX, startY, a, &variance);

    if (a <= minBlockSize || variance <= threshold){
        return createNodeQT(avgColor, true);
    }

    QTNode *node = createNodeQT(avgColor, false);
    int halfA = a / 2;
    node->children[0] = buildNodeQT(img, startX, startY, halfA, threshold, minBlockSize);
    node->children[1] = buildNodeQT(img, startX + halfA, startY, halfA, threshold, minBlockSize);
    node->children[2] = buildNodeQT(img, startX, startY + halfA, halfA, threshold, minBlockSize);
    node->children[3] = buildNodeQT(img, startX + halfA, startY + halfA, halfA, threshold, minBlockSize);

    return node;
}

////////////////////////////////////////////////////

QuadTree initQT(Image *img, double threshold, int minBlockSize)
{
    QuadTree qt;
    qt.width = img->width;
    qt.height = img->height;

    int squareImgSize = 1;
    // Expand current rectangular image bounds to the closest square
    int currentImgMaxSize = img->width > img->height ? img->width : img->height;
    while (squareImgSize < currentImgMaxSize){
        squareImgSize *= 2;
    }

    qt.root = buildNodeQT(img, 0, 0, squareImgSize, threshold, minBlockSize);
    return qt;
}

void freeNodeQT(QTNode *node)
{
    if (node == NULL){
        return;
    }
    for (int i = 0; i < 4; i++){
        freeNodeQT(node->children[i]);
    }
    free(node);
}

void releaseQT(QuadTree *qt)
{
    freeNodeQT(qt->root);
    qt->root = NULL;
    qt->width = 0;
    qt->height = 0;
}

////////////////////////////////////////////////////

static void fillRegionImg(Image *img, int startX, int startY, int a, Pixel color)
{
    int maxX = startX + a > img->width ? img->width : startX + a;
    int maxY = startY + a > img->height ? img->height : startY + a;

    for (int y = startY; y < maxY; y++){
        for (int x = startX; x < maxX; x++){
            img->data[y * img->width + x] = color;
        }
    }

}

static void reconstructNodeQT(QTNode *node, Image *img, int startX, int startY, int a)
{
    if (node == NULL){
        return;
    }

    if (node->isLeaf)
    {
        fillRegionImg(img, startX, startY, a, node->color);
    }
    else
    {
        int halfA = a / 2;
        reconstructNodeQT(node->children[0], img, startX, startY, halfA);
        reconstructNodeQT(node->children[1], img, startX + halfA, startY, halfA);
        reconstructNodeQT(node->children[2], img, startX, startY + halfA, halfA);
        reconstructNodeQT(node->children[3], img, startX + halfA, startY + halfA, halfA);
    }
}

void reconstructImageQT(QuadTree *qt, Image *outputImg)
{
    int squareImgSize = 1;
    int currentImgMaxSize = qt->width > qt->height ? qt->width : qt->height;
    while (squareImgSize < currentImgMaxSize){
        squareImgSize *= 2;
    }

    reconstructNodeQT(qt->root, outputImg, 0, 0, squareImgSize);
}

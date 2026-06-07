#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdbool.h>
#include "imageIO.h"


typedef struct QTNode {
    bool isLeaf;
    Pixel color;
    struct QTNode *children[4];
} QTNode;

typedef struct {
    QTNode *root;
    int width;
    int height;
} QuadTree;

QTNode *createNodeQT(Pixel color, bool isLeaf);
QuadTree initQT(Image *img, double threshold, int minBlockSize);
void releaseQT(QuadTree *qt);

void reconstructImageQT(QuadTree *qt, Image *outputImg);

#endif
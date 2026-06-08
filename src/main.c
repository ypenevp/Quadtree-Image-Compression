#include <stdio.h>
#include <stdbool.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "imageIO.h"
#include "QuadTree.h"

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"
#define BRIGHT_YEL "\033[1;93m"
#define BRIGHT_GRN "\033[1;92m"
#define YELLOW "\033[0;33m"
#define WHITE "\033[0;37m"
#define GREEN "\033[1;32m"

#define MENU_WIDTH 42

////////////////////////////////////////////////////

void printMenu(const char *title, const char **options, int count, int selected)
{
    system("cls");
    printf("\n");

    printf(WHITE "  +" RESET);
    for (int i = 0; i < MENU_WIDTH; i++){
        printf(WHITE "-" RESET);
    }
    printf(WHITE "+\n" RESET);

    printf(WHITE "  |  " BRIGHT_GRN BOLD "%-*s" RESET WHITE "|\n" RESET, MENU_WIDTH - 2, title);

    printf(WHITE "  +" RESET);
    for (int i = 0; i < MENU_WIDTH; i++){
        printf(WHITE "-" RESET);
    }
    printf(WHITE "+\n" RESET);

    for (int i = 0; i < count; i++)
    {
        if (i == selected){
            printf(WHITE "  |  " BRIGHT_YEL "> %-*s" RESET WHITE "|\n" RESET, MENU_WIDTH - 4, options[i]);
        }
        else{
            printf(WHITE "  |  " DIM "  %-*s" RESET WHITE "|\n" RESET, MENU_WIDTH - 4, options[i]);
        }
    }

    printf(WHITE "  +" RESET);
    for (int i = 0; i < MENU_WIDTH; i++){
        printf(WHITE "-" RESET);
    }
    printf(WHITE "+\n\n" RESET);
}

int runMenu(const char *title, const char **options, int count)
{
    int selected = 0;
    while (true)
    {
        printMenu(title, options, count, selected);
        int key = _getch();
        if (key == 224)
        {
            key = _getch();
            if (key == 72){
                selected = (selected - 1 + count) % count;
            }
            else if (key == 80){
                selected = (selected + 1) % count;
            }
        }
        else if (key == 13)
        {
            return selected;
        }
    }
}

////////////////////////////////////////////////////

int getImagePath(char *pathBuffer, bool *isCustom)
{
    const char *opts[] = {"Default (assets/input.ppm)", "Custom path", "Back"};
    int choice = runMenu("Select Image Source", opts, 3);

    switch (choice)
    {
        case 0:
            system("cls");
            *isCustom = false;
            strcpy(pathBuffer, "assets/input.ppm");
            break;

        case 1:
            system("cls");
            *isCustom = true;
            printf(YELLOW "  Image path: " RESET);
            scanf("%255s", pathBuffer);
            break;

        case 2:
            return 0;
    }

    return 1;
}

////////////////////////////////////////////////////

double getThreshold()
{
    double t;
    do {
        printf(YELLOW "  Threshold: " RESET);
        scanf("%lf", &t);
    } while(t < 0);

    return t;
}

int getMinBlockSize()
{
    int s;
    do {
        printf(YELLOW "  Min block size: " RESET);
        scanf("%d", &s);
    } while (s < 1);

    return s;
}

////////////////////////////////////////////////////

void compressToPPM()
{
    char imagePath[256];
    bool isCustom;
    if (!getImagePath(imagePath, &isCustom)){
        return;
    }
    double threshold = getThreshold();
    int minBlockSize = getMinBlockSize();

    char outputPath[256];

    if (!isCustom)
    {
        strcpy(outputPath, "assets/output.ppm");
    }
    else
    {
        char *fileName = strrchr(imagePath, '\\');

        if (fileName == NULL){
            fileName = imagePath;
        }
        else{
            fileName++; // move pointer from "\" to first letter from the name
        }

        char baseName[256];
        strcpy(baseName, fileName);

        char *extension = strrchr(baseName, '.');
        if (extension != NULL){ 
            *extension = '\0'; // "zdr.ppm" => "zdr"
        }

        sprintf(outputPath, "assets/%s_out.ppm", baseName);
    }

    Image input = readPPM(imagePath);
    QuadTree qTree = initQT(&input, threshold, minBlockSize);
    Image output = initImage(input.width, input.height);
    reconstructImageQT(&qTree, &output);
    writePPM(outputPath, &output);

    releaseImage(&input);
    releaseImage(&output);
    releaseQT(&qTree);

    printf(GREEN "\n  Saved to %s\n" RESET, outputPath);
    printf(DIM "\n  Press any key to continue..." RESET);
    _getch();
}

////////////////////////////////////////////////////

int main()
{
    const char *mainOpts[] = {
        "Compress to PPM",
        "Exit"};

    while (true)
    {
        int choice = runMenu("QuadTree Image Compressor", mainOpts, 2);
        switch(choice){
            case 0: compressToPPM(); break;
            case 1: system("cls"); return 0;
        }
    }

    return 0;
}
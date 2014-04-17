/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef LOADDOCUMENT_C
#define LOADDOCUMENT_C

#include <stdbool.h>
#include "libpngHelper.c"

#define LOADDOCUMENT_VERBOSE 1
#define SPLIT_THRESHHOLD 64
#define MAX_IMAGE_DEPTH 255

void findRows(int *imageVector, int imageWidth, int imageHeight, int imageDepth)
{
	int pixelIverse = 0;
	int currentPixel = 0;
	
	bool blankRow = true;
	int pixelRowBegin = 0;
	int pixelRowEnd = 0;
	int letterSpacing = 0;

	for (int i = 0; i < imageHeight; ++i) {
		for (int j = 0; j < imageWidth; ++j) {
			currentPixel = (i * imageWidth) + j;
			pixelIverse = imageDepth - imageVector[currentPixel];

			// store inverted pixel
			// assume text is normally black on white background,
			// and we want white text on black background so that
			// higher value coresponds with more information
			imageVector[currentPixel] = pixelIverse;
			
			if (pixelIverse > SPLIT_THRESHHOLD) {
				blankRow = false;
			}
		}

		if (!blankRow) {
			if (!pixelRowBegin) {
				pixelRowBegin = i;
			}
			pixelRowEnd = i;
			blankRow = true;
		} else {
			if (pixelRowEnd - pixelRowBegin) {
				printf("%d - %d\n", pixelRowBegin, pixelRowEnd);
				//
				// results, spacing = split_characters(text_rows, threshhold, row_index, spacing)
				// for result in results:
				// 	characters.append(result)
				// characters.append('\n')
				// text_rows = []
				// return characters
			}
			pixelRowBegin = 0;
			pixelRowEnd = 0;
		}
	}
}

void loadDocument(char *filename)
{
	int *imageVector;
	int imageWidth = 0;
	int imageHeight = 0;
	imageVector = readPNGFile(filename, &imageWidth, &imageHeight, LOADDOCUMENT_VERBOSE);
	if (imageVector && imageWidth && imageHeight) {
		findRows(imageVector, imageWidth, imageHeight, MAX_IMAGE_DEPTH);
	}
}

#endif

/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef OCRLIB_C
#define OCRLIB_C

#include <stdlib.h>
#include <math.h>
#include "imageDocument.c"
#include "resizeImage.c"

#define KLIMIT 64
#define DEBUG_SAVE_STANDARDIZED_CHARACTERS 0

struct OCRkit {
	double *eigenImageSpace;
	int dimensionality;
	
	char *characters;
	double *characterWeights;

	int *imageVector;
	int imageWidth;
	struct imageDocument *imageDoc;
};

struct OCRkit *newOCRkit()
{
	struct OCRkit *newKit;
	newKit = (struct OCRkit*)malloc(sizeof(struct OCRkit));

	newKit->eigenImageSpace = NULL;
	newKit->dimensionality = 0;
	
	newKit->characters = NULL;
	newKit->characterWeights = NULL;

	newKit->imageVector = NULL;
	newKit->imageWidth = 0;
	newKit->imageDoc = NULL;

	return newKit;
}

int standardizeImageMatrix(int *imageVector, int imageWidth, struct imageDocumentChar *imageDocChar, int **charImage)
{
	int width = imageDocChar->x2 - imageDocChar->x1;
	int height = imageDocChar->y2 - imageDocChar->y1;

	// ignore things, spaces, which do not need sizing
	if ((width == 0) || height == 0) {
		return 0;
	}

	int padding = 0;
	int paddingQ = 0;
	int paddingR = 0;
	int newWidth = width;
	int newHeight = height;

	int padLeft = 0;
	int padRight = newWidth;
	int padTop = 0;
	int padBottom = newHeight;

	// only if the area is rectangular
	// determine how to fix the shorter dimension
	if (height != width) {
		if (height > width) {
			padding = height - width;
			paddingQ = (int)round((padding / 2));
			paddingR = padding - (paddingQ * 2);
			newWidth = paddingQ + width + paddingQ + paddingR;

			padLeft = paddingQ;
			padRight = padLeft + width - 1;
		} else {
			padding = width - height;
			paddingQ = (int)round((padding / 2));
			paddingR = padding - (paddingQ * 2);
			newHeight = paddingQ + paddingR + height + paddingQ;

			padTop = paddingQ + paddingR;
			padBottom = padTop + height - 1;
		}
	}
	// create a working image
	int *tempImage = (int*)malloc(newWidth * newHeight * sizeof(int));
	memset(tempImage, 0, (newWidth * newHeight * sizeof(int)));

	int k = 0;
	int l = 0;
	int imagePixel = 0;
	int currentPixel = 0;
	for (int i = 0; i < newHeight; ++i) {
		if ((i >= padTop) && i <= padBottom) {
			k = 0;
			for (int j = 0; j < newWidth; ++j) {
				if ((j >= padLeft) && (j <= padRight)) {
					imagePixel = ((imageDocChar->y1 + l) * imageWidth) + (imageDocChar->x1 + k);
					currentPixel = (i * newWidth) + j;
					++k;
					
					tempImage[currentPixel] = imageVector[imagePixel];
				}
			}
			++l;
		}
	}

	int targetImageWidth = 16;
	int *resizedImage = (int*)malloc(targetImageWidth * targetImageWidth * sizeof(int));
	memset(resizedImage, 0, (targetImageWidth * targetImageWidth * sizeof(int)));
	resizeImage(tempImage, resizedImage, newWidth, newHeight, targetImageWidth, targetImageWidth);

	if (DEBUG_SAVE_STANDARDIZED_CHARACTERS) {
		char fName[100] = "./tst/tst-1-";
		char buffer[16];
		snprintf(buffer, sizeof(buffer), "%d-%d.png", imageDocChar->y1, imageDocChar->x1);
		strcat(fName, buffer);
		write_png_file(resizedImage, targetImageWidth, targetImageWidth, fName);
	}

	*charImage = resizedImage;
	return 1;
}

void projectCandidate(int *charImageVector, struct OCRkit *ocrKit)
{
	double *tempWeights = (double*)malloc(KLIMIT * sizeof(double));
	memset(tempWeights, 0, (KLIMIT * sizeof(double)));

	for (int i = 0; i < KLIMIT; ++i) {
		/* code */
	}
}

void ocrCharacter(struct OCRkit *ocrKit, struct imageDocumentChar *imageDocChar)
{
	if (imageDocChar) {
		printf("%c", imageDocChar->value);
		int *charImage;
		int standardizeOk = standardizeImageMatrix(ocrKit->imageVector, ocrKit->imageWidth, imageDocChar, &charImage);

		projectCandidate(charImage, ocrKit);

		if (standardizeOk) {
			free(charImage);
		}
	}
}

void ocrCharLoop(struct OCRkit *ocrKit, struct imageDocumentLine *imageDocLine)
{
	if (imageDocLine) {
		if (imageDocLine->characters) {
			struct imageDocumentChar *currentChar = imageDocLine->characters;
			struct imageDocumentChar *nextChar;

			ocrCharacter(ocrKit, currentChar);

			while (currentChar->nextChar) {
				nextChar = currentChar->nextChar;
				currentChar = nextChar;

				ocrCharacter(ocrKit, currentChar);
			}

			freeImageDocumentChar(nextChar);
		}
	}
}

void ocrLineLoop(struct OCRkit *ocrKit)
{
	struct imageDocument *imageDoc = ocrKit->imageDoc;
	if (imageDoc) {
		if (imageDoc->lines) {
			struct imageDocumentLine *currentLine = imageDoc->lines;
			struct imageDocumentLine *nextLine;

			ocrCharLoop(ocrKit, currentLine);

			while (currentLine->nextLine) {
				nextLine = currentLine->nextLine;
				currentLine = nextLine;

				ocrCharLoop(ocrKit, currentLine);
			}

			freeImageDocumentLine(nextLine);
		}
	}
}

void startOcr(struct OCRkit *ocrKit)
{
	ocrLineLoop(ocrKit);
}

#endif

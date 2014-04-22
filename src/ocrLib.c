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

#define STANDARD_IMAGE_SIDE 16
#define DEBUG_SAVE_STANDARDIZED_CHARACTERS 1
#define DEBUG_PRINT_STANDARDIZED_CHARACTERS 0

struct OCRkit {
	int klimit;
	int dimensionality;
	double *eigenImageSpace;
	
	char *characters;
	int characterCount;
	double *characterWeights;

	int *imageVector;
	int imageWidth;
	struct imageDocument *imageDoc;
};

struct OCRkit *newOCRkit()
{
	struct OCRkit *newKit;
	newKit = (struct OCRkit*)malloc(sizeof(struct OCRkit));

	newKit->klimit = 0;
	newKit->dimensionality = 0;
	newKit->eigenImageSpace = NULL;
	
	newKit->characters = NULL;
	newKit->characterCount = 0;
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

	int targetImageWidth = STANDARD_IMAGE_SIDE;
	int *resizedImage = (int*)malloc(targetImageWidth * targetImageWidth * sizeof(int));
	memset(resizedImage, 0, (targetImageWidth * targetImageWidth * sizeof(int)));
	sizeSquareImage(tempImage, resizedImage, newWidth, targetImageWidth);
	
	if (DEBUG_SAVE_STANDARDIZED_CHARACTERS) {
		char fName[100] = "./tst/tst-1-";
		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%d-%d.png", imageDocChar->y1, imageDocChar->x1);
		strcat(fName, buffer);
		write_png_file(resizedImage, targetImageWidth, targetImageWidth, fName);
		// write_png_file(tempImage, newWidth, newHeight, fName);
	}

	*charImage = resizedImage;
	if (DEBUG_PRINT_STANDARDIZED_CHARACTERS) {
		for (int i = 0; i < (STANDARD_IMAGE_SIDE*STANDARD_IMAGE_SIDE); ++i) {
			printf("%3d ", resizedImage[i]);
			if (!(i % STANDARD_IMAGE_SIDE)) {
				printf("\n");
			}
			if (!(i % (STANDARD_IMAGE_SIDE*STANDARD_IMAGE_SIDE))) {
				printf("\n");
			}
		}
	}
	return 1;
}

double *projectCandidate(int *charImageVector, struct OCRkit *ocrKit)
{
	int klimit = round(ocrKit->klimit / 4);
	double *tempWeights = (double*)malloc(klimit * sizeof(double));
	memset(tempWeights, 0, (klimit * sizeof(double)));

	double *eigenImageSpace = ocrKit->eigenImageSpace;
	int dimensionality = ocrKit->dimensionality;

	int currentEigen = 0;
	double weight = 0;
	for (int i = 0; i < klimit; ++i) {
		weight = 0;
		for (int j = 0; j < (STANDARD_IMAGE_SIDE * STANDARD_IMAGE_SIDE); ++j) {
			currentEigen = (i * dimensionality) + j;
			weight += (charImageVector[j] * eigenImageSpace[currentEigen]);
		}
		tempWeights[i] = weight;
	}
	return tempWeights;
}

char nearestNeighborCPU(struct OCRkit *ocrKit, double *questionWeights)
{
	int klimit = round(ocrKit->klimit / 4);
	int dimensionality = ocrKit->dimensionality;
	int characterCount = ocrKit->characterCount;
	double *characterWeights = ocrKit->characterWeights;
	char *characters = ocrKit->characters;

	int charWeightIndex = 0;
	double numerator = 0;
	double denominatorA = 0;
	double denominatorB = 0;
	double totalScore = 0;
	double maxScore = -999999;
	char answer = '~';

	for (int i = 0; i < characterCount; ++i) {
		numerator = 0;
		denominatorA = 0;
		denominatorB = 0;
		for (int j = 0; j < klimit; ++j) {
			// TODO: verify correct index is being used here
			charWeightIndex = (i * (dimensionality-1)) + j;
			numerator += questionWeights[j] * characterWeights[charWeightIndex];
			denominatorA += questionWeights[j] * questionWeights[j];
			denominatorB += characterWeights[charWeightIndex] * characterWeights[charWeightIndex];
			if (denominatorA && denominatorB) {
				totalScore = numerator / (sqrt(denominatorA) * sqrt(denominatorB));
			} else {
				totalScore = 0;
			}
		}
		// printf("%d %c: %2.16f -- %f => %f\n", i, characters[i], characterWeights[charWeightIndex], totalScore, maxScore);

		if (totalScore > maxScore) {
			maxScore = totalScore;
			answer = characters[i];
		}
	}
	return answer;
}

void ocrCharacter(struct OCRkit *ocrKit, struct imageDocumentChar *imageDocChar)
{
	if (imageDocChar) {
		//printf("%c", imageDocChar->value);
		int *charImage;
		int standardizeOk = standardizeImageMatrix(ocrKit->imageVector, ocrKit->imageWidth, imageDocChar, &charImage);
		if (standardizeOk) {
			double *weights = projectCandidate(charImage, ocrKit);
			free(charImage);

			char answer = nearestNeighborCPU(ocrKit, weights);
			printf("%c", answer);
		} else {
			printf("%c", imageDocChar->value);
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

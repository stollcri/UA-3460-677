/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef OCRKIT_C
#define OCRKIT_C

#include <stdlib.h>

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

static struct OCRkit *newOCRkit()
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

#endif

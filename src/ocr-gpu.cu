/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#include <unistd.h>
#include "loadKnowledge.c"
#include "imageDocument.c"
#include "loadDocument.c"
#include "ocrKit.c"
#include "knn.cu"
#include "ocrLib.c"

#define DEFAULT_DIMENSIONALITY 256

void runOcr(char *targetfile, int peserveNewline)
{
	int klimit = 0;
	int dimensionality = 0;
	double *eigenImageSpace;
	loadEigenspace("./dat/eigenspace", &eigenImageSpace, &klimit, &dimensionality);
	
	char *characters;
	int characterCount = 0;
	double *characterWeights;
	characterCount = loadCharacters("./dat/characters", dimensionality, &characters, &characterWeights);

	int *imageVector;
	int imageWidth = 0;
	int imageHeight = 0;
	imageVector = loadDocument(targetfile, &imageWidth, &imageHeight);
	if (!imageVector || !imageWidth || !imageHeight) {
		printf("Error loading PNG text image.\n");
		exit(1);
	}

	struct imageDocument *imageDoc;
	imageDoc = processDocument(imageVector, imageWidth, imageHeight);
	if (!imageDoc) {
		printf("Error processing PNG text image.\n");
		exit(1);
	}

	struct OCRkit *ocrKit;
	ocrKit = newOCRkit();
	ocrKit->klimit = klimit;
	ocrKit->dimensionality = dimensionality;
	ocrKit->eigenImageSpace = eigenImageSpace;
	ocrKit->characters = characters;
	ocrKit->characterCount = characterCount;
	ocrKit->characterWeights = characterWeights;
	ocrKit->imageVector = imageVector;
	ocrKit->imageWidth = imageWidth;
	ocrKit->imageDoc = imageDoc;
	startOcr(ocrKit);

	printDocument(imageDoc, peserveNewline);
}

int main(int argc, char const *argv[])
{
	if (argc > 1) {
		char *filename = (char*)argv[1];
		if (access(filename, R_OK) != -1) {
			runOcr(filename, 1);
		} else {
			printf("Error reading file %s\n", argv[1]);
		}
	} else {
		printf("Usage:\n");
		printf(" %s filename\n", argv[0]);
	}
}

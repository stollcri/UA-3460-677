/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#include "loadKnowledge.c"
#include "imageDocument.c"
#include "loadDocument.c"
#include "ocrLib.c"

int main(int argc, char const *argv[])
{
	int dimensionality = 0;
	double *eigenImageSpace;
	dimensionality = loadEigenspace("./dat/eigenspace", &eigenImageSpace);
	
	char *characters;
	double *characterWeights;
	loadCharacters("./dat/characters", &characters, &characterWeights);

	int *imageVector;
	int imageWidth = 0;
	int imageHeight = 0;
	imageVector = loadDocument("./tst/RightsOfManB.png", &imageWidth, &imageHeight);
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
	ocrKit->eigenImageSpace = eigenImageSpace;
	ocrKit->dimensionality = dimensionality;
	ocrKit->characters = characters;
	ocrKit->characterWeights = characterWeights;
	ocrKit->imageVector = imageVector;
	ocrKit->imageWidth = imageWidth;
	ocrKit->imageDoc = imageDoc;
	startOcr(ocrKit);
}

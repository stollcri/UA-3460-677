/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef IMAGEDOCUMENT_C
#define IMAGEDOCUMENT_C

#include <stdlib.h>
 
struct imageDocumentChar {
	int x1;
	int y1;
	int x2;
	int y2;
	char value;
	//char pad[7]; // unused memory padding
	double *weights;
	struct imageDocumentChar *nextChar;
	struct imageDocumentChar *lastChar;
};

struct imageDocumentLine {
	struct imageDocumentChar *characters;
	struct imageDocumentLine *nextLine;
	struct imageDocumentLine *lastLine;
};

struct imageDocument {
	int totalWeightCount;
	struct imageDocumentLine *lines;
};


static struct imageDocument *newImageDocument()
{
	struct imageDocument *newDocument;
	newDocument = (struct imageDocument*)malloc(sizeof(struct imageDocument));

	newDocument->totalWeightCount = 0;
	newDocument->lines = NULL;

	return newDocument;
}

static struct imageDocumentLine *newImageDocumentLine()
{
	struct imageDocumentLine *newDocumentLine;
	newDocumentLine = (struct imageDocumentLine*)malloc(sizeof(struct imageDocumentLine));

	newDocumentLine->characters = NULL;
	newDocumentLine->nextLine = NULL;
	newDocumentLine->lastLine = NULL;
	
	return newDocumentLine;
}

static struct imageDocumentChar *newImageDocumentChar(int x1, int y1, int x2, int y2, char value)
{
	struct imageDocumentChar *newDocumentChar;
	newDocumentChar = (struct imageDocumentChar*)malloc(sizeof(struct imageDocumentChar));

	newDocumentChar->x1 = x1;
	newDocumentChar->y1 = y1;
	newDocumentChar->x2 = x2;
	newDocumentChar->y2 = y2;
	newDocumentChar->value = value;
	newDocumentChar->weights = NULL;
	newDocumentChar->nextChar = NULL;
	newDocumentChar->lastChar = NULL;

	return newDocumentChar;
}

// static struct imageDocumentChar *newImageDocumentCharBlank()
// {
// 	return newImageDocumentChar(0, 0, 0, 0, ' ');
// }

// static void freeImageDocument(struct imageDocument *targetDocument)
// {
// 	if (targetDocument) {
// 		// TODO: free the memory
// 	}
// }

static void freeImageDocumentLine(struct imageDocumentLine *targetLine)
{
	if (targetLine) {
		// TODO: free the memory
	}
}

static void freeImageDocumentChar(struct imageDocumentChar *targetChar)
{
	if (targetChar) {
		// TODO: free the memory
	}
}


static void addLineToDocument(struct imageDocument *targetDocument, struct imageDocumentLine *sourceLine)
{
	if (targetDocument && sourceLine) {
		if (targetDocument->lines) {
			struct imageDocumentLine *currentLine = targetDocument->lines;
			struct imageDocumentLine *nextLine = NULL;
			
			while (currentLine->nextLine) {
				nextLine = currentLine->nextLine;
				currentLine = nextLine;
			}
			currentLine->nextLine = sourceLine;
			currentLine->lastLine = sourceLine;

			freeImageDocumentLine(nextLine);
		} else {
			targetDocument->lines = sourceLine;
		}
	}
}

static void addCharToLine(struct imageDocumentLine *targetLine, struct imageDocumentChar *sourceChar)
{
	if (targetLine && sourceChar) {
		if (targetLine->characters) {
			struct imageDocumentChar *currentChar = targetLine->characters;
			struct imageDocumentChar *nextChar = NULL;

			while (currentChar->nextChar) {
				nextChar = currentChar->nextChar;
				currentChar = nextChar;
			}
			currentChar->nextChar = sourceChar;
			currentChar->lastChar = sourceChar;

			freeImageDocumentChar(nextChar);
		} else {
			targetLine->characters = sourceChar;
		}
	}
}

static void printDocument(struct imageDocument *targetDocument, int preserveNewLine)
{
	if (targetDocument) {
		if (targetDocument->lines) {
			struct imageDocumentLine *currentLine = targetDocument->lines;
			struct imageDocumentLine *nextLine = NULL;

			struct imageDocumentChar *currentChar = NULL;
			struct imageDocumentChar *nextChar = NULL;
			
			while (currentLine) {
				currentChar = currentLine->characters;

				while (currentChar->nextChar) {
					printf("%c", currentChar->value);

					nextChar = currentChar->nextChar;
					currentChar = nextChar;
				}
				if (preserveNewLine) {
					printf("%c", currentChar->value);
				}

				nextLine = currentLine->nextLine;
				currentLine = nextLine;
			}

			freeImageDocumentChar(nextChar);
			freeImageDocumentChar(currentChar);
			freeImageDocumentLine(nextLine);
			freeImageDocumentLine(currentLine);
		}
	}
}


double *vectorizeWeights(struct imageDocument *targetDocument)
{
	if (targetDocument) {
		if (targetDocument->lines) {
			struct imageDocumentLine *currentLine = targetDocument->lines;
			struct imageDocumentLine *nextLine = NULL;

			struct imageDocumentChar *currentChar = NULL;
			struct imageDocumentChar *nextChar = NULL;
			
			int numberOfWeights = 0;
			while (currentLine) {
				currentChar = currentLine->characters;
				while (currentChar->nextChar) {
					++numberOfWeights;
					nextChar = currentChar->nextChar;
					currentChar = nextChar;
				}
				nextLine = currentLine->nextLine;
				currentLine = nextLine;
			}

			double *tempWeights = (double*)malloc((unsigned long)numberOfWeights * sizeof(double));
			memset(tempWeights, 0, ((unsigned long)numberOfWeights * sizeof(double)));

			double *allWeights = (double*)malloc((unsigned long)numberOfWeights * klimit * sizeof(double));
			memset(allWeights, 0, ((unsigned long)numberOfWeights * klimit * sizeof(double)));

			int currentWeight = 0;
			while (currentLine) {
				currentChar = currentLine->characters;
				while (currentChar->nextChar) {
					if (currentChar->weights) {
						tempWeights = currentChar->weights;
						for (int i = 0; i < klimit; ++i) {
							allWeights[currentWeight] = tempWeights[i];
							++currentWeight;
						}
					} else {
						currentWeight += klimit;
					}
					
					nextChar = currentChar->nextChar;
					currentChar = nextChar;
				}
				nextLine = currentLine->nextLine;
				currentLine = nextLine;
			}

			freeImageDocumentChar(nextChar);
			freeImageDocumentChar(currentChar);
			freeImageDocumentLine(nextLine);
			freeImageDocumentLine(currentLine);

			targetDocument->totalWeightCount = numberOfWeights * klimit;
			return allWeights;
		}
		return NULL;
	}
	return NULL;
}

#endif

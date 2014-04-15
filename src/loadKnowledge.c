/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef LOADKNOWLEDGE_C
#define LOADKNOWLEDGE_C

#include <stdio.h>

void readEigenspaceFromFile(char *filename)
{
	int klimit = 0;
	int dimensions = 0;

	FILE *inFile;
	inFile = fopen(filename, "r");
	if (inFile) {
		// read header
		fread(&klimit, sizeof(int), 1, inFile);
		fread(&dimensions, sizeof(int), 1, inFile);
		// printf("klimit: %i \n", klimit);
		// printf("dimensions: %i \n", dimensions);

		// read eigenimagespace (doubles)
		for (int i = 0; i < klimit; ++i) {
			for (int j = 0; j < dimensions; ++j) {
				/* code */
			}
		}

		// read means (doubles)
		for (int i = 0; i < klimit; ++i) {
			/* code */
		}

		// read eigenvalues (doubles)
		for (int i = 0; i < klimit; ++i) {
			/* code */
		}
	}
	fclose(inFile);
}

void readCharactersFromFile(char *filename)
{
	int characterCount = 0;

	FILE *inFile;
	inFile = fopen(filename, "r");
	if (inFile) {
		// read header
		fread(&characterCount, sizeof(int), 1, inFile);
		// printf("characterCount: %i \n", characterCount);

		// read characters (chars)
		char currentChar = 0;
		for (int i = 0; i < characterCount; ++i) {
			fread(&currentChar, sizeof(char), 1, inFile);
			// printf("%c ", currentChar);
		}
		// printf("\n");

		// read weights (doubles)
		double currentWeight = 0;
		for (int i = 0; i < characterCount; ++i) {
			fread(&currentWeight, sizeof(double), 1, inFile);
			// printf("%2.16f ", currentWeight);
		}
		// printf("\n");
	}
	fclose(inFile);
}

void loadKnowledge()
{
	readEigenspaceFromFile("./dat/eigenspace");
	readCharactersFromFile("./dat/characters");
}

#endif

/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef LOADKNOWLEDGE_C
#define LOADKNOWLEDGE_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct eigenimagespace {
	int sampleCount;
	int dimensionCount;
	double *eigenspace;
	double *eigenvalues;
};

struct charactersspace {
	char *characters;
	double *weights;
};

int readEigenspaceFromFile(char *filename, double **eigenimagespace)
{
	int klimit = 0;
	int dimensions = 0;

	FILE *inFile;
	inFile = fopen(filename, "r");
	if (inFile) {
		// read header
		fread(&klimit, sizeof(int), 1, inFile);
		fread(&dimensions, sizeof(int), 1, inFile);

		double *tempEigenSpace = (double*)malloc(klimit * dimensions * sizeof(double));
		memset(tempEigenSpace, 0, (klimit * dimensions * sizeof(double)));

		// read eigenimagespace (doubles)
		int k = 0;
		double currentEigen = 0;
		for (int i = 0; i < klimit; ++i) {
			for (int j = 0; j < dimensions; ++j) {
				fread(&currentEigen, sizeof(double), 1, inFile);
				tempEigenSpace[k] = currentEigen;
				++k;
			}
		}

		*eigenimagespace = tempEigenSpace;

		/*
		// read means (doubles)
		double currentMean = 0;
		for (int i = 0; i < klimit; ++i) {
			fread(&currentMean, sizeof(double), 1, inFile);
			// we are not using them, discard
		}

		double *tempEigenValues = (double*)malloc(klimit * sizeof(double));
		memset(tempEigenValues, 0, (klimit * sizeof(double)));

		// read eigenvalues (doubles)
		double currentEigenValue = 0;
		for (int i = 0; i < klimit; ++i) {
			fread(&currentEigenValue, sizeof(double), 1, inFile);
			tempEigenSpace[k] = currentEigen;
		}
		*/
	}
	fclose(inFile);
	return dimensions;
}

int readCharactersFromFile(char *filename, int dimensionality, char **characters, double **characterWeights)
{
	int characterCount = 0;

	FILE *inFile;
	inFile = fopen(filename, "r");
	if (inFile) {
		// read header
		fread(&characterCount, sizeof(int), 1, inFile);

		char *tempCharacters = (char*)malloc(characterCount * sizeof(char));
		memset(tempCharacters, 0, (characterCount * sizeof(char)));

		// read characters (chars)
		int j = 0;
		char currentChar = 0;
		for (int i = 0; i < characterCount; ++i) {
			fread(&currentChar, sizeof(char), 1, inFile);
			tempCharacters[j] = currentChar;
			++j;
		}

		*characters = tempCharacters;


		double *tempCharacterWeights = (double*)malloc(characterCount * dimensionality * sizeof(double));
		memset(tempCharacterWeights, 0, (characterCount * dimensionality * sizeof(double)));

		// read weights (doubles)
		j = 0;
		double currentWeight = 0;
		for (int i = 0; i < (characterCount * (dimensionality-1)); ++i) {
			fread(&currentWeight, sizeof(double), 1, inFile);
			tempCharacterWeights[j] = currentWeight;
			++j;
		}

		*characterWeights = tempCharacterWeights;
	}
	fclose(inFile);
	return characterCount;
}

int loadEigenspace(char *eigenspaceFile, double **eigenspace)
{
	int dimensionality = 0;
	double *tempEigenspace;
	dimensionality = readEigenspaceFromFile(eigenspaceFile, &tempEigenspace);
	*eigenspace = tempEigenspace;
	return dimensionality;
}

int loadCharacters(char *charactersFile, int dimensionality, char **characters, double **characterWeights)
{
	int characterCount = 0;
	char *tempCharacters;
	double *tempCharacterWeights;
	characterCount = readCharactersFromFile(charactersFile, dimensionality, &tempCharacters, &tempCharacterWeights);
	*characters = tempCharacters;
	*characterWeights = tempCharacterWeights;
	return characterCount;
}

#endif

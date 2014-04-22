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

void readEigenspaceFromFile(char *filename, double **eigenimagespace, int *klimit, int *dimensions)
{
	int tmpKlim = 0;
	int tmpDims = 0;

	FILE *inFile;
	inFile = fopen(filename, "r");
	if (inFile) {
		// read header
		fread(&tmpKlim, sizeof(int), 1, inFile);
		fread(&tmpDims, sizeof(int), 1, inFile);

		double *tempEigenSpace = (double*)malloc(tmpKlim * tmpDims * sizeof(double));
		memset(tempEigenSpace, 0, (tmpKlim * tmpDims * sizeof(double)));

		// read eigenimagespace (doubles)
		int k = 0;
		double currentEigen = 0;
		for (int i = 0; i < tmpKlim; ++i) {
			for (int j = 0; j < tmpDims; ++j) {
				fread(&currentEigen, sizeof(double), 1, inFile);
				tempEigenSpace[k] = currentEigen;
				++k;
			}
		}

		*eigenimagespace = tempEigenSpace;
		*klimit = tmpKlim;
		*dimensions = tmpDims;

		/*
		// read means (doubles)
		double currentMean = 0;
		for (int i = 0; i < tmpKlim; ++i) {
			fread(&currentMean, sizeof(double), 1, inFile);
			// we are not using them, discard
		}

		double *tempEigenValues = (double*)malloc(klimit * sizeof(double));
		memset(tempEigenValues, 0, (klimit * sizeof(double)));

		// read eigenvalues (doubles)
		double currentEigenValue = 0;
		for (int i = 0; i < tmpKlim; ++i) {
			fread(&currentEigenValue, sizeof(double), 1, inFile);
			tempEigenSpace[k] = currentEigen;
		}
		*/
	}
	fclose(inFile);
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

void loadEigenspace(char *eigenspaceFile, double **eigenspace, int *klimit, int *dimensionality)
{
	int tmpKlim = 0;
	int tmpDims = 0;
	double *tempEigenspace;
	readEigenspaceFromFile(eigenspaceFile, &tempEigenspace, &tmpKlim, &tmpDims);
	*klimit = tmpKlim;
	*dimensionality = tmpDims;
	*eigenspace = tempEigenspace;
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

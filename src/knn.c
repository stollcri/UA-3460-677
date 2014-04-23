/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef KNN_C
#define KNN_C

#include "ocrKit.c"
#include <sys/time.h>

#define DEBUG_PRINT_TIME 1

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
		}
		if (denominatorA && denominatorB) {
			totalScore = numerator / (sqrt(denominatorA) * sqrt(denominatorB));
		} else {
			totalScore = 0;
		}
		// printf("%d %c: %2.16f -- %f => %f\n", i, characters[i], characterWeights[charWeightIndex], totalScore, maxScore);

		if (totalScore > maxScore) {
			maxScore = totalScore;
			answer = characters[i];
		}
	}
	return answer;
}

char nearestNeighbor(struct OCRkit *ocrKit, double *questionWeights)
{
	struct timeval stop, start;
	char answer = '?';

	gettimeofday(&start, NULL);
	answer = nearestNeighborCPU(ocrKit, questionWeights);
	gettimeofday(&stop, NULL);

	if (DEBUG_PRINT_TIME) {
		printf("Time: %u us \n", stop.tv_usec - start.tv_usec);
	}

	return answer;
}

#endif

/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef KNN_C
#define KNN_C

#include "ocrKit.c"
#include <sys/time.h>
#include <cutil.h>

#define TRAINING_SET_SIZE = 78
#define DEBUG_PRINT_TIME 1

__global__ void nearestNeighborGPUa(int g_klimit, int g_dimensionality, double *g_charWeights, double *g_qWeights, double *g_scores)
{
	extern __shared__ double s_qWeights[];
	int idx = (blockIdx.x * blockDim.x) + threadIdx.x;

	// load shared memory
	if (idx < g_klimit) {
		s_qWeights[idx] = g_qWeights[idx];
	}
	__syncthreads();

	// set some rgisters
	int charWeightIndex = 0;
	int charWeightIndexPart = (idx * (g_dimensionality-1));
	double numerator = 0;
	double denominatorA = 0;
	double denominatorB = 0;
	double totalScore = 0;

	// calulate the cosine similarity
	for (int j = 0; j < g_klimit; ++j) {
		charWeightIndex = charWeightIndexPart + j;

		numerator += s_qWeights[j] * g_charWeights[charWeightIndex];
		denominatorA += s_qWeights[j] * s_qWeights[j];
		denominatorB += g_charWeights[charWeightIndex] * g_charWeights[charWeightIndex];
	}

	if (denominatorA && denominatorB) {
		totalScore = numerator / (sqrt(denominatorA) * sqrt(denominatorB));
	}

	__syncthreads(); 

	// save cosine similarity score
	g_scores[idx] = totalScore;
}

static char launchNearestNeighborA(struct OCRkit *ocrKit, double *questionWeights)
{
	// gather basic information
	int characterCount = ocrKit->characterCount;
	int klimit = (ocrKit->klimit / 4);
	int dimensionality = ocrKit->dimensionality;
	// We are using a static declaration to get more shared memory
	// A pitfall is if the eigen data is updated to include more characters
	// This program must be changed and recompiled
	// if (characterCount != TRAINING_SET_SIZE) {
	// 	printf("WARNING: Character set size %d differs from expected %d\n", characterCount, TRAINING_SET_SIZE);
	// }

	// allocate memory for character weights
	int cWeightMemSize = klimit * dimensionality * sizeof(double);
	double *d_charWeights = NULL;
	CUDA_SAFE_CALL(cudaMalloc((void**)&d_charWeights, cWeightMemSize));
	CUDA_SAFE_CALL(cudaMemcpy(d_charWeights, ocrKit->characterWeights, cWeightMemSize, cudaMemcpyHostToDevice));

	// allocate memory for candidate weights
	int qWeightMemSize = dimensionality * sizeof(double);
	double *d_qWeights = NULL;
	CUDA_SAFE_CALL(cudaMalloc((void**)&d_qWeights, qWeightMemSize));
	CUDA_SAFE_CALL(cudaMemcpy(d_qWeights, questionWeights, qWeightMemSize, cudaMemcpyHostToDevice));

	// allocate memory for scores
	int scoreMemSize = characterCount * sizeof(double);
	double *h_scores = (double*)malloc(scoreMemSize);
	memset(h_scores, 0, characterCount * sizeof(double));
	double *d_scores = NULL;
	CUDA_SAFE_CALL(cudaMalloc((void**)&d_scores, scoreMemSize));
	CUDA_SAFE_CALL(cudaMemcpy(d_scores, h_scores, scoreMemSize, cudaMemcpyHostToDevice));

	// set up parallel dimensions
	int threadsPerBlock = characterCount;
	int blocksPerGrid = 1;
	dim3 dimGrid(blocksPerGrid);
	dim3 dimBlock(threadsPerBlock);
	int sharedMemSize = klimit * sizeof(double);

	// run the kernel
	nearestNeighborGPUa<<< dimGrid,dimBlock,sharedMemSize >>>(klimit, dimensionality, d_charWeights, d_qWeights, d_scores);
	cudaThreadSynchronize();

	// get the scores
	CUDA_SAFE_CALL(cudaMemcpy(h_scores, d_scores, scoreMemSize, cudaMemcpyDeviceToHost));

	char answer = '?';
	int maxScore = -999;
	for (int i = 0; i < characterCount; ++i) {
		if (h_scores[i] > maxScore) {
			maxScore = h_scores[i];
			answer = ocrKit->characters[i];
		}
	}
	return answer;
}

static char nearestNeighbor(struct OCRkit *ocrKit, double *questionWeights)
{
	struct timeval stop, start;
	char answer = '?';

	gettimeofday(&start, NULL);
	//answer = nearestNeighborCPU(ocrKit, questionWeights);
	answer = launchNearestNeighborA(ocrKit, questionWeights);
	gettimeofday(&stop, NULL);

	if (DEBUG_PRINT_TIME) {
		printf("Time: %u us (%c)\n", (unsigned int)(stop.tv_usec - start.tv_usec), answer);
	}

	return answer;
}

#endif

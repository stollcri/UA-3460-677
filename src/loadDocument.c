/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef LOADDOCUMENT_C
#define LOADDOCUMENT_C

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "libpngHelper.c"
#include "imageDocument.c"

#define LOADDOCUMENT_VERBOSE 1
#define SPLIT_THRESHHOLD 128
#define MAX_IMAGE_DEPTH 255

#define DEBUG_SAVE_INDIVIDUAL_CHARACTERS 0

struct imageDocumentLine *findCharacters(int *imageVector, int imageWidth, int rowBegin, int rowEnd, int *spacing)
{
	struct imageDocumentLine *currentLine = newImageDocumentLine();
	struct imageDocumentChar *newchar;
	
	// set a space width estimate
	int charSpacing = *spacing;
	if (!charSpacing) {
		charSpacing = 2 + (int)round(imageWidth * .005);
		*spacing = charSpacing;
	}
	int maxCharSpacing = (charSpacing * 2) + 2;

	int currentPixel = 0;
	bool blankCol = true;
	int spaceCols = 0;
	int pixelCols = 0;
	int pixelColsBegin = 0;
	int pixelColsEnd = 0;

	int finalX1 = 0;
	int finalY1 = 0;
	int finalX2 = 0;
	int finalY2 = 0;

	// examine each column to find places to split
	for (int j = 0; j < imageWidth; ++j) {
		// check if the column is empty
		for (int i = rowBegin; i < rowEnd; ++i) {
			currentPixel = (i * imageWidth) + j;
			if (imageVector[currentPixel] > SPLIT_THRESHHOLD) {
				blankCol = false;
			}
		}

		// Process columns which contain data
		if (!blankCol) {
			// blank area larger than a single space
			if (spaceCols > maxCharSpacing) {
				//

			// blanks area about the size of a space
			} else if (spaceCols > charSpacing) {
				newchar = newImageDocumentChar(0, 0, 0, 0, ' ');
				addCharToLine(currentLine, newchar);
			}

			pixelColsEnd = j;
			blankCol = true;
			spaceCols = 0;
			++pixelCols;

		// Process past data columns
		// TODO: FIX: a data column on the right hand side will be ignored
		} else {
			// update our space width estimation
			if (pixelCols > 2) {
				charSpacing = (int)round((charSpacing + (pixelCols / 3)) / 2) + 1;
				*spacing = charSpacing;
			}
			++spaceCols;

			// data to process
			if (pixelColsEnd) {
				// calculate character bounds within the image
				finalX1 = (pixelColsEnd-pixelCols);
				finalY1 = rowBegin-1;
				finalX2 = pixelColsEnd+2;
				finalY2 = rowEnd+2;

				// remove blank space from the top
				int padTop = 0;
				bool charLine = false;
				for (int k = finalY1; k < finalY2; ++k) {
					for (int l = finalX1; l < finalX2; ++l) {
						currentPixel = (k * imageWidth) + l;
						if (imageVector[currentPixel] > SPLIT_THRESHHOLD) {
							charLine = true;
							break;
						}
					}
					if (charLine && !padTop) {
						padTop = (k - 1) - finalY1;
						break;
					}
				}
				if (padTop > 1) {
					finalY1 += padTop;
				}
				
				// remove blank space from the bottom
				int padBottom = 0;
				charLine = false;
				for (int k = (finalY2 - 1); k > finalY1; --k) {
					for (int l = finalX1; l < finalX2; ++l) {
						currentPixel = (k * imageWidth) + l;
						if (imageVector[currentPixel] > SPLIT_THRESHHOLD) {
							charLine = true;
							break;
						}
					}
					if (charLine && !padBottom) {
						padBottom = finalY2 - (k + 2);
						break;
					}
				}
				if (padBottom > 1) {
					finalY2 -= padBottom;
				}

				// store the newly found character
				newchar = newImageDocumentChar(finalX1, finalY1, finalX2, finalY2, '?');
				addCharToLine(currentLine, newchar);
				
				// to visually see what is being stored
				if (DEBUG_SAVE_INDIVIDUAL_CHARACTERS) {
					int wdth = finalX2 - finalX1;
					int high = finalY2 - finalY1;

					char fName[100] = "./tst/tst_0-";
					char buffer[16];
					snprintf(buffer, sizeof(buffer), "%d-%d-%d.png", rowBegin, finalX1, finalX2);
					strcat(fName, buffer);
					// printf(" %d,%d = %s\n", wdth, high, fName);
					int k = 0;
					int *charImageVector = (int*)malloc(wdth * high * sizeof(int));
					for (int i = finalY1; i < finalY2; ++i) {
						for (int j = finalX1; j < finalX2; ++j) {
							currentPixel = (i * imageWidth) + j;
							charImageVector[k] = imageVector[currentPixel];
							++k;
						}
					}
					write_png_file(charImageVector, wdth, high, fName);
				}
			}

			pixelCols = 0;
			pixelColsEnd = 0;
		}
	}
	return currentLine;
}

struct imageDocument *findRows(int *imageVector, int imageWidth, int imageHeight, int imageDepth)
{
	int pixelIverse = 0;
	int currentPixel = 0;
	
	bool blankRow = true;
	int pixelRowBegin = 0;
	int pixelRowEnd = 0;

	int letterSpacing = 0;
	struct imageDocument *currentImageDoc = newImageDocument();
	struct imageDocumentLine *newline;
	struct imageDocumentChar *newchar;

	// examine each row to find places to split
	for (int i = 0; i < imageHeight; ++i) {
		// check if a row is empty
		for (int j = 0; j < imageWidth; ++j) {
			currentPixel = (i * imageWidth) + j;
			pixelIverse = imageDepth - imageVector[currentPixel];

			// store inverted pixel
			// assume text is normally black on white background,
			// and we want white text on black background so that
			// higher value coresponds with more information
			imageVector[currentPixel] = pixelIverse;
			
			if (pixelIverse > SPLIT_THRESHHOLD) {
				blankRow = false;
			}
		}

		// Process rows which contain data
		if (!blankRow) {
			if (!pixelRowBegin) {
				pixelRowBegin = i;
			}
			pixelRowEnd = i;
			blankRow = true;

		// Process past data rows
		// TODO: FIX: a data row on the bottom will be ignored
		} else {
			if (pixelRowEnd - pixelRowBegin) {
				newline = findCharacters(imageVector, imageWidth, pixelRowBegin, pixelRowEnd, &letterSpacing);
				newchar = newImageDocumentChar(0, 0, 0, 0, '\n');
				addCharToLine(newline, newchar);
				addLineToDocument(currentImageDoc, newline);
			}
			pixelRowBegin = 0;
			pixelRowEnd = 0;
		}
	}
	return currentImageDoc;
}

int *loadDocument(char *filename, int *imageWidth, int *imageHeight)
{
	int *imageVector;
	imageVector = readPNGFile(filename, imageWidth, imageHeight, LOADDOCUMENT_VERBOSE);
	return imageVector;
}

struct imageDocument *processDocument(int *imageVector, int imageWidth, int imageHeight)
{
	if (imageVector && imageWidth && imageHeight) {
		struct imageDocument *imageDoc;
		imageDoc = findRows(imageVector, imageWidth, imageHeight, MAX_IMAGE_DEPTH);
		return imageDoc;
	} else {
		return NULL;
	}
}

#endif

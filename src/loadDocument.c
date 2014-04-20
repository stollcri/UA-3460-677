/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef LOADDOCUMENT_C
#define LOADDOCUMENT_C

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

	for (int j = 0; j < imageWidth; ++j) {
		for (int i = rowBegin; i < rowEnd; ++i) {
			currentPixel = (i * imageWidth) + j;
			if (imageVector[currentPixel] > SPLIT_THRESHHOLD) {
				//printf(" %d, %d\n", currentPixel, imageVector[currentPixel]);
				blankCol = false;
			}
		}

		if (!blankCol) {
			if (spaceCols > maxCharSpacing) {
				//printf(" SKIP  %d/%d (%d, %d), (%d, %d)\n", spaceCols, maxCharSpacing, j-spaceCols, rowBegin, j, rowEnd);
			} else if (spaceCols > charSpacing) {
				//printf(" SPACE %d/%d (%d, %d), (%d, %d)\n", spaceCols, charSpacing, j-spaceCols, rowBegin, j, rowEnd);
				newchar = newImageDocumentChar(0, 0, 0, 0, ' ');
				addCharToLine(currentLine, newchar);
			}

			pixelColsEnd = j;
			blankCol = true;
			spaceCols = 0;
			++pixelCols;
		} else {
			if (pixelCols > 2) {
				charSpacing = (int)round((charSpacing + (pixelCols / 3)) / 2) + 1;
				*spacing = charSpacing;
			}
			++spaceCols;
			if (pixelColsEnd) {
				// printf(" CHAR  (%d, %d) %d, (%d, %d) %d\n", pixelColsEnd-pixelCols, rowBegin, (pixelColsEnd - (pixelColsEnd-pixelCols) + 2), pixelColsEnd, rowEnd, (rowEnd - rowBegin + 3));

				newchar = newImageDocumentChar((pixelColsEnd-pixelCols), rowBegin-1, pixelColsEnd+2, rowEnd+2, '?');
				addCharToLine(currentLine, newchar);
				
				if (DEBUG_SAVE_INDIVIDUAL_CHARACTERS) {
					int wdth = pixelColsEnd - (pixelColsEnd-pixelCols) + 2;
					int high = rowEnd - rowBegin + 3;

					char fName[100] = "./tst/tst-";
					char buffer[16];
					snprintf(buffer, sizeof(buffer), "%d-%d-%d.png", rowBegin, (pixelColsEnd-pixelCols), pixelColsEnd);
					strcat(fName, buffer);
					// printf(" %d,%d = %s\n", wdth, high, fName);
					int k = 0;
					int *charImageVector = (int*)malloc(wdth * high * sizeof(int));
					for (int i = rowBegin-1; i < rowEnd+2; ++i) {
						for (int j = (pixelColsEnd-pixelCols); j < pixelColsEnd+2; ++j) {
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

	for (int i = 0; i < imageHeight; ++i) {
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

		if (!blankRow) {
			if (!pixelRowBegin) {
				pixelRowBegin = i;
			}
			pixelRowEnd = i;
			blankRow = true;
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

void scaleImageMatrix(int *imageVector, struct imageDocumentChar *imageDocChar)
{
	int width = imageDocChar->x2 - imageDocChar->x1;
	int height = imageDocChar->y2 - imageDocChar->y1;
	int newWidth = width;
	int newHeight = height;

	if (height != width) {
		if (height > width) {
			int padding = height - width;
			int paddingQ = (int)(padding / 2);
			int paddingR = padding - (paddingQ * 2);
			newWidth = paddingQ + width + paddingQ + paddingR;
		} else {
			int padding = width - height;
			int paddingQ = (int)(padding / 2);
			int paddingR = padding - (paddingQ * 2);
			newHeight = paddingQ + width + paddingQ + paddingR;
		}
	}
}

void ocrCharacter(int *imageVector, struct imageDocumentChar *imageDocChar)
{
	if (imageDocChar) {
		printf("%c", imageDocChar->value);
		scaleImageMatrix(imageVector, imageDocChar);
	}
}

void ocrCharLoop(int *imageVector, struct imageDocumentLine *imageDocLine)
{
	if (imageDocLine) {
		if (imageDocLine->characters) {
			struct imageDocumentChar *currentChar = imageDocLine->characters;
			struct imageDocumentChar *nextChar;

			ocrCharacter(imageVector, currentChar);

			while (currentChar->nextChar) {
				nextChar = currentChar->nextChar;
				currentChar = nextChar;

				ocrCharacter(imageVector, currentChar);
			}

			freeImageDocumentChar(nextChar);
		}
	}
}

void ocrLineLoop(int *imageVector, struct imageDocument *imageDoc)
{
	if (imageDoc) {
		if (imageDoc->lines) {
			struct imageDocumentLine *currentLine = imageDoc->lines;
			struct imageDocumentLine *nextLine;

			ocrCharLoop(imageVector, currentLine);

			while (currentLine->nextLine) {
				nextLine = currentLine->nextLine;
				currentLine = nextLine;

				ocrCharLoop(imageVector, currentLine);
			}

			freeImageDocumentLine(nextLine);
		}
	}
}

void startOcr(int *imageVector, struct imageDocument *imageDoc)
{
	ocrLineLoop(imageVector, imageDoc);
}

void loadDocument(char *filename)
{
	int *imageVector;
	int imageWidth = 0;
	int imageHeight = 0;
	imageVector = readPNGFile(filename, &imageWidth, &imageHeight, LOADDOCUMENT_VERBOSE);
	if (imageVector && imageWidth && imageHeight) {
		struct imageDocument *imageDoc;
		imageDoc = findRows(imageVector, imageWidth, imageHeight, MAX_IMAGE_DEPTH);
		startOcr(imageVector, imageDoc);
	}
}

#endif

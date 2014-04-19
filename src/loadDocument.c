/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef LOADDOCUMENT_C
#define LOADDOCUMENT_C

#include <stdbool.h>
#include <math.h>
#include "libpngHelper.c"
#include "imageDocument.c"

#define LOADDOCUMENT_VERBOSE 1
#define SPLIT_THRESHHOLD 64
#define MAX_IMAGE_DEPTH 255

struct imageDocumentLine *findCharacters(int *imageVector, int imageWidth, int rowBegin, int rowEnd, int *spacing)
{
	struct imageDocumentLine *currentLine = newImageDocumentLine();
	struct imageDocumentChar *newchar;
	
	int charSpacing = *spacing;
	if (!charSpacing) {
		charSpacing = 2 + (int)round(imageWidth * .005);
		*spacing = charSpacing;
	}
	int spaceWidth = charSpacing * 4;
	int maxSpaceWidth = spaceWidth * 2;

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
			// WARN: Deviation from Python original
			if (spaceCols > maxSpaceWidth) {
				printf(" SKIP  (%d, %d), (%d, %d)\n", j-spaceCols, rowBegin, j, rowEnd);
				//newchar = newImageDocumentChar();
				//addCharToLine();
			} else if (spaceCols > spaceWidth) {
				printf(" SKIP  (%d, %d), (%d, %d)\n", j-spaceCols, rowBegin, j, rowEnd);
			}

			pixelColsEnd = j;
			blankCol = true;
			spaceCols = 0;
			++pixelCols;
		} else {
			if (pixelCols > 2) {
				charSpacing = (int)round((spaceWidth + (pixelCols / 3)) / 2) + 1;
				*spacing = charSpacing;
			}
			++spaceCols;
			if (pixelColsEnd) {
				printf(" CHAR  (%d, %d), (%d, %d)\n", pixelColsEnd-pixelCols, rowBegin, pixelColsEnd, rowEnd);
			}
			pixelCols = 0;
			pixelColsEnd = 0;
		}
	}

	return currentLine;
	// 	for row in xrange(0, row_count):
	// 		if text_row[row][col] > threshhold:
	// 			blank_col = False
	// 		pixel_col.append(text_row[row][col])

	// 	if not blank_col:
	// 		if space_cols > 8:
	// 			characters.append([''])
	// 		elif space_cols > space_width:
	// 			characters.append([' '])

	// 		text_cols.append(pixel_col)
	// 		blank_col = True
	// 		space_cols = 0
	// 		pixel_cols += 1
			
	// 	else:
	// 		if pixel_cols > 2:
	// 			space_width = int(round((space_width + (pixel_cols / 3)) / 2)) + 1
	// 			# print pixel_cols, space_width
	// 		space_cols += 1
	// 		pixel_cols = 0
	// 		if len(text_cols):
	// 			filename = "./out/img_" + str(row_index) + "-" + str(col) + ".png"
	// 			results = size_character(transpose_image(text_cols), filename)
	// 			characters.append(results)
	// 			text_cols = []

	// return characters, space_width
}

void findRows(int *imageVector, int imageWidth, int imageHeight, int imageDepth)
{
	int pixelIverse = 0;
	int currentPixel = 0;
	
	bool blankRow = true;
	int pixelRowBegin = 0;
	int pixelRowEnd = 0;

	int letterSpacing = 0;
	struct imageDocument *imageDoc = newImageDocument();
	struct imageDocumentLine *newline;

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
				printf("%d - %d\n", pixelRowBegin, pixelRowEnd);

				newline = findCharacters(imageVector, imageWidth, pixelRowBegin, pixelRowEnd, &letterSpacing);
				addLineToDocument(imageDoc, newline);
				//
				// results, spacing = split_characters(text_rows, threshhold, row_index, spacing)
				// for result in results:
				// 	characters.append(result)
				// characters.append('\n')
				// text_rows = []
				// return characters
			}
			pixelRowBegin = 0;
			pixelRowEnd = 0;
		}
	}
}

void loadDocument(char *filename)
{
	int *imageVector;
	int imageWidth = 0;
	int imageHeight = 0;
	imageVector = readPNGFile(filename, &imageWidth, &imageHeight, LOADDOCUMENT_VERBOSE);
	if (imageVector && imageWidth && imageHeight) {
		findRows(imageVector, imageWidth, imageHeight, MAX_IMAGE_DEPTH);
	}
}

#endif

/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef IMAGEDOCUMENT_C
#define IMAGEDOCUMENT_C

struct imageDocumentChar {
	int x1;
	int y1;
	int x2;
	int y2;
	char value;
	struct imageDocumentChar *nextChar;
	struct imageDocumentChar *lastChar;
};

struct imageDocumentLine {
	struct imageDocumentChar *characters;
	struct imageDocumentLine *nextLine;
	struct imageDocumentLine *lastLine;
};

struct imageDocument {
	struct imageDocumentLine *lines;
};


struct imageDocument *newImageDocument()
{
	struct imageDocument *newDocument;
	newDocument = (struct imageDocument*) malloc(sizeof(struct imageDocument));

	newDocument->lines = NULL;

	return newDocument;
}

struct imageDocumentLine *newImageDocumentLine()
{
	struct imageDocumentLine *newDocumentLine;
	newDocumentLine = (struct imageDocumentLine*) malloc(sizeof(struct imageDocumentLine));

	newDocumentLine->characters = NULL;
	newDocumentLine->nextLine = NULL;
	newDocumentLine->lastLine = NULL;
	
	return newDocumentLine;
}

struct imageDocumentChar *newImageDocumentChar(x1, y1, x2, y2, value)
{
	struct imageDocumentChar *newDocumentChar;
	newDocumentChar = (struct imageDocumentChar*) malloc(sizeof(struct imageDocumentChar));

	newDocumentChar->x1 = x1;
	newDocumentChar->y1 = y1;
	newDocumentChar->x2 = x2;
	newDocumentChar->y2 = y2;
	newDocumentChar->value = value;
	newDocumentChar->nextChar = NULL;
	newDocumentChar->lastChar = NULL;

	return newDocumentChar;
}

struct imageDocumentChar *newImageDocumentCharBlank()
{
	return newImageDocumentChar(0, 0, 0, 0, ' ');
}

void freeImageDocument(struct imageDocument *targetDocument)
{
	// TODO: free the memory
}

void freeImageDocumentLine(struct imageDocumentLine *targetLine)
{
	// TODO: free the memory
}

void freeImageDocumentChar(struct imageDocumentChar *targetChar)
{
	// TODO: free the memory
}


void addLineToDocument(struct imageDocument *targetDocument, struct imageDocumentLine *sourceLine)
{
	if (targetDocument && sourceLine) {
		if (targetDocument->lines) {
			struct imageDocumentLine *currentLine = targetDocument->lines;
			struct imageDocumentLine *nextLine = newImageDocumentLine();
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

void addCharToLine(struct imageDocumentLine *targetLine, struct imageDocumentChar *sourceChar)
{
	if (targetLine && sourceChar) {
		if (targetLine->characters) {
			struct imageDocumentChar *currentChar = targetLine->characters;
			struct imageDocumentChar *nextChar = newImageDocumentCharBlank();

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

#endif

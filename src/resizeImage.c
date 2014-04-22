/**
 * http://www.compuphase.com/graphic/scale.htm
 */

#ifndef RESIZEIMAGE_C
#define RESIZEIMAGE_C

void ScaleLineA(int *Target, int *Source, int SrcWidth, int TgtWidth)
{
	int NumPixels = TgtWidth;
	int IntPart = SrcWidth / TgtWidth;
	int FractPart = SrcWidth % TgtWidth;
	int E = 0;

	while (NumPixels-- > 0) {
		*Target++ = *Source;
		Source += IntPart;
		E += FractPart;
		if (E >= TgtWidth) {
			E -= TgtWidth;
			Source++;
		} /* if */
	} /* while */
}

#define AVERAGE(a, b)   (int)( ((a) + (b)) >> 1 )

void ScaleLine(int *Target, int *Source, int SrcWidth, int TgtWidth)
{
	/* N.B. because of several simplifications of the algorithm,
	 *      the zoom range is restricted between 0.5 and 2. That
	 *      is: TgtWidth must be >= SrcWidth/2 and <= 2*SrcWidth.
	 */
	int NumPixels = TgtWidth;
	int Mid = TgtWidth / 2;
	int E = 0;
	int p;

	if (TgtWidth > SrcWidth)
		NumPixels--;
	while (NumPixels-- > 0) {
		p = *Source;
		if (E >= Mid)
			p = AVERAGE(p, *(Source+1));
		*Target++ = p;
		E += SrcWidth;
		if (E >= TgtWidth) {
			E -= TgtWidth;
			Source++;
		} /* if */
	} /* while */
	if (TgtWidth > SrcWidth)
		*Target = *Source;
}

void resizeImage(int *Source, int *Target, int SrcWidth, int SrcHeight, int TgtWidth, int TgtHeight)
{
	int NumPixels = TgtHeight;
	int IntPart = (SrcHeight / TgtHeight) * SrcWidth;
	int FractPart = SrcHeight % TgtHeight;
	int E = 0;
	int *PrevSource = NULL;

	while (NumPixels-- > 0) {
		if (Source == PrevSource) {
			memcpy(Target, Target-TgtWidth, TgtWidth*sizeof(*Target));
		} else {
			ScaleLine(Target, Source, SrcWidth, TgtWidth);
			PrevSource = Source;
		} /* if */
		Target += TgtWidth;
		Source += IntPart;
		E += FractPart;
		if (E >= TgtHeight) {
			E -= TgtHeight;
			Source += SrcWidth;
		} /* if */
	} /* while */
}

void sizeSquareImage(int *source, int *target, int sourceSides, int targetSides)
{
	int diff = targetSides - sourceSides;
	int sourcePixel = 0;
	int targetPixel = 0;

	if (diff == 0) {
		// printf(" DIFF 0\n");
		for (int i = 0; i < targetSides; ++i) {
			for (int j = 0; j < targetSides; ++j) {
				sourcePixel = (i * sourceSides) + j;
				targetPixel = (i * targetSides) + j;
				target[targetPixel] = source[sourcePixel];
			}
		}

	} else if (diff <= -2) {
		// printf(" DIFF 1-4\n");
		int j = 0;
		int l = 0;
		int modSkip = diff;
		if (modSkip == 1) {
			modSkip = round(targetSides / 2);
		}
		for (int i = 0; i < targetSides; ++i) {
			l = 0;
			for (int k = 0; k < targetSides; ++k) {
				sourcePixel = (j * sourceSides) + l;
				targetPixel = (i * targetSides) + k;
				target[targetPixel] = source[sourcePixel];
				
				
				if ((k % modSkip) != (modSkip - 1)) {
					// printf("%d, %d, %d\n", k, diff, (k % diff));
					++l;
				}
			}
			if ((i % modSkip) != (modSkip - 1)) {
				++j;
			}
		}

	} else if (diff <= 2) {
		// printf(" DIFF 1-4\n");
		int j = 0;
		int l = 0;
		for (int i = 0; i < targetSides; ++i) {
			l = 0;
			for (int k = 0; k < targetSides; ++k) {
				sourcePixel = (j * sourceSides) + l;
				targetPixel = (i * targetSides) + k;
				target[targetPixel] = source[sourcePixel];
				
				if (k != 6) {
					++l;
				}
			}
			if (i != 3) {
				++j;
			}
		}

	} else if (diff <= 8) {
		// printf(" DIFF 1-4\n");
		int j = 0;
		int l = 0;
		for (int i = 0; i < targetSides; ++i) {
			l = 0;
			for (int k = 0; k < targetSides; ++k) {
				sourcePixel = (j * sourceSides) + l;
				targetPixel = (i * targetSides) + k;
				target[targetPixel] = source[sourcePixel];
				
				if ((k != 4) && (k != 6) && (k != 8) && (k != 10)) {
					++l;
				}
			}
			if ((i != 5) && (i != 7) && (i != 9) && (i != 11)) {
				++j;
			}
		}
		
	// punctuation
	} else {
		// printf(" DIFF >4\n");
		resizeImage(source, target, sourceSides, sourceSides, targetSides, targetSides);
	}
}

#endif

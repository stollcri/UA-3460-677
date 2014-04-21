/**
 * http://www.compuphase.com/graphic/scale.htm
 */

void ScaleLine(int *Target, int *Source, int SrcWidth, int TgtWidth)
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

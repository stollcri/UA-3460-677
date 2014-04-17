/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#include "loadKnowledge.c"
#include "loadDocument.c"

int main(int argc, char const *argv[])
{
	loadKnowledge("./dat/eigenspace", "./dat/characters");
	loadDocument("./tst/RightsOfManB.png");
}

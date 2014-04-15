CC = cc
CFLAGS = -I ./lib/

default: all

.PHONY: all ocr test
all: ocr test

ocr:
	${CC} ${CFLAGS} -o ./bin/ocr ./src/ocr.c

test:
	./bin/ocr

CC = cc
CFLAGS = -I ./lib/

default: all

.PHONY: all ocr test
all: ocr test

ocr:
	${CC} ${CFLAGS} -o ./bin/ocr-cpu ./src/ocr-cpu.c

test:
	./bin/ocr-cpu

clean:
	-rm ./bin/ocr-cpu

OS := $(shell uname)

ifeq ($(OS),Darwin)
	CC = cc
	NVCC = cc
else
	CC = c99
	NVCC = cc
endif


CFLAGS = -I/usr/local/include/libpng16 -L/usr/local/lib -lpng16
NVCFLAGS = -I/usr/local/include/libpng16 -L/usr/local/lib -lpng16

default: all

.PHONY: all ocr ocr-cpu ocr-gpu test test-cpu test-gpu
all: ocr-cpu test-cpu



ocr: ocr-cpu ocr-gpu

ocr-cpu:
	${CC} ${CFLAGS} -o ./bin/ocr-cpu ./src/ocr-cpu.c

ocr-gpu:
	${NVCC} ${NVCFLAGS} -o ./bin/ocr-cpu ./src/ocr-cpu.c



test: test-cpu test-gpu

test-cpu:
	./bin/ocr-cpu "./tst/RightsOfManB.png"

test-gpu:
	./bin/ocr-gpu "./tst/RightsOfManB.png"



clean:
	-rm ./bin/ocr-cpu

libpng:
	:
	# cd libpng-1.6.10
	# ./configure --prefix=/usr/local
	# cp ./scripts/makefile.darwin makefile
	# vi makefile # change ARCH= to ARCH=-arch x86_64
	# sudo make install
	# make clean
	# cd /usr/local/lib
	# sudo rm libpng16.dylib
	# sudo ln -s libpng16.16.dylib libpng16.dylib
	# /usr/local/bin/libpng-config --cflags --ldflags # use results as CFLAGS
	:

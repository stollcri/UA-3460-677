OS := $(shell uname)

ifeq ($(OS),Darwin)
	CC = cc
	NVCC = cc
	CFLAGS = -I/usr/local/include/libpng16 -L/usr/local/lib -lpng16
	NVCFLAGS = -Weverything -I/usr/local/include/libpng16 -L/usr/local/lib -lpng16
else
	CC = nvcc
	NVCC = nvcc
	CFLAGS = --compiler-options -std=c99 -arch=sm_21 -I/usr/include/libpng12 -L/usr/lib/x86_64-linux-gnu/ -lpng12 -I/usr/local/cutil/include -L/usr/local/cutil/lib -lcutil_x86_64
	NVCFLAGS = --compiler-options -std=c99 -arch=sm_21 -I/usr/include/libpng12 -L/usr/lib/x86_64-linux-gnu/ -lpng12 -I/usr/local/cutil/include -L/usr/local/cutil/lib -lcutil_x86_64
endif


default: all

.PHONY: all ocr ocr-cpu ocr-gpu test test-cpu test-gpu
all: ocr-cpu test-cpu



ocr: ocr-cpu ocr-gpu
gpu: ocr-gpu

ocr-cpu:
	mkdir -p ./bin/
	${CC} ${CFLAGS} -o ./bin/ocr-cpu ./src/ocr-cpu.c

ocr-gpu:
	mkdir -p ./bin/
	${NVCC} ${NVCFLAGS} -o ./bin/ocr-gpu ./src/ocr-gpu.cu



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

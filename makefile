CC = cc
CFLAGS = -I/usr/local/include/libpng16 -L/usr/local/lib -lpng16

default: all

.PHONY: all ocr test
all: ocr test

ocr:
	${CC} ${CFLAGS} -o ./bin/ocr-cpu ./src/ocr-cpu.c

test:
	./bin/ocr-cpu

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
	
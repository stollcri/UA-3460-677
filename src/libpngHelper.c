/**
 * 3460:677 Final Project: Paralelizing OCR using PCA
 * Christopher Stoll, 2014
 */

#ifndef LIBPNGHELPER_C
#define LIBPNGHELPER_C

#include <stdlib.h>
#include <stdio.h>
#include <png.h>

#define PNG_BYTES_TO_CHECK 4

/**
 * Uses libpng to read in a PNG file
 * @param  filename    The name of the PNG file to read in
 * @param  imageWidth  Reference to an integer which will store the image's width
 * @param  imageHeight Reference to an integer which will store the image's height
 * @param  verbosity   Whether or not to print error messages to stderr
 * @return             Returns an array of integers representing the image pixels
 */
int *readPNGFile(char *filename, int *imageWidth, int *imageHeight, int verbosity) {
	FILE *pngfile = fopen(filename, "rb");
	if(!pngfile) {
		if(verbosity > 0) {
			fprintf(stderr, "Error opening file: %s\n", filename);
		}
		return NULL;
	}

	char header[PNG_BYTES_TO_CHECK];
	fread(header, 1, PNG_BYTES_TO_CHECK, pngfile);
	int is_png = !png_sig_cmp((png_bytep)header, 0, PNG_BYTES_TO_CHECK);
	if (!is_png) {
		if(verbosity > 0) {
			fprintf(stderr, "Invalid PNG file: %s\n", filename);
		}
		return NULL;
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) {
		return NULL;
	}

	png_infop info = png_create_info_struct(png);
	if(!info) {
		return NULL;
	}

	if(setjmp(png_jmpbuf(png))) {
		return NULL;
	}

	png_init_io(png, pngfile);
	png_set_sig_bytes(png, PNG_BYTES_TO_CHECK);
	png_read_info(png, info);
	int width = png_get_image_width(png, info);
	int height = png_get_image_height(png, info);
	png_byte color_type = png_get_color_type(png, info);
	png_byte bit_depth = png_get_bit_depth(png, info);

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if(bit_depth == 16)
		png_set_strip_16(png);

	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if(color_type == PNG_COLOR_TYPE_RGB ||
		 color_type == PNG_COLOR_TYPE_GRAY ||
		 color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if(color_type == PNG_COLOR_TYPE_GRAY ||
		 color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	png_bytep *row_pointers;
	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
	}

	png_read_image(png, row_pointers);
	fclose(pngfile);

	int *imagePixels = (int*)malloc(sizeof(int) * height * width);

	int n = 0;
	int rPixel = 0;
	int bPixel = 0;
	int gPixel = 0;
	//int aPixel = 0;
	for(int y = 0; y < height; ++y) {
		png_bytep row = row_pointers[y];
		for(int x = 0; x < width; ++x) {
			png_bytep pixel = &(row[x * 4]);
			rPixel = (int)pixel[0];
			gPixel = (int)pixel[1];
			bPixel = (int)pixel[2];
			//aPixel = (int)pixel[3];

			n = (y * width) + x;
			if ((rPixel == gPixel) && (gPixel == bPixel)) {
				imagePixels[n] = rPixel;
			} else {
				if (rPixel > gPixel) {
					if (rPixel > bPixel) {
						imagePixels[n] = rPixel;
					} else {
						imagePixels[n] = bPixel;
					}
				} else {
					if (gPixel > bPixel) {
						imagePixels[n] = gPixel;
					} else {
						imagePixels[n] = bPixel;
					}
				}
			}
		}
	}

	// release png memory
	for(int y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);

	*imageWidth = width;
	*imageHeight = height;
	return imagePixels;
}

void write_png_file(int *imageVector, int width, int height, char *filename) {
	FILE *fp = fopen(filename, "wb");
	if(!fp) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGBA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);

	png_bytep *row_pointers;
	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int y = 0; y < height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
	}

	int i = 0;
	for(int y = 0; y < height; y++) {
		for(int z = 0; z < width; z++) {
			row_pointers[y][z*4+0] = (png_byte)imageVector[i];
			row_pointers[y][z*4+1] = (png_byte)imageVector[i];
			row_pointers[y][z*4+2] = (png_byte)imageVector[i];
			row_pointers[y][z*4+3] = 255;
			++i;
		}
	}

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);

	for(int y = 0; y < height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);

	fclose(fp);
}

#endif

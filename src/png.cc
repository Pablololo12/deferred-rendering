#include "png.h"
#include <iostream>
#include <cstdio>
#include <png.h>

using namespace std;

PNG::PNG()
{
	w = h = 0;
}

PNG::PNG(const string& name)
{
	load(name);
}

void PNG::load(const string& name)
{
	w = h = 0;

	FILE* f = fopen(name.c_str(),"rb");
	if (!f)
		return;

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if (!png)
	{
		fclose(f);
		return;
	}

	png_infop   info = png_create_info_struct(png);
	if (!info)
	{
		png_destroy_read_struct(&png,0,0);
		fclose(f);
		return;
	}

	png_init_io(png,f);
	png_read_info(png,info);

	w = png_get_image_width(png,info);
	h = png_get_image_height(png,info);

	uint32_t mode = png_get_color_type(png,info);
	uint32_t bits = png_get_bit_depth(png,info);

	switch(mode)
	{
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(png);
			break;
		case PNG_COLOR_TYPE_GRAY:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			if (bits<8)
				png_set_expand_gray_1_2_4_to_8(png);
			png_set_gray_to_rgb(png);
			break;
	}
	if (bits==16)
#if PNG_LIBPNG_VER >= 10504
		png_set_scale_16(png);
#else
		png_set_strip_16(png);
#endif
	if (bits<8)
		png_set_packing(png);
	if (png_get_valid(png,info,PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	png_read_update_info(png,info);

	uint32_t chns = png_get_channels(png,info);
	uint32_t rb = png_get_rowbytes(png,info);

	px.resize(3*w*h);

	png_byte* buffer = new png_byte[rb];
	const float ik = 1.0/255.0;
	for (int row=0; row<h; row++)
	{
		png_read_row(png,buffer,0);
		png_byte* src = buffer;
		float*    dst = &px[3*row*w];
        for (int col=0; col<w; col++)
        {
			dst[0] = ik*src[0];
			dst[1] = ik*src[1];
			dst[2] = ik*src[2];
            src += chns;
			dst += 3;
        }
	}
	png_read_end(png,0);

	png_destroy_read_struct(&png,&info,0);
	fclose(f);
}

void save_image_vector(int width, int height, const char * name, float* pixels)
{
	int i,d,j;
	png_bytep row = NULL;
	cout << "Taking Snapshot ";
	FILE *fp = fopen(name, "wb");
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png))){
		cout << "Error taking snapshot" << endl;
	}
	png_init_io(png, fp);
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);
	
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));
	for (i=height-1; i>=0; i--) {
		for (d=0;d<width*3;d++) {
			row[d]=(int)(pixels[i*width*3+d]*255)%256;
		}
		png_write_row(png, row);
	}
	free(row);
	png_write_end(png, NULL);
	fclose(fp);
	cout << "[\033[1;32mDone\033[0m]" << endl;
}

void save_image_vector_2(int width, int height, const char * name, unsigned char* pixels)
{
	int i,d,j;
	png_bytep row = NULL;
	cout << "Taking Snapshot ";
	FILE *fp = fopen(name, "wb");
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png))){
		cout << "Error taking snapshot" << endl;
	}
	png_init_io(png, fp);
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);
	
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));
	for (i=height-1; i>=0; i--) {
		for (d=0,j=0;d<width*3;d=d+3,j=j+4) {
			row[d]=pixels[i*width*4+j];
			row[d+1]=pixels[i*width*4+j+1];
			row[d+2]=pixels[i*width*4+j+2];
		}
		png_write_row(png, row);
	}
	free(row);
	png_write_end(png, NULL);
	fclose(fp);
	cout << "[\033[1;32mDone\033[0m]" << endl;
}

void save_image_vector_3(int width, int height, const char * name, unsigned char* pixels)
{
	int i,d,j;
	png_bytep row = NULL;
	cout << "Taking Snapshot ";
	FILE *fp = fopen(name, "wb");
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info = png_create_info_struct(png);
	if (setjmp(png_jmpbuf(png))){
		cout << "Error taking snapshot" << endl;
	}
	png_init_io(png, fp);
	png_set_IHDR(
		png,
		info,
		width, height,
		8,
		PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);
	png_write_info(png, info);
	
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));
	for (i=height-1; i>=0; i--) {
		for (d=0;d<width*3;d++) {
			row[d]=pixels[i*width*3+d];
		}
		png_write_row(png, row);
	}
	free(row);
	png_write_end(png, NULL);
	fclose(fp);
	cout << "[\033[1;32mDone\033[0m]" << endl;
}

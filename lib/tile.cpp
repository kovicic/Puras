// --------------------------------------------------------------------------

#include <stdio.h>     // fopen, fread, fprintf, perror
#include <stdlib.h>
#include <png.h>       // PNG data types and functions
#include <GLES/gl.h>   // OpenGL ES 1.1 data types and functions
#include "gles.h"      // struct opengles, gles*-functions
#include "tile.h"      // struct tile, loadPngTile

// --------------------------------------------------------------------------
//   load PNG as object which can be used immeadiately 
// --------------------------------------------------------------------------
int tileLoadPng(struct opengles *gles, struct tile *tile, const char *file)
{
	// return values
	int ret;

	// open PNG file in binary format
	FILE *png_file = fopen(file, "rb");
	if (png_file == 0) {
		perror("loadPngTile.fopen");
		exit(-1);
	}

	// read PNG-Header
	png_byte png_header[8];
	ret = fread(png_header, 1, 8, png_file);
	if (ret < 0) {
		perror("loadPngTile.fread");
		exit(-1);
	}

	// is this a PNG?
	ret = png_sig_cmp(png_header, 0, 8);
	if (ret) {
		fprintf(stderr, "%s is not a PNG file!\n", file);
		exit(-1);
	}

	// create PNG structure for later access
	png_structp png_read_ptr;
	png_read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_read_ptr) {
		fprintf(stderr, "png_create_read_struct failed!\n");
		fclose(png_file);
		exit(-1);
	}

	// create PNG structures with image information
	png_infop png_info_ptr, png_iend_ptr;
	png_info_ptr = png_create_info_struct(png_read_ptr);
	png_iend_ptr = png_create_info_struct(png_read_ptr);
	if (!png_info_ptr || !png_iend_ptr)
	{
		fprintf(stderr, "png_create_info_struct failed!\n");
		png_destroy_read_struct(&png_read_ptr, 0, 0);
		fclose(png_file);
		exit(-1);
	}

	// init error handling for later PNG access
	ret = setjmp(png_jmpbuf(png_read_ptr));
	if (ret) {
		fprintf(stderr, "png_jmpbuf failed!\n");
		png_destroy_read_struct(&png_read_ptr, &png_info_ptr,
		                        &png_iend_ptr);
		fclose(png_file);
		exit(-1);
	}

	// init IO access to PNG
	png_init_io(png_read_ptr, png_file);

	// signal, that first 8 bytes have been read
	png_set_sig_bytes(png_read_ptr, 8);

	// read PNG header
	png_read_info(png_read_ptr, png_info_ptr);

	// read image infors from PNG structure
	png_uint_32 width, height;
	int depth, type;
	png_get_IHDR(png_read_ptr, png_info_ptr,
	             &width, &height, &depth, &type, 0, 0, 0);

	// read number of bytes per line
	int rowsize;
	rowsize = png_get_rowbytes(png_read_ptr, png_info_ptr);

	// allocate memory
	png_byte *png_data;
	png_data = (png_byte*)malloc(rowsize * height);
	if (!png_data) {
		fprintf(stderr, "malloc failed!\n");
		png_destroy_read_struct(&png_read_ptr, &png_info_ptr,
		                        &png_iend_ptr);
		fclose(png_file);
		exit(-1);
	}

	// Pointer array for all lines
	png_bytep *png_row_ptrs;
	png_row_ptrs = (png_bytep*)malloc(sizeof(png_bytep) * height);
	if (!png_data) {
		fprintf(stderr, "malloc failed!\n");
		png_destroy_read_struct(&png_read_ptr, &png_info_ptr,
		                        &png_iend_ptr);
		fclose(png_file);
		free(png_data);
		exit(-1);
	}

	// set up pointer array setzen. Each entry points to the beginning
	// of a line
	png_uint_32 i;
	for (i = 0; i < height; i++)
	{
		png_row_ptrs[height - 1 - i] = png_data + i * rowsize;
	}

	// read the image using the pointer array
	png_read_image(png_read_ptr, png_row_ptrs);

	// calculate vertex coordinates
	GLfloat x_coord = (gles->ratio) / (gles->width  / (GLfloat)width );
	GLfloat y_coord = (       1.0f) / (gles->height / (GLfloat)height);

	// set vertex ccordinates
	tile->vertices[0] = -x_coord;
	tile->vertices[2] = -x_coord;
	tile->vertices[4] =  x_coord;
	tile->vertices[6] =  x_coord;
	tile->vertices[1] = -y_coord;
	tile->vertices[3] =  y_coord;
	tile->vertices[5] = -y_coord;
	tile->vertices[7] =  y_coord;

	// create texture object if required
	if (tile->texture == 0)
	{
		glGenTextures(1, &(tile->texture));
	}

	// pull texture object into foreground
	glBindTexture(GL_TEXTURE_2D, tile->texture);

	// connect texture object with the PNG data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
	             GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)png_data);

	// setup filter mechanisms of texture object
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// deallocate ressourcen of libpng
	png_destroy_read_struct(&png_read_ptr, &png_info_ptr, &png_iend_ptr);
	free(png_data);
	free(png_row_ptrs);
	fclose(png_file);

	return 0;
}


// --------------------------------------------------------------------------
//   render the tile
// --------------------------------------------------------------------------
int tileDraw(struct tile *tile)
{
	// tell OpenGL to write vertrex and texture coordinates
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// use dashboard texture
	glBindTexture(GL_TEXTURE_2D, tile->texture);

	// specify the texture coords to be used
	glTexCoordPointer(2, GL_FLOAT, 0, tile_texture_coords);

	// hand over dashboard vertex coords
	glVertexPointer(2, GL_FLOAT, 0, tile->vertices);

	// render object
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// done
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	return 0;
}


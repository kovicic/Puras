// --------------------------------------------------------------------------


// include only once
#ifndef TILE_H_
#define TILE_H_ 1

struct tile
{
	GLfloat vertices[8];
	GLuint  texture;
};

#define TILE_ZEROINIT { { 0 }, 0 }

// specify coordinates for textures. Since we work only with
// rectangles, we reuse the same coodinates for all textures
static const GLfloat tile_texture_coords[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
};

// load PNG as an object to be used immediately
int tileLoadPng(struct opengles *gles, struct tile *tile, const char *file);

// render tile
int tileDraw(struct tile *tile);

#endif

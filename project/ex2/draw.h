#ifndef __DRAW_H__
#define __DRAW_H__

#include <GLES/gl.h>   //!< OpenGL ES 1.1 data types and functions

//!< Function prototypes

GLfloat filter(GLfloat value, GLfloat *buffer);

GLfloat kmh2deg(GLfloat kmh);
GLfloat rpm2deg(GLfloat rpm);
GLfloat temp2deg(GLfloat temp);
GLfloat fuel2deg(GLfloat val);

void* drawNewDesignMain(void *);
void* drawOriginalDesignMain(void *data);

#endif /* __DRAW_H__ */

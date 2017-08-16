#include <stdio.h>     //!< asprintf()
#include <unistd.h>    //!< usleep
#include <math.h>      //!< tanh
#include <GLES/gl.h>   //!< OpenGL ES 1.1 data types and functions

#include "draw.h"

GLfloat filter(GLfloat value, GLfloat *buffer)
{
	*buffer = *buffer + tanh((value - *buffer) / 16.0f);
	return *buffer;
}

GLfloat kmh2deg(GLfloat kmh)
{
	//!< Limit range
	if(kmh > 150.0f)
	{
		kmh = 150.0f;
	}
	else if(kmh < 0.0f)
	{
		kmh = 0.0f;
	}

	//!< The scale has 16 marks, i.e. 15 degrees each
	kmh *= 1.5f;

	//!< Add offset for zero position
	kmh = 135.0f - kmh;

	return kmh;
}

GLfloat rpm2deg(GLfloat rpm)
{
	//!< Limit range
	if(rpm > 7000.0f)
	{
		rpm = 7000.0f;
	}
	else if(rpm < 0.0f)
	{
		rpm = 0.0f;
	}

	//!< The scale has 16 marks, i.e. 15 degrees each
	rpm *= 0.03f;

	//!< Add offset for zero position
	rpm += -135.0f;

	return rpm;
}

GLfloat temp2deg(GLfloat temp)
{
	//!< Limit range
	if(temp > 120.0f)
	{
		temp = 120.0f;
	}
	else if(temp < 40.0f)
	{
		temp = 40.0f;
	}

	temp -= 40.0f;

	//!< The scale has 9 marks, i.e. 13.33 degrees each
	temp *= (9.0f * 13.33f) / 80.0f;

	//!< Add offset for zero position
	temp = 150.0f - temp;

	return temp;
}

GLfloat fuel2deg(GLfloat val)
{
	//!< Limit range
	if(val > 1.0f)
	{
		val = 1.0f;
	}
	else if(val < 0.0f)
	{
		val = 0.0f;
	}

	//!< The scale has 9 marks, i.e. 13.33 degrees each
	val *= (9.0f * 13.33f);

	//!< Add offset for zero position
	val += -150.0f;

	return val;
}
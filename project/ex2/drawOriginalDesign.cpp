#include <stdio.h>     //!< asprintf()
#include <unistd.h>    //!< usleep
#include <math.h>      //!< tanh
#include <GLES/gl.h>   //!< OpenGL ES 1.1 data types and functions

#include "dashboard.h"
#include "gles.h"
#include "tile.h"
#include "draw.h"

void* drawOriginalDesignMain(void *data)
{
	struct dashboard *db = (struct dashboard *)data;
	GLfloat kmhdeg = 135.0f;
	GLfloat rpmdeg = -135.0f;

	//!< OpenGL ES init
	struct opengles opengles;
	glesInitialize(&opengles);

	//!< Load texture for dashboard
	struct tile dashboard = TILE_ZEROINIT;
	tileLoadPng(&opengles, &dashboard, "../../imgOriginalDesign/dashboard_full.png");

	//!< Load texture for needle
	struct tile needle = TILE_ZEROINIT;
	tileLoadPng(&opengles, &needle, "../../imgOriginalDesign/needle.png");

	//!< Load texture for blinker
	struct tile blinker = TILE_ZEROINIT;
	tileLoadPng(&opengles, &blinker, "../../imgOriginalDesign/blinker.png");

	GLuint flags = 0;
	GLuint counter = 0;
	GLfloat tempdeg = 0.0;
	GLfloat fueldeg = 0.0;

	do
	{
		//!< Smoothly approach to the km/h and rpm values
		kmhdeg = filter(kmh2deg(db->kmh), &kmhdeg);
		rpmdeg = filter(rpm2deg(db->rpm), &rpmdeg);
		tempdeg = filter(temp2deg(db->temp), &tempdeg);
		fueldeg = filter(fuel2deg(db->fuel), &fueldeg);

		//!< Logic for blinkers
		if(db->lamps & DASHBOARD_LEFT)
		{
			if(!(counter % 25))
			{
				flags ^= DASHBOARD_LEFT;
			}
		}
		else
		{
			flags &= ~DASHBOARD_LEFT;
		}

		if(db->lamps & DASHBOARD_RIGHT)
		{
			if(!(counter % 25))
			{
				flags ^= DASHBOARD_RIGHT;
			}
		}
		else
		{
			flags &= ~DASHBOARD_RIGHT;
		}

		//!< Clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT);

		//!< Render dashboard
		tileDraw(&dashboard);

		//!< ----------- BIG NEEDLES ------------
		//!< -------- Render left needle --------

		//!< Save current transformation matrix
		glPushMatrix();

			//!< Shift needle to correct position
			glTranslatef(-1.0f, 0, 0);

			//!< Rotate the needle
			glRotatef(kmhdeg, 0, 0, 1);

			//!< Correct the rotational center of the needle
			glTranslatef(0, 0.25f, 0);

			//!< Render the needle
			tileDraw(&needle);

		//!< Restore the transformation matrix
		glPopMatrix();


		//!< -------- Render right needle --------

		//!< Save current transformation matrix
		glPushMatrix();

			//!< Shift needle to correct position
			glTranslatef(1.0f, 0, 0);

			//!< Rotate the needle
			glRotatef(rpmdeg, 0, 0, 1);

			//!< Correct the rotational center of the needle
			glTranslatef(0, 0.25f, 0);

			//!< Render the needle
			tileDraw(&needle);

		glPopMatrix();

		//!< ------------ SMALL NEEDLES ------------
		//!< -------- Render fuel indicator --------

		//!< Save current transformation matrix
		glPushMatrix();

			//!< Shift needle to correct position
			glTranslatef(1.0f/8.0f, -0.564f, 0);

			//!< Scale the needle
			glScalef(0.5f, 0.5f, 0);

			//!< Rotate the needle
			glRotatef(fueldeg, 0, 0, 1);

			//!< Correct the rotational center of the needle
			glTranslatef(0, 0.25f, 0);

			//!< Render the needle
			tileDraw(&needle);

		//!< Restore the transformation matrix
		glPopMatrix();

		//!< -------- Render temperatur indicator --------

		//!< Save current transformation matrix
		glPushMatrix();

			//!< Shift needle to correct position
			glTranslatef(-1.0f/8.0f, -0.564f, 0);

			//!< Scale the needle
			glScalef(0.5f, 0.5f, 0);

			//!< Rotate the needle
			glRotatef(tempdeg, 0, 0, 1);

			//!< Correct the rotational center of the needle
			glTranslatef(0, 0.25f, 0);

			//!< Render the needle
			tileDraw(&needle);

		//!< Restore the transformation matrix
		glPopMatrix();


		//!< -------- Render left blinker --------
		if(flags & DASHBOARD_LEFT)
		{
			//!< Save current transformation matrix
			glPushMatrix();

				//!< Shift blinker to correct position
				glTranslatef(-5.0f/8.0f, -28.0f/32.0f, 0);

				//!< Render the blinker
				tileDraw(&blinker);

			//!< Restore the transformation matrix
			glPopMatrix();
		}

		//!< -------- Render right blinker --------
		if(flags & DASHBOARD_RIGHT)
		{
			//!< Save current transformation matrix
			glPushMatrix();

				//!< Shift blinker to correct position
				glTranslatef(5.0f/8.0f, -28.0f/32.0f, 0);

				//!< Scale the blinker
				glScalef(-1.0f, 1.0f, 1.0f);

				//!< Render the blinker
				tileDraw(&blinker);

			//!< Restore the transformation matrix
			glPopMatrix();
		}

		//!< -------- Display the rendered image  --------
		glesDraw(&opengles);

		usleep(20*1000);

		//!< Counter increment.
		counter++;
	}
	while(glesRun(&opengles));

	//!< Deallocate OpenGL ES Ressources
	glesDestroy(&opengles);

	return 0;
}

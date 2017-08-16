#include <stdio.h>     //!< asprintf()
#include <unistd.h>    //!< usleep
#include <math.h>      //!< tanh
#include <GLES/gl.h>   //!< OpenGL ES 1.1 data types and functions

#include "dashboard.h"
#include "gles.h"
#include "tile.h"
#include "draw.h"

void* drawNewDesignMain(void *data)
{
	struct dashboard *db = (struct dashboard *)data;
	GLfloat kmhdeg = 135.0f;
	GLfloat rpmdeg = -135.0f;

	//!< OpenGL ES init
	struct opengles opengles;
	glesInitialize(&opengles);

	//!< Load texture for dashboard
	struct tile dashboard = TILE_ZEROINIT;
	tileLoadPng(&opengles, &dashboard, "../../imgNewDesign/dashboard_full.png");

	//!< Load texture for big needle
	struct tile rightNeedle = TILE_ZEROINIT;
	tileLoadPng(&opengles, &rightNeedle, "../../imgNewDesign/rightNeedle.png");

	//!< Load texture for small needle
	struct tile leftNeedle = TILE_ZEROINIT;
	tileLoadPng(&opengles, &leftNeedle, "../../imgNewDesign/leftNeedle.png");

	//!< Load texture for blinker
	struct tile blinker = TILE_ZEROINIT;
	tileLoadPng(&opengles, &blinker, "../../imgNewDesign/blinker.png");

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

		//!< clear framebuffer
		glClear(GL_COLOR_BUFFER_BIT);

		//!< render dashboard
		tileDraw(&dashboard);

		//!< ----------- BIG NEEDLES ------------
		//!< -------- Render left needle --------

		glPushMatrix();

			//!< Shift needle to correct position
			glTranslatef(-1.0f, 0, 0);

			//!< Rotate the needle
			glRotatef(kmhdeg, 0, 0, 1);

			//!< Correct the rotational center of the needle
			glTranslatef(0.0f, 0.25f, 0);

			//!< Render the needle
			tileDraw(&leftNeedle);

		//!< Restore the transformation matrix
		glPopMatrix();

		//!< -------- Render right needle --------
		glPushMatrix();

			//!< Shift needle to correct position
			glTranslatef(1.0f, 0, 0);

			//!< Rotate the needle
			glRotatef(rpmdeg, 0, 0, 1);

			//!< Correct the rotational center of the needle
			glTranslatef(0.0f, 0.25f, 0);

			//!< Render the needle
			tileDraw(&rightNeedle);

		//!< Restore the transformation matrix
		glPopMatrix();

		//!< ------------ SMALL NEEDLES ------------
		//!< -------- Render fuel indicator --------
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
			tileDraw(&rightNeedle);

		//!< Restore the transformation matrix
		glPopMatrix();

		//!< -------- Render temperatur indicator --------
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
			tileDraw(&leftNeedle);

		//!< Restore the transformation matrix
		glPopMatrix();


		//!< -------- Render left blinker --------
		if(flags & DASHBOARD_LEFT)
		{
			glPushMatrix();

				//!< Shift blinker to correct position
				glTranslatef(-0.582f, -0.875f, 0);

				//!< Render the blinker
				tileDraw(&blinker);

			//!< Restore the transformation matrix
			glPopMatrix();
		}

		//!< -------- Render right blinker --------
		if(flags & DASHBOARD_RIGHT)
		{
			glPushMatrix();

				//!< Shift blinker to correct position
				glTranslatef(0.582f, -0.875f, 0);

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

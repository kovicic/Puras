#include <stdio.h>       //!< printf, fprintf
#include <stdlib.h>      //!< exit
#include <unistd.h>      //!< sleep
#include <math.h>        //!< e.g. M_PI, cos(), sin()
#include <pthread.h>	 //!< threads
#include <GLES/gl.h>     //!< OpenGL ES 1.1 date types and functions
#include "../lib/gles.h" //!< struct opengles, gles*-functions
#include "../lib/tile.h" //!< struct tile, loadPngTile

#include "draw.h"
#include "dashboard.h"
#include "input.h"

int main(void)
{
	dashboard_t *db = new dashboard_t();

	//!< Create worker thread
	pthread_t drawNewDesignedThread;
	pthread_t drawOriginalDesignedThread;
	pthread_t inputThread;
	
	pthread_create(&drawNewDesignedThread,  0, drawNewDesignMain,  (void*)db);
	pthread_create(&drawOriginalDesignedThread, 0, drawOriginalDesignMain, (void*)db);
	pthread_create(&inputThread,  0, inputMain,  (void*)db);

	pthread_join(drawNewDesignedThread, 0);
	pthread_join(drawOriginalDesignedThread, 0);

	return 0;
}

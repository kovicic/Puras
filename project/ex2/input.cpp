#include <stdio.h>      //!< printf(), perror()
#include <stdint.h>     //!< uint32_t, ...
#include <stdlib.h>     //!< exit()
#include <string.h>     //!< strcmp()
#include <unistd.h>     //!< write()
#include <errno.h>      //!< errno
#include <math.h>       //!< M_PI
#include <fcntl.h>      //!< O_WRONLY, O_CREAT
#include <net/if.h>     //!< struct ifreq
#include <sys/ioctl.h>  //!< ioctl(), SIOCGIFINDEX
#include <GLES/gl.h>    //!< OpenGL ES 1.1 data types and definitions
#include "canLib/can.h" 	//!< can_open()

#include "input.h"
#include "dashboard.h"
#include <linux/can.h>  //!< PF_CAN, CAN_RAW, struct sockaddr_can

//!< This data structure represents a single entry of the log file
struct log_entry
{
	struct timeval    timeval;
	struct can_frame  frame;
};

void* inputMain(void *data)
{
	dashboard_t *db = (dashboard_t *)data;

	int fd_can;
	int fd_log = 0;
	struct log_entry entry;
	int ret;

	//!< Open socket with the CAN interface provide. 
    //!< Then we can access the interface using read() and write()
	fd_can = can_open("vcan0");

	//!< Open log file. Here we find the data acc. to struct log_entry
	//!< which can be read subsequently using read() function
	fd_log = open(LOG_FILE, O_RDWR, 0660);
	if(fd_log < 0)
	{
		perror("Open file");
	}

	//!< Using the write() function we can send a single message to 
    //!< the CAN interface. The content is determined by our data structure.
	while(1)
	{
		//!< Read data from log file
		ret = read(fd_log, &entry, sizeof(struct log_entry));
		if(ret < 0)
		{
			perror("Read file");
		}
		else if(ret == 0)
		{
			break;
		}

		//!< Parse read data
		canParseBMW(&entry.frame, db);

		ret = write(fd_can, &entry.frame, sizeof(struct can_frame));
		if(ret < 0)
		{
			perror("Write");
		}

		usleep(20*1000);
	}

	return 0;
}

int canParseBMW(struct can_frame *frame, dashboard_t *db)
{
	uint32_t id;
	id = frame->can_id & CAN_EFF_MASK;

	uint8_t *d = &(frame->data[0]);

	switch(id)
	{
		case 0x254:

			db->kmh =
				( (((d[1]*256.0f + d[0]) - 32767.0f) / 50.0f) +
				  (((d[3]*256.0f + d[2]) - 32767.0f) / 50.0f) +
				  (((d[5]*256.0f + d[4]) - 32767.0f) / 50.0f) +
				  (((d[7]*256.0f + d[6]) - 32767.0f) / 50.0f) ) / 4.0f;
		break;
		case 0xa5:
			db->rpm = ((d[6]* 256.0f) + d[5]) / 4.0f;
		break;
		case 0x1f6:
		
			db->lamps &= ~(DASHBOARD_LEFT | DASHBOARD_RIGHT);
		    if(d[0] == 145)
		    {
		    	db->lamps |= DASHBOARD_LEFT;
		    }
			else if(d[0] == 161)
			{
				db->lamps |= DASHBOARD_RIGHT;
			}
		break;

		default:
			break;
	}

	return 0;
}




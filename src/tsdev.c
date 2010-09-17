/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <tslib.h>

#include "utils.h"

#define UINPUT_DEV_PATH1 "/dev/uinput"
#define UINPUT_DEV_PATH2 "/dev/input/uinput"

static int uinput_fd=-1;
int btn=0;

int main(){
	int err=0;
	int fd;

	struct tsdev *ts;
	ts=ts_open("/dev/input/event2",0);
	if(!ts){
		error("tslib open failed\n");
		return -1;
	}
	ts_config(ts);
	fd=ts_fd(ts);
	if (ioctl(fd, EVIOCGRAB, 2) < 0) {
		error("evdev ioctl GRAB error of evdev\n");
	}

	setup_uinput();
	while(1){
		struct ts_sample s;
		ts_read(ts,&s,1);
		send_key_uinput(EV_ABS, ABS_Y, 480 - s.x);
		send_key_uinput(EV_ABS, ABS_X, s.y);
		int p=s.pressure?1:0;
		if(p!=btn)
		{
			btn=p;
			send_key_uinput(EV_KEY, BTN_LEFT, btn);
	//		printf("BTN_LEFT=%d\n",btn);
		}
		send_key_uinput(EV_SYN, 0, 0);
	//	printf("read %d %d %d\n",s.x,s.y,s.pressure);
	}
}


int send_key_uinput(int type,int code, int value){
	struct input_event event;
	
	gettimeofday(&event.time, NULL);
	event.type = type;
	event.code = code;
	event.value = value;

	if(write(uinput_fd, &event, sizeof(event))<0){
		error("Error send key: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int setup_uinput(void){
	struct uinput_user_dev uinp;
	uinput_fd=open(UINPUT_DEV_PATH1, O_WRONLY|O_NDELAY);
	if(uinput_fd<0){
		uinput_fd=open(UINPUT_DEV_PATH2, O_WRONLY|O_NDELAY);
		if(uinput_fd<0){
			goto error;
		}
	}

	// create the device
	memset(&uinp,0,sizeof(uinp)); // Intialize the uInput device to NULL
	strncpy(uinp.name, "TSDev", UINPUT_MAX_NAME_SIZE);
	uinp.id.version = 4;
	uinp.id.bustype = BUS_VIRTUAL;
	uinp.absmin[ABS_X]=0;
	uinp.absmax[ABS_X]=800;
	uinp.absmin[ABS_Y]=0;
        uinp.absmax[ABS_Y]=480;

	write(uinput_fd, &uinp, sizeof(uinp));
	if (ioctl(uinput_fd, UI_SET_EVBIT, EV_KEY))
	{
		goto error;
	}
	if (ioctl(uinput_fd, UI_SET_EVBIT, EV_ABS))
        {
                goto error;
        }
	if (ioctl(uinput_fd, UI_SET_EVBIT, EV_SYN))
        {
                goto error;
        }

	ioctl(uinput_fd, UI_SET_KEYBIT, BTN_LEFT);
	ioctl(uinput_fd, UI_SET_ABSBIT, ABS_X);
	ioctl(uinput_fd, UI_SET_ABSBIT, ABS_Y);

	if (ioctl(uinput_fd, UI_DEV_CREATE))
	{
		     goto error;
	}
	
	return 0;
error:
	error("Initialization error of uinput: %s\n", strerror(errno));
	return -1;
}

void clear_uinput(void){
	if(uinput_fd>=0){
		ioctl(uinput_fd, UI_DEV_DESTROY);
		close(uinput_fd);
	}
}


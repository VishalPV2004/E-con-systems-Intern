#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>


#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
        void   *start;
        size_t  length;
	unsigned long timestamp;
};

struct v4l2_app_data {
	char            dev_name[20];
	struct buffer   *buffers;
	unsigned int     n_buffers;
	int              fd ;
};


int v4l2_init_capture(struct v4l2_app_data *v4l2_data, int width, int height);
void v4l2_deinit_capture(struct v4l2_app_data *v4l2_data);


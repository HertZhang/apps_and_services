/*
Hardware:
	EIM_D29-->PWR_BTN_SNS	
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/types.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/time.h>

//#include "backlight_key.h"

#define PWR_KEY_DEV_PATH		"/dev/input/event2"			// power button event

#if 0
#define info_level 0
int debug = 0;
#define print(fmt, ...) \
	do { if (debug > info_level) printf(fmt"\r\n", ##__VA_ARGS__); } while (0)

enum{
        F_READ = 0,
	F_WRITE = 1,
};

struct LED_INFO {
	unsigned int mode;
	char *brightness;
};

struct LED_INFO led_info = {
"500",
};
#endif

//static int i2c_keep_sending_data(unsigned int sleep);

static void help_information(char *exe_name)
{
	printf( \
" Usage:\n" \
"	 %s <als> <enable=1>\n" \
"	 %s <dim> <max=1> <percen=88>\n" \
"	 %s <test> <sleep=1000>\n" \
" Value range:\n" \
"	 enable = 0(disable), 1(enable).\n" \
"	 max = 0(min), 1(max).\n" \
"	 percen = [0, 100], dimming duty cycle.\n" \
"	 sleep = [100, +oo], in us.\n" \
" Example:\n" \
"	 %s als 1\n" \
"	 %s dim 1 88\n" \
"	 %s test 1000\n" \
	, exe_name, exe_name, exe_name, exe_name, exe_name, exe_name);
}

int main(int argc, char *argv[])
{
	int fd = -1;
	int ret = -1;
	unsigned int count = 0;
	unsigned char buffer[128] = {0};
	int i = 0, j = 0;
	struct input_event *event;
	
	if (argc <= 1) {
		goto error;
	}
#if 0	
	if(!strcmp(argv[1], "als")) {
		printf("als=%u\n", atoi(argv[2]));
        	ALS_Enable(atoi(argv[2]));
        } else if (!strcmp(argv[1], "dim")) {
		printf("max=%u, percent=%u\n", atoi(argv[2]), atoi(argv[3]));
                set_min_or_max_dimming_duty_cycle(atoi(argv[2]), atoi(argv[3]));
        } else if (!strcmp(argv[1], "test")) {
		printf("sleep=%u\n", atoi(argv[2]));
                i2c_keep_sending_data(atoi(argv[2]));
        } else {
                goto error;
        }
#endif

	fd = open(PWR_KEY_DEV_PATH, O_RDWR);
        if(fd < 0) {
                perror("open power key error!\n");
                return -1;
        }

        while(1) {
                ret = read(fd, );
                if(ret < 0) {
                        printf("keep_sending_data() write error!\n");
                }

                printf("count=%u\n", count);
                count ++;
                usleep(sleep); // sleep s
        }

        close(fd);

#if 0
	while(1) {
		printf("count = %d\n", count);
		usleep(1000000); // sleep 3s
		count ++;
	}
#endif
	return 0;
	
error:
    	help_information(argv[0]);

	return 0;
}




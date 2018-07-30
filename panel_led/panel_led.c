/*
Hardware: 
LED1(Green):	NANDF_D2(GPIO2_IO02)-->GPIO_LED1-->LED1
LED2(Red):	NANDF_D3(GPIO2_IO03)-->GPIO_LED2-->LED2

Definition: 
系统启动阶段的LED显示状态为：
Amber Solid : Operating system starting
系统启动完成后的LED显示状态为（优先级由上到下降低）：
RED Blinking : System over temperature
GREEN Solid : Normal operation
GREEN Blinking : Backlight Off

# ls panel_led_green
	brightness     max_brightness  subsystem       uevent
	device          power           trigger
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

//#include "panel_led.h"

#define SYSTEM_OVER_TEMPERATURE         99//44
#define MAX_SLEEK_FILTER_COUNT 		6
#define info_level 0
int debug = 0;
#define print(fmt, ...) \
	do { if (debug > info_level) printf(fmt"\r\n", ##__VA_ARGS__); } while (0)

enum{
	SYSTEM_START = 0,
        OVER_TEMPERATURE,
        NORMAL_OPERATION,
        BACKLINGHT_OFF,
	DEBUG,
};

enum{
        SOLID = 0,
        BLINK,
};

enum{
        GREEN = 0,
        RED,
	CHARGER,
};

struct LED_INFO {
	unsigned int mode;
	unsigned int last_mode;
	unsigned int number;
	unsigned int sys_status;
	unsigned int last_sys_status;
	char *brightness;
	char *delay_on;
	char *delay_off;
};

struct LED_INFO led_info = {
SOLID,
255,
GREEN,
NORMAL_OPERATION,
255,
"1",
"500",
"500",
};

static int set_led_blinking(unsigned int number, char *delay_on, char *delay_off);
static int set_led_brightness(unsigned int number, char *value);
static int get_system_temperature(void);
static int get_panel_backlight_status(void);
static float sleek_filter(float raw_data, int filter_count);

static void help_information(char *exe_name)
{
	printf( \
" Usage:\n" \
" Set working status or mode:\n" \
"   (1) ./panel_led (normal working in default)\n" \
"   (2) ./panel_led system_start (force into system start status)\n" \
"   (3) ./panel_led normal_operation (force into normal operation status)\n" \
"   (4) ./panel_led over_temprature (force into over temperature status)\n" \
"   (5) ./panel_led backligt_off (force into backligt_off status)\n" \
"   (6) You can enter the debug mode with the following instructions:\n" \
"	 ./panel_led <green/red/charger> <bright> <value=1>\n" \
"	 or\n" \
"	 ./panel_led <green/red/charger> <blink> <delay_on=500> <delay_of=500>\n" \
"     Value range:\n" \
"	 value = [0, 255].\n" \
"	 delay_on = [0, 65535], in ms.\n" \
"	 delay_off = [0, 65535], in ms.\n" \
"     Example:\n" \
"	 ./panel_led charger bright 1\n" \
"	 ./panel_led charger blink 500 500\n" \
	);
}

int main(int argc, char *argv[])
{
	int fd_green = -1, fd_red = -1;
	unsigned int count = 0;
	
	if (argc > 1) {
		if(!strcmp(argv[1], "system_start")) {
                        led_info.sys_status = SYSTEM_START;
		} else if(!strcmp(argv[1], "normal_operation")) {
                        led_info.sys_status = NORMAL_OPERATION;
                } else if (!strcmp(argv[1], "over_temprature")) {
                        led_info.sys_status = OVER_TEMPERATURE;
		} else if (!strcmp(argv[1], "backligt_off")) {
                        led_info.sys_status = BACKLINGHT_OFF;
                } else {
             		led_info.sys_status = DEBUG;

			if(!strcmp(argv[1], "green")) {
                	        led_info.number = GREEN;
                	} else if (!strcmp(argv[1], "red")) {
                	        led_info.number = RED;
                	} else if (!strcmp(argv[1], "charger")) {
                	        led_info.number = CHARGER;
                	} else {
                	        goto error;
                	}
                	// printf("led: %s\n", argv[1]);
                }

		if(argc == 4) {
			if(!strcmp(argv[2], "bright")) {
				int value = atoi(argv[3]);
				led_info.mode = SOLID;
			
				if(value >= 0 && value < 256) {
					led_info.brightness = argv[3];
					//printf("brightness: %s\n", led_info.brightness);
				} else {
					printf(" Please check the range of values in [0,255]!\n");
					goto error;
				}
			} else {
				goto error;
			}
	    	} else if(argc == 5) {
			if(!strcmp(argv[2], "blink")) {
				int value = atoi(argv[3]);
	                        led_info.mode = BLINK;
	
	                        if(value >= 0 && value < 65536) {
	                                led_info.delay_on = argv[3];
					//printf("delay_on: %s\n", led_info.delay_on);
	                        } else {
					printf(" Please check the range of values in [0,65535]!\n");
	                                goto error;
				}
				
				value = atoi(argv[4]);
				if(value >= 0 && value < 65536) {
	                                led_info.delay_off = argv[4];
					//printf("delay_off: %s\n", led_info.delay_off);
	                        } else {
					printf(" Please check the range of values in [0,65535]!\n");
	                                goto error;
				}
	                } else {
	                        goto error;
	                }
		}
	}
	
	while(1) {
		//printf("count = %d\n", count);

		// status detection
		if((led_info.sys_status != DEBUG) && (led_info.sys_status != SYSTEM_START) && (argc < 2)) {
			int temp = get_system_temperature();
			if(temp > SYSTEM_OVER_TEMPERATURE) { // teperature is over 99C trigger warning!
				led_info.sys_status = OVER_TEMPERATURE;
				//printf("cpu temperature is over than 99C: %d\n", temp);
			} else {
				led_info.sys_status = NORMAL_OPERATION;
			}
			//printf("current led_info.sys_status: %d\n", led_info.sys_status);
		}

		switch(led_info.sys_status) {
			case SYSTEM_START:
                                if(led_info.last_sys_status != led_info.sys_status) {
                                        led_info.last_sys_status = led_info.sys_status;
                                        set_led_brightness(GREEN, "1");
                                        set_led_brightness(RED, "1");
					//printf("SYSTEM_START\n");
                                }
                                break;
			case NORMAL_OPERATION:
				if(led_info.last_sys_status != led_info.sys_status) {
                			led_info.last_sys_status = led_info.sys_status;
					set_led_brightness(GREEN, "1");
					set_led_brightness(RED, "0");
					//printf("NORMAL_OPERATION\n");
				}
				break;
			case OVER_TEMPERATURE:
				 if(led_info.last_sys_status != led_info.sys_status) {
                                        led_info.last_sys_status = led_info.sys_status;
					set_led_brightness(GREEN, "0");
					set_led_blinking(RED, "500", "500");
					//printf("OVER_TEMPERATURE\n");
				}
				break;
			case BACKLINGHT_OFF:
				if(led_info.last_sys_status != led_info.sys_status) {
                                        led_info.last_sys_status = led_info.sys_status;
                                        set_led_blinking(GREEN, "500", "500");
                                        set_led_brightness(RED, "0");
					//printf("BACKLINGHT_OFF\n");
                                }
				break;
			case DEBUG:
				if(led_info.last_mode != led_info.mode) {
        	                	led_info.last_mode = led_info.mode;

        	                	if(led_info.mode == SOLID) {
        	                	        set_led_brightness(led_info.number, led_info.brightness);
        	                	} else if(led_info.mode == BLINK) {
        	                	        set_led_blinking(led_info.number, led_info.delay_on, led_info.delay_off);
        	                	}
        	        	}
				break;
			default:
				//printf("defualt\n");
				break;
		}

		usleep(500000); // sleep 500ms
		//count ++;
	}

	return 0;
	
error:
    	help_information(argv[0]);

	return 0;
}

static int device_file_node_write_string(char *dev_name, char *file_name, char *value)
{
        int fd = -1;
        char path_name[48] = {0};

        snprintf(path_name, sizeof(path_name), "/sys/class/leds/%s/%s", dev_name, file_name);
	//printf("path_name: %s\n", path_name);

        fd = open(path_name, O_WRONLY);
        if (fd < 0) {
                printf("Can't open %s!\n", path_name);
                return -1;
        }
	
	//printf("value: %s\n", value);
        write(fd, value, strlen(value) + 1);

        close(fd);

        return 0;
}

static int set_led_blinking(unsigned int number, char *delay_on, char *delay_off)
{
        switch(number){
                case GREEN:
			if(device_file_node_write_string("panel_led_green", "trigger", "timer") < 0)
                                return -1;

			if(device_file_node_write_string("panel_led_green", "delay_on", delay_on) < 0)
				return -1;
			
			if(device_file_node_write_string("panel_led_green", "delay_off", delay_off) < 0)
				return -1;
			
                        break;
                case RED:
			if(device_file_node_write_string("panel_led_red", "trigger", "timer") < 0)
                                return -1;

                        if(device_file_node_write_string("panel_led_red", "delay_on", delay_on) < 0)
                                return -1;

                        if(device_file_node_write_string("panel_led_red", "delay_off", delay_off) < 0)
                                return -1;
			
			break;
		case CHARGER:
			if(device_file_node_write_string("charger-led", "trigger", "timer") < 0)
                                return -1;
			
                        if(device_file_node_write_string("charger-led", "delay_on", delay_on) < 0)
                                return -1;

                        if(device_file_node_write_string("charger-led", "delay_off", delay_off) < 0)
                                return -1;

                        break;
                default:
                        printf("Set led blinking error!\n");
        }

        return 0;
}

static int set_led_brightness(unsigned int number, char *brightness)
{
	int return_value = -1;

	//printf("Set brightness: %s\n", brightness);

	switch(number){
                case GREEN:
                        return_value = device_file_node_write_string("panel_led_green", "brightness", brightness);
			break;
                case RED:
                        return_value = device_file_node_write_string("panel_led_red", "brightness", brightness);
			break;
		case CHARGER:
			return_value = device_file_node_write_string("charger-led", "brightness", brightness);
                        break;
                default:
                        printf("Set led brightness error!\n");
        }

	return return_value;
}

static int get_system_temperature(void)
{
	int fd = -1;
	char rec_buf[7] = {0};
	int temperature = 0;

        fd = open("/sys/class/thermal/thermal_zone0/temp", O_RDONLY);
        if (fd < 0) {
                printf("Can't open /sys/class/thermal/thermal_zone0/temp!\n");
                return -1;
        }

        read(fd, rec_buf, sizeof(rec_buf) - 1);

        close(fd);
	
	temperature = (int)sleek_filter((atoi(rec_buf) / 1000), MAX_SLEEK_FILTER_COUNT);
	//printf("cpu temperature is: %d\n", temperature);

        return temperature;
}

static int get_panel_backlight_status(void)
{
	int fd = -1;
        char rec_buf[7] = {0};

        fd = open("/sys/class/leds/backlight_enable", O_RDONLY);
        if (fd < 0) {
                printf("Can't open /sys/class/leds/backlight_enable!\n");
                return -1;
        }

        read(fd, rec_buf, sizeof(rec_buf) - 1);

        close(fd);

        //printf("backlight status is: %d\n", atoi(rec_buf));

        return atoi(rec_buf);
}

static float sleek_filter(float raw_data, int filter_count)   
{
	char count ;
	static char i = 0;
	static float temp_value[MAX_SLEEK_FILTER_COUNT];
	double sum = 0;

	if(i < filter_count)
		temp_value[i++] = raw_data;
	else
		i = 0;
	 
	for(count=0; count<filter_count; count++ )
	{
		sum += temp_value[count];
	}
	 
	return (sum / filter_count);
}










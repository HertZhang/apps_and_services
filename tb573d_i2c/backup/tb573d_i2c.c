/*
Hardware:
	GPIO_3-->I2C3_SCL-->
	GPIO_6-->I2C3_SDA-->

I2C speed:
	100KHz for all I2C.

I2C2 Address:
	EEPROM(EDID): 0x50
	PMIC: 0x08
I2C3 Address:
	MCU(tb573d): 0x66
	DS1337S: 0x68
I2C4 Address:
        EEPROM: 0x05

Protocal:
	
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

#include "i2c_data.h"
//#include "tb573d_i2c.h"

#define info_level 0
int debug = 0;
#define print(fmt, ...) \
	do { if (debug > info_level) printf(fmt"\r\n", ##__VA_ARGS__); } while (0)

#if 0
enum{
        GREEN = 0,
};

struct LED_INFO {
	unsigned int mode;
	char *brightness;
};

struct LED_INFO led_info = {
"500",
};
#endif

static void help_information(char *exe_name)
{
	printf( \
" Usage:\n" \
"	 %s <read/write> <length> <freq> <hertz>\n" \
" Value range:\n" \
"	 length = [1, +oo].\n" \
"	 hertz = [0, +oo], in hertz.\n" \
" Example:\n" \
"	 %s write 1 freq 0\n" \
"	 %s read 1 freq 0\n" \
	, exe_name, exe_name, exe_name);
}

int main(int argc, char *argv[])
{
	int fd = -1;
	int ret = -1;
	unsigned int count = 0;
	unsigned char buffer[128] = {0};
	int i = 0, j = 0;
	
#if 0
	if (argc <= 1) {
		goto error;
	}
	
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

	if(argc == 4) {
		if(!strcmp(argv[2], "bright")) {
			int value = atoi(argv[3]);
			led_info.mode = SOLID;
		
			if(value >= 0 && value < 256) {
				led_info.brightness = argv[3];
				printf("brightness: %s\n", led_info.brightness);
			} else {
				printf(" Please check the range of values in [0,255]!\n");
				goto error;
			}
		} else {
			goto error;
		}
    	}
#endif	
	fd = open("/dev/i2c-1",O_RDWR); // EDID
	//fd = open("/dev/i2c-2",O_RDWR); // MCU
	//fd = open("/dev/i2c-3",O_RDWR); // EEPROM
	if(fd < 0)
    	{
        	perror("open error\n");
    	}

	ret = i2c_data_read_str(fd, 0x50, 0, buffer, array_size(buffer));
	if(ret < 0)
		printf("i2c_data_read_str() error!\n");
	else {
		printf("\n");
                printf("EDID eeprom memory content at address[0, 127]:\n");
                for(i = 0; i <= 0x07; i++){
                        for(j = 0; j <= 0x0f; j++){
                                printf("%02X ", buffer[i*0x10 + j]);
                        }
                        printf("\n");
                }
		printf("\n");
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



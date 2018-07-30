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

#define I2C_DEVICE_PATH_TEST    "/dev/i2c-0"            // test i2c1
#define I2C_ADDRESS_TEST        0x15
#define I2C_DEVICE_PATH 	"/dev/i2c-2"		// "/dev/i2c-1"=EDID  "/dev/i2c-2"=MCU  "/dev/i2c-3"=EEPROM
#define I2C_ADDRESS		0x66			// 0x66=MCU  0x50=EDID
#define REG_ALS_EN		0x01			// ALS Enable or Disable
#define REG_DIM_MIN		0x02			// Minimum dimming duty cycle
#define REG_DIM_MAX		0x03			// Maximum dimming duty cycle

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

static int ALS_Enable(unsigned char enable);
static int set_min_or_max_dimming_duty_cycle(unsigned char set_max, unsigned char percentage);
static int i2c_keep_sending_data(unsigned int sleep);

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
	
	if (argc <= 1) {
		goto error;
	}
	
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

static int ALS_Enable(unsigned char enable)
{
	int ret = -1;
	int fd = -1;
	unsigned char buffer[1] = {0};
	
	printf("device path = %s\n", I2C_DEVICE_PATH);
	//printf("i2c address = %02X\n", I2C_ADDRESS);

	fd = open(I2C_DEVICE_PATH, O_RDWR);
        if(fd < 0) {
                perror("open error\n");
		return -1;
	}

	enable = (enable == 0) ? 0 : 1;
	buffer[0] = enable;
	ret = i2c_data_write_str(fd, I2C_ADDRESS, REG_ALS_EN, buffer, array_size(buffer));
	if(ret < 0) {
                printf("ALS_Enable() write error!\n");
        }
	else
		printf("write ALS value = %u\n", buffer[0]);
	close(fd);
	
	usleep(1000000);

	// read again
	fd = open(I2C_DEVICE_PATH, O_RDWR);
	ret = i2c_data_read_str(fd, I2C_ADDRESS, REG_ALS_EN, buffer, array_size(buffer));
        if(ret < 0) {
                printf("ALS_Enable() read error!\n");
	} 
	else 
		printf("read ALS value = %u\n", buffer[0]);
	
	close(fd);
	
	if(buffer[0] != enable) {
		ret = -1;
		printf("ALS_Enable() (buffer != enable) error!\n");
	}

	return ret;
}

static int set_min_or_max_dimming_duty_cycle(unsigned char set_max, unsigned char percentage)
{
	int ret = -1;
	int fd = -1;
        unsigned char buffer[1] = {0};
	unsigned char reg_addr = (set_max == 0) ? REG_DIM_MIN : REG_DIM_MAX;

	printf("device path = %s\n", I2C_DEVICE_PATH);
	//printf("i2c address = 0x%02X\n", I2C_ADDRESS);

	if(set_max == 0) { 
		if(percentage > 20) {
			printf("Please keep the percen in range [0, 20] \n");
			return -1;
		}
	} else {
		if(percentage < 20 || percentage > 100) {
                        printf("Please keep the percen in range [21, 100] \n");
                        return -1;
                }
	}

        buffer[0] = percentage;
	
	fd = open(I2C_DEVICE_PATH, O_RDWR);
        if(fd < 0) {
                perror("open error\n");
                return -1;
        }
	//
	ret = i2c_data_write_str(fd, I2C_ADDRESS, reg_addr, buffer, array_size(buffer));
	if(ret < 0)
                printf("set_min_or_max_dimming_duty_cycle() (REG_DIM) write error!\n");
	else 
	{
		if(reg_addr == REG_DIM_MIN)
			printf("write REG_DIM_MIN value = %u\n", buffer[0]);
		else
			printf("write REG_DIM_MAX value = %u\n", buffer[0]);
	}
	//
	close(fd);
	
	usleep(1000000);

	// read again
	fd = open(I2C_DEVICE_PATH, O_RDWR);
	if(fd < 0) {
        	perror("open error\n");
        	return -1;
	}
	//
        ret = i2c_data_read_str(fd, I2C_ADDRESS, reg_addr, buffer, array_size(buffer));
        if(ret < 0)
                printf("set_min_or_max_dimming_duty_cycle() (REG_DIM) read error!\n");
	else
	{
		if(reg_addr == REG_DIM_MIN)
                	printf("read REG_DIM_MIN value = %u\n", buffer[0]);
        	else
                	printf("read REG_DIM_MAX value = %u\n", buffer[0]);
	}
	//
	close(fd);

        if(buffer[0] != percentage) {
                ret = -1;
                printf("set_min_or_max_dimming_duty_cycle() (buffer != percentage) error!\n");
        }
	
        return ret;
}

static int i2c_keep_sending_data(unsigned int sleep)
{
        int ret = -1;
	int fd = -1;
        unsigned char buffer[1] = {0xf5};
	unsigned int count = 0;
	
	if(sleep < 100)
		sleep = 100;

	printf("sleep time=%uus\n", sleep);
	printf("I2C_DEVICE_PATH_TEST=%s\n", I2C_DEVICE_PATH_TEST);
	printf("I2C_ADDRESS_TEST=%u, REGISTER=%u\n", I2C_ADDRESS_TEST, REG_ALS_EN);
	printf("send buffer[0]=%u\n", buffer[0]);

        fd = open(I2C_DEVICE_PATH_TEST, O_RDWR);
        if(fd < 0) {
                perror("open error\n");
                return -1;
        }

        while(1) {
		ret = i2c_data_write_str(fd, I2C_ADDRESS_TEST, REG_ALS_EN, buffer, array_size(buffer));
	        if(ret < 0) {
	                printf("keep_sending_data() write error!\n");
	        }
		
		printf("count=%u\n", count);
		count ++;
		usleep(sleep); // sleep s
	}

        close(fd);

        return ret;
}




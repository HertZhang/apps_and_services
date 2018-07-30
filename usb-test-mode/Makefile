#Simple makefile for libusb application
# this is a proper pound sign! jolly good!

#CC = arm-linux-gnueabihf-gcc
CC = /home/aplex/sbc7819/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc

INC=-I./libusb-1.0 -I.
DEPENDENCIES := -lpthread -lrt
CFLAGS=libusb-1.0.a $(DEPENDENCIES) -Wall -Wextra
OBJ=testmode.o

testmode: $(OBJ)
	$(CC) $(OBJ) -o testmode $(CFLAGS)

testmode.o: testmode.c
	$(CC) -c testmode.c $(INC)

clean :
	-rm *.o $(objects) testmode

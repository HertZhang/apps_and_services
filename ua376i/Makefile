#CC=/home/myzr/myandroid/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-gcc 
CC= arm-linux-gnueabihf-gcc
#CC=gcc
#CC=arm-linux-gcc
UNAME := $(shell uname)
# Assume target is Mac OS if build host is Mac OS; any other host targets Linux
ifeq ($(UNAME), Darwin)
	DEPENDENCIES := -lpthread -lobjc -framework IOKit -framework CoreFoundation
else
	DEPENDENCIES := -lpthread -lrt
endif
CFLAGS=libftd2xx.a libtest.so $(DEPENDENCIES) -Wall -Wextra
#CFLAGS=libftd2xx.a  $(DEPENDENCIES) -Wall -Wextra
APP = read_ua376

all: $(APP)

$(APP): read_ua376
	#$(CC) read_ua376_main.c ua376_linux.c -o $(APP) $(CFLAGS)
	$(CC) read_ua376_main.c  -o $(APP) $(CFLAGS) 
#	$(CC) read_ua327_main.c ua327t.c -o $(APP) $(CFLAGS)
clean:
	-rm -f *.o ; rm $(APP)

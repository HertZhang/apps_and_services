cc = arm-linux-gnueabihf-gcc
target = backlight_key
obj = backlight_key.o

$(target) : $(obj)
	$(cc) $(obj) -Wall -o $(target)
	
$(obj) : backlight_key.c
	$(cc) -c backlight_key.c

.PHONY : clean
clean:
	rm -f *.o $(target)

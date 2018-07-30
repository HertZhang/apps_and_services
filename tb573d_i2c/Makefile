cc = arm-linux-gnueabihf-gcc
target = tb573d_i2c
obj = tb573d_i2c.o i2c_data.o

tb573d_i2c : $(obj)
	$(cc) $(obj) -Wall -o $(target)
	
tb573d_i2c.o : tb573d_i2c.c
	$(cc) -c tb573d_i2c.c

i2c_data.o : i2c_data.c i2c_data.h i2c.h i2c-dev.h
	$(cc) -c i2c_data.c

.PHONY : clean
clean:
	rm -f *.o $(target)

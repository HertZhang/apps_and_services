cc = arm-linux-gnueabihf-gcc

target1 = client
src1 = client.c
obj1 = client.o
#
target2 = server
src2 = server.c
obj2 = server.o

all: $(target1) $(target2)

$(target1) : $(obj1)
	$(cc) $(obj1) -Wall -o $(target1)
#
$(obj1) : $(src1)
	$(cc) -c $(src1)

$(target2) : $(obj2)
	$(cc) $(obj2) -Wall -o $(target2)
#
$(obj2) : $(src2)
	$(cc) -c $(src2)

.PHONY : clean
clean:
	rm -f *.o $(target1) $(target2)



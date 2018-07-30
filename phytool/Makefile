.PHONY: all clean install dist

# Top directory for building complete system, fall back to this directory
ROOTDIR    ?= $(shell pwd)

VERSION = 2
NAME    = phytool
PKG     = $(NAME)-$(VERSION)
ARCHIVE = $(PKG).tar.xz
APPLETS = mv6tool

#CC = arm-linux-gnueabihf-gcc
#CC = /home/aplex/sbc7819/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
#CC = /home/aplex/sbc7819/toolchain/gcc-linaro-7.3.1-2018.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
#CC = /home/aplex/sbc7819/toolchain/gcc-linaro-6.3.1-2017.05-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CC = $(CROSS_COMPILE)gcc
#CC = CC=/home/aplex/sbc7819/fsl-release-bsp/imx6q-x11/tmp/sysroots/x86_64-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc

PREFIX ?= /usr/local/
CFLAGS ?= -Wall -Wextra -Werror
LDLIBS  = 

#INC=/home/aplex/sbc7819/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/include/

objs = $(patsubst %.c, %.o, $(wildcard *.c))
hdrs = $(wildcard *.h)

%.o: %.c $(hdrs) Makefile
	@printf "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(CFLAGS) -c $< -o $@ #-I$(INC)

phytool: $(objs)
	@printf "  CC      $(subst $(ROOTDIR)/,,$(shell pwd)/$@)\n"
	@$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

all: phytool

clean:
	@rm -f *.o
	@rm -f phytool

dist:
	@echo "Creating $(ARCHIVE), with $(ARCHIVE).md5 in parent dir ..."
	@git archive --format=tar --prefix=$(PKG)/ v$(VERSION) | xz >../$(ARCHIVE)
	@(cd .. && md5sum $(ARCHIVE) > $(ARCHIVE).md5)

install: phytool
	@cp phytool $(DESTDIR)/$(PREFIX)/bin/
	@for app in $(APPLETS); do \
		ln -sf phytool $(DESTDIR)/$(PREFIX)/bin/$$app; \
	done

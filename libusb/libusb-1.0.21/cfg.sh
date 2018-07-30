#! /bin/sh

#arm-linux-gnueabihf
#arm-linux
#x86_64-pc-linux-gnu
#i686-linux

sudo ./configure --build=x86_64-pc-linux-gnu --host=arm-linux-gnueabihf --prefix=$PWD/install --disable-udev CC=/home/aplex/sbc7819/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc CXX=/home/aplex/sbc7819/toolchain/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++

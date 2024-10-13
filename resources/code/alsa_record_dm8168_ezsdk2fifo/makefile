####### Compiler, tools and options

CC            = arm-none-linux-gnueabi-gcc
CXX           = arm-none-linux-gnueabi-g++
CFLAGS        = -pipe -fno-exceptions -O3 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -Wall -W 
CXXFLAGS      = -pipe -fno-exceptions -O3 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -Wall -W 
INCPATH       = -I/home/user/ti-ezsdk_dm816x-evm_5_05_01_04/linux-devkit/arm-none-linux-gnueabi/usr/include/
LINK          = arm-none-linux-gnueabi-g++
LFLAGS        = -Wl,-rpath-link,-O1,-rpath
LIBS          = -L /home/user/ti-ezsdk_dm816x-evm_5_05_01_04/linux-devkit/arm-none-linux-gnueabi/usr/lib  -lasound 
#LIBS	      = -L /home/user/alsa/lib  -lasound

#cc = $(CROSS_COMPILE)gcc
alsa02fifo.o: alsa02fifo.c
	$(CC) -c alsa02fifo.c $(CFLAGS) $(INCPATH)
alsa02fifo: alsa02fifo.o
	$(CC) -o alsa02fifo alsa02fifo.o $(LIBS)
clean:
	rm -f *.o
install:
	cp -a alsa02fifo /home/user/ti-ezsdk_dm816x-evm_5_05_01_04/filesystem/ezsdk-dm816x-evm-rootfs/home/root/audio_from_fifo2aplay/alsa02fifo


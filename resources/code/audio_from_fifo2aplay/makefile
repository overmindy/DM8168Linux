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
audio_from_fifo2aplay.o: audio_from_fifo2aplay.c
	$(CC) -c audio_from_fifo2aplay.c $(CFLAGS) $(INCPATH)
audio_from_fifo2aplay: audio_from_fifo2aplay.o
	$(CC) -o audio_from_fifo2aplay audio_from_fifo2aplay.o $(LIBS)
clean:
	rm -f *.o
install:
	cp -a run.sh /home/user/ti-ezsdk_dm816x-evm_5_05_01_04/filesystem/ezsdk-dm816x-evm-rootfs/home/root/audio_from_fifo2aplay/run.sh
	cp -a audio_from_fifo2aplay /home/user/ti-ezsdk_dm816x-evm_5_05_01_04/filesystem/ezsdk-dm816x-evm-rootfs/home/root/audio_from_fifo2aplay/audio_from_fifo2aplay


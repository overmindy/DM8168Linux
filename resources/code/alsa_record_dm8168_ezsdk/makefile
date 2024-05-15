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
alsa02.o: alsa02.c
	$(CC) -c alsa02.c $(CFLAGS) $(INCPATH)
alsa02: alsa02.o
	$(CC) -o alsa02 alsa02.o $(LIBS)
clean:
	rm -f *.o


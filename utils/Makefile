# 定义编译器和编译选项  
CC = gcc  
CFLAGS = -Wall  
  
# 定义源文件和目标文件  
SRCS = a.c uniform.c  
OBJS = $(SRCS:.c=.o)  
TARGET = my_program  
  
# 定义头文件  
HEADERS = uniform.h  
  
# 链接所有目标文件生成最终的可执行文件  
$(TARGET): $(OBJS)  
	$(CC) $(CFLAGS) -o $@ $^  
  
# 编译每个源文件生成目标文件  
%.o: %.c $(HEADERS)  
	$(CC) $(CFLAGS) -c $< -o $@  
  
# 清除生成的目标文件和可执行文件  
clean:  
	rm -f $(OBJS) $(TARGET)  
  
# 打印帮助信息  
help:  
	@echo "Makefile usage:"  
	@echo "  make        - compile and link all source files"  
	@echo "  make clean  - remove all generated files"  
	@echo "  make help   - print this help message"

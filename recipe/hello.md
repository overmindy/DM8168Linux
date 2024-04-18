## hello

- 经过uboot启动后，在cutecom中发送root登陆

```
#cutecom发送命令，建立一个文件夹用来练习
$$ mkdir yourname
```

- 在里面自己写好一个heloo
- 打开Linux终端，编译hello

```shell
$ gcc -o main ./hello.c
$ ./main
```
- 在开发板上运行main，报错
```
./main: line 1: syntax error: word unexpected (expecting ")")
```
- 这里要使用交叉编译,先配置一下
```shell
#将之前下载的拷贝一下
$ cp -r /home/elainay/CodeSourcery /usr/local
$ cd /usr/local
$ vim /etc/bash.bashrc
#在最后添加
PATH=$PATH:/usr/local/CodeSourcery/Sourcery_G++_Lite/bin
#重启使其生效
$ source  /etc/bash.bashrc
#检查是否输出显示版本号
$ arm-none-linux-gnueabi-gcc -v
```
- 没问题后重新编译
- 总之把这个编译库添加到PATH环境
```shell
$ arm-none-linux-gnueabi-gcc -g -o hello ./hello.c
#在开发板上重新运行
$$ ./hello
#输出
Hello,World!
```
- 安装32位程序开发依赖
```shell
sudo apt-get install gcc-multilib g++-multilib
```
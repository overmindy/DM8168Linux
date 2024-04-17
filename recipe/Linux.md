# 电脑环境配置
## 安装Linux系统
- 【最详细安装双系统教程（适用于uefi启动模式） - CSDN App】http://t.csdnimg.cn/UCYM7
- 之前装过一次，这个教程是我见过很不错的了
- 建议efi这种分区空间多给点，不然后面很容易报错
## 设置root密码
```shell
$ sudo passwd
```
## 设置镜像
```shell
#在/etc/apt/source.list中选择最佳服务器，然后执行以下命令
$ apt-get update
```
## 下载软件
```shell
#查看当前Linux版本
$ uname -a
#ubuntu则在官网下载x86 .deb文件安装软件，在终端执行以下命令
$ dpkg -i ****.deb
#安装git(https://cn.linux-console.net/)
$ apt install git-all
#git配置（https://www.cnblogs.com/OnlyAR/p/16155406.html）
```
我是华硕电脑，还安装了一个类似[Linux的奥创中心](https://asus-linux.org/)
## 下载包
```shell
#很多报错是缺少包，直接安装
$ apt install **
#uninstall
$ ape-get remove **
```
## 解压文件
```shell
$ tar -xvf **.tar
$ tar -zxvf **.tar.gz
```
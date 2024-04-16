# 电脑环境配置
## 安装Linux系统
- 【最详细安装双系统教程（适用于uefi启动模式） - CSDN App】http://t.csdnimg.cn/UCYM7
- 装过两三次了，这个教程是我见过最好的了
- 建议某些空间多给点，不然后面很容易报错
## 设置root密码
```shell
$sudo passwd
```
## 设置镜像
```
#在/etc/apt。source.list中选择最佳服务器，然后执行以下命令
$apt-get update
```
## 下载软件
```shell
#查看当前Linux版本
$uname -a
#ubuntu则在官网下载x86 .deb文件安装软件，在终端执行以下命令
$dpkg -i ****.deb
#安装git(https://cn.linux-console.net/)
apt install git-all
```
# 主要介绍环境搭建

## 下载

```shell
#下载以下资源，并在home文件夹解压
$ tar -zxvf ti-ezsdk.tar.gz 
$ tar -zxvf CodeSourcery.tar.gz 
```

## NFS

```shell
#安装网络文件挂载系统nfs
$ apt install nfs-kernel-server
#修改目录
$ gedit /etc/exports
#添加以下信息在文件末，elainay是我的用户名，下面也是我的路径，自己修改
 /home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/filesystem/ezsdk-dm816x-evm-rootfs *(rw,nohide,insecure,no_subtree_check,async,no_root_squash)
 #重启nfs
 $ sudo /etc/init.d/nfs-kernel-server restart
```

## 启动uboot

- 终端软件：

```shell
#download term
$ apt install cutecom
#启动cutecom
$ cutecom
```

- 启动之后是一个图形化界面的终端，玩过msp430应该都很熟悉了（之后cutecom的发送终端命令以 `$$`开头，和Linux终端区分开来）

```shell
#连接电源等，启动开发板，串口终端会发送一堆#字符串和break之类的
#赶紧随便发送回车，暂停boot启动，再发送以下命令，打印环境变量
$$ printenv
#输出
bootargs=console=ttyO2,115200n8 noinitrd ip=192.168.1.21:192.168.1.43:192.168.1.43:255.255.255.0::eth0:off mem=256M rootwait=1 rw ubi.mtd=4,2048 rootfstype=ubifs root=ubi0:rootfs init=/init vram=20M notifyk.vpssm3_sva=0xBEE00000 stdin=serial ddr_mem=1024M
```

- 那我们接下来要设置板子的boot参数了
- 板子挂载的系统为在ti-ezsdk
- 所以需要nfs，先配置有线连接

```shell
#print ip
$ ifconfig
#连接板子并启动，在设置-网络-有线连接中，手动分配ipv4
#随便分配一个没有使用的，我这里为为192.168.1.51
$ ifconfig
```

- 然后提前写好bootargs，方便以后复制
- setenv bootargs 'console=ttyO2,115200n8 rootwait root=/dev/nfs rw rootftype=jffs2 mem=169M@0x80000000 vram=50M  nfsroot=192.168.1.51:/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/filesystem/ezsdk-dm816x-evm-rootfs ip=192.168.1.88:192.168.1.51:192.168.1.51:255.255.255.0::eth0:off notifyk.vpssm3_sva=0xBF900000 earlyprintk'
- 其中ip=板子ip:电脑ip:根ip

```shell
#restart
$$ printenv
$$ setenv bootargs 'console=ttyO2,115200n8 rootwait root=/dev/nfs rw rootftype=jffs2 mem=169M@0x80000000 vram=50M  nfsroot=192.168.1.51:/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/filesystem/ezsdk-dm816x-evm-rootfs ip=192.168.1.88:192.168.1.51:192.168.1.51:255.255.255.0::eth0:off notifyk.vpssm3_sva=0xBF900000 earlyprintk'
$$ saveenv
$$ nand read 0x81000000 0x00580000 0x300000
$$ bootm 0x81000000
```

- 成功打印ACCEPT

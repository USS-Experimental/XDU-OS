# Linux内核编译

## 查看当前系统内核

执行如下操作查看当前内核版本

```bash
$ uname -r
6.1.0-3-generic
```

此处为系统默认的内核

## 删除之前编译的内核

如果之前自行编译安装过内核，建议将原有的内核删除，执行如下命令

```bash
$ rm -rf /boot/vmlinuz-6.1.0
$ rm -rf /boot/System.map-6.1.0
$ rm -rf /boot/initrd.img-6.1.0
$ rm -rf /boot/config-6.1.0
$ rm -rf /lib/modules/6.1.0
```

6.1.0为课程资料中提供的内核版本，删除脚本为`delete.sh`，使用方法`sudo bash delete.sh`

## 编译新内核

执行如下操作

```bash
# 使用图形化工具配置编译
$ make menuconfig
# 出现图形化界面之后直接选择exit退出即可，若了解参数可自行修改

# 此处参数根据虚拟机分配的cpu核数设置
$ make -j8

# 安装子模块
$ sudo make modules_install

# 安装内核
$ sudo make install

# 更新grub引导
$ sudo update-grub

# 重启
$ sudo reboot
```

**注意！**在重启进入grub界面时，操纵方向键阻止其选中默认内核，手动选择刚刚编译的Linux 6.1.0，而非Linux 6.1.0-3-generic。

重新启动后，执行如下操作查看当前内核版本

```bash
$ uname -r
6.1.0
```

内核编译替换成功。
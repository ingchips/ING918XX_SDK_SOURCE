1. To build & run 32bit under 64bit linux (WSL1 not work)

```shell
$ sudo dpkg --add-architecture i386
$ sudo apt-get update
$ sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386
$ sudo apt-get install gcc-multilib
```

1. for Linux:

```
sudo sysctl fs.mqueue.msg_max=100
```
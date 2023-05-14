# Setup devices

## Image raspberry pi

1. Install raspberry pi imager from [here](https://www.raspberrypi.com/software/)
2. Select "CHOOSE OS"->"Rasbperry Pi OS (other)"->"Raspberry Pi OS (64-bit)"
3. Select "CHOOSE STORAGE"->desird sd card
4. Optional: Select the gear icon to change the set a hostname, set username and password, configure wifi, and/or set location setting
5. Then click the "WRITE" button
6. Install the sd card into the raspberrypi and boot it up

## Setup raspberry pi

1. Update the system using `sudo apt update` and `sudo apt upgrade`
2. Install packages that are required to compile OpenCV `sudo apt install cmake build-essential pkg-config git`
3. Install packages that support different image and video formats for OpenCV 

```
sudo apt install libjpeg-dev libtiff-dev libpng-dev libwebp-dev libopenexr-dev
``` 

not libjasper-dev

```
sudo apt install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libdc1394-22-dev libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev
```
4. Install packages required for OpenCV interfaces `sudo apt install libgtk-3-dev libqt5gui5 libqt5webkit5 libqt5test5 python3-pyqt5`
5. Install packages to help OpenCV run faster on raspbery pi `sudo apt install libatlas-base-dev liblapacke-dev gfortran`
6. Install packages that OpenCV uses to manage data `sudo apt install libhdf5-dev libhdf5-103`
7. Install packages that for support of Python `sudo apt install python3-dev python3-pip python3-numpy`
8. Set up ssh keys `ssh-keygen -t rsa`

## Setup Ubuntu virtual machine

This setup requires a linux machine to cross compile for raspberry pi. If the user is using a Windows machine the following steps show how to setup a virtual machine to run Linux

1. Download and install VirtualBox [here](https://www.virtualbox.org/wiki/Downloads)
2. Download a Ubuntu iso from [here](https://ubuntu.com/download/desktop)
3. Create a new OS in VirtualBox using the Ubuntu iso
5. When creating the new os in VirtualBox make sure the check the "Skip Unattended Installation" check box. So the user will be created during installation and allow user to be sudo group
6. It is recommended to create the system with all available CPUs, 16GB of base memory, and 40GB of storage
5. After booting, follow the installation steps and choose default settings
8. Update the system using `sudo apt update` and `sudo apt upgrade`
7. Insert guest additions CD image. In the top menu bar "Devices"->"Insert Guest Additions CD image..."
8. Open the cd image and run `autorun.sh`
8. Eject the cd
9. Power off the system and click on the newly created OS in VirtualBox menu. "Settings"->"Display"->"Video Memory" increase to a max of 128MB to prevent OS screen from going black
10. Enable bidirectional clipboard "Devices"->"Shared Clipboard"->"Bidirectional"

## Setup Ubuntu environment

1. Create a variable that will store all the location of all the project files. Change `/home/keenan/Documents/PoolController` to a desired location. Make sure to use an absolute path and to not include the tilde "~" as it won't work in cmake
```
echo export PoolControllerProjectPath=/home/keenan/Documents/PoolController >> ~/.bashrc  # Project path on Linux cross compiler computer
echo export PoolControllerProjectPiPath=/home/jeff/Documents/PoolController >> ~/.bashrc  # Project path on Raspberry Pi
source ~/.bashrc
mkdir -p $PoolControllerProjectPath
```
11. Setup ssh key `ssh-keygen -t rsa`
12. Copy public key to raspberry pi. ssh from Ubuntu machine to raspberry pi. Then copy the contents in ~/.ssh/id_rsa.pub from Ubuntu machine to Raspberry Pi machine at ~/.ssh/authorized_keys
11. Download a raspberry pi cross-compiler [here](https://github.com/abhiTronix/raspberry-pi-cross-compilers). Select a version that Host OS="any x64/x86 Linux machine", Target OS="Bullseye 64-bit OS (Debian Version 11) only". As of writing this that version would be [Raspberry Pi GCC 64-Bit Cross-Compiler Toolchains (Bullseye) 10.3.0](https://sourceforge.net/projects/raspberry-pi-cross-compilers/files/Bonus%20Raspberry%20Pi%20GCC%2064-Bit%20Toolchains/Raspberry%20Pi%20GCC%2064-Bit%20Cross-Compiler%20Toolchains/Bullseye/GCC%2010.3.0/)
12. Extract using `tar -xvzf cross-gcc-10.3.0-pi_64.tar.gz`
13. Move cross compiler to project location `mv cross-pi-gcc-10.3.0-64 $PoolControllerProjectPath`
12. Install git and cmake `sudo apt install git cmake`
13. For some reason the system gtk is required `sudo apt install libgtk-3-dev`
14. Download opencv to desired location `git clone https://github.com/opencv/opencv.git $PoolControllerProjectPath/opencv`
16. Copy raspberry pi library and header files to Ubuntu cross compiler directly using 
```
rsync -az --delete-after jeff@jeffpi:/{lib,usr} $PoolControllerProjectPath/cross-pi-gcc-10.3.0-64/aarch64-linux-gnu/libc
```
16. Create a cmake toolchain file for cross compiling called PiToolchain.cmake
```
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

# Specify the cross compiler locations
SET(TOOLROOT $ENV{PoolControllerProjectPath}/cross-pi-gcc-10.3.0-64)
SET(SYSROOT ${TOOLROOT}/aarch64-linux-gnu/libc)

# specify the cross compiler
SET(CMAKE_C_COMPILER   ${TOOLROOT}/bin/aarch64-linux-gnu-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLROOT}/bin/aarch64-linux-gnu-g++)

SET(CMAKE_SYSROOT ${SYSROOT})
SET(CMAKE_FIND_ROOT_PATH ${TOOLROOT})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_SYSTEM_PROCESSOR ARM64)
SET(PKG_CONFIG_LIBDIR ${SYSROOT}/usr/lib/pkgconfig)
SET(PKG_CONFIG_SYSROOT_DIR ${SYSROOT})

include_directories(SYSTEM ${SYSROOT}/usr/include/aarch64-linux-gnu)
link_directories(${SYSROOT}/usr/lib/aarch64-linux-gnu)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${SYSROOT}/usr/lib/aarch64-linux-gnu")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath-link,${SYSROOT}/usr/lib/aarch64-linux-gnu")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-rpath-link,${SYSROOT}/usr/lib/aarch64-linux-gnu")
```
15. Create a file called "BuildOpencv.sh" and copy the following code into it and save it to $PoolControllerProjectPath
```
#!/bin/bash

BUILD_DIR=$PoolControllerProjectPath/opencv/build
rm -r $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake -DOPENCV_ENABLE_PKG_CONFIG=ON -DCMAKE_TOOLCHAIN_FILE=../../PiToolchain.cmake .. && make -j20 && make install
```
16. Change permissions to allow to run it `chmod 777 BuildOpencv.sh`
17. Run `./BuildOpencv.sh` and wait for opencv to compile
18. Copy installed opencv into cross compiler installation directory `cp -a $PoolControllerProjectPath/opencv/build/install/* $PoolControllerProjectPath/cross-pi-gcc-10.3.0-64/aarch64-linux-gnu/libc`
19. On the raspberry pi make sure to tell it where the opencv install shared library are by running `export LD_LIBRARY_PATH="~/Documents/OpencvInstall/lib":$LD_LIBRARY_PATH`
20. Probably needs to be exported to root by create a file called `/etc/ld.so.conf.d/PoolController.conf` on the raspberry pi with the following content
```
/home/jeff/Documents/OpencvInstall/lib
```

## Useful websites

- https://tttapa.github.io/Pages/Raspberry-Pi/C++-Development/index.html
- https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/CrossCompiling
- https://visualgdb.com/tutorials/raspberry/opencv/build/
- 

## Useful commands
```
# Log in as root and add library path in a new file created in /etc/ld.so.conf.d then run the following command
# To add a shared library path run the following commands
echo /home/jeff/Documents/OpencvInstall/lib > /etc/ld.so.conf.d/PoolController.conf
ldconfig -v  # Log in on the remote computer as root and run this command

make && rsync ./Build/out jeff@jeffpi:~/Documents/Remote

ssh jeff@jeffpi 'cd ~/Documents/Remote && sudo ./out'
ssh jeff@jeffpi 'cd ~/Documents/Remote && sudo gdbserver localhost:2000 ./out'

ssh jeff@jeffpi 'sudo kill $(sudo lsof -ti :2000)'  # Kill gdbserver if not stopped properly before
ssh jeff@jeffpi 'sudo killall out'  # Kill a running exe if not stopped properly

pkg-config --libs opencv
pkg-config --cflags opencv

pkg-config --libs gtk+-3.0
pkg-config --cflags gtk+-3.0
ldconfig -p | grep gtk

sudo apt install build-essential  # Useful for getting standard developer packages
sudo apt install libncurses5
sudo apt install python2.7-dev

rsync -az --delete-after jeff@jeffpi:/{lib,usr,opt,etc} $PoolControllerProjectPath/cross-pi-gcc-10.3.0-64/aarch64-linux-gnu/libc
rsync -az $PoolControllerProjectPath/opencv/build/OpencvInstall jeff@jeffpi:~/Documents/  # Copy opencv installation to raspberry pi

target remote jeffpi:2000  # gdb command

# How to setup git with github remote on linux
wget https://github.com/git-ecosystem/git-credential-manager/releases/download/v2.1.2/gcm-linux_amd64.2.1.2.deb -P ~/Downloads
sudo dpkg -i ~/Downloads/gcm-linux_amd64.2.1.2.deb
git-credential-manager configure
git config --global credential.credentialStore cache

# How to install a markdown viewer on Ubuntu. Doesn't seem to work properly though
wget https://github.com/marktext/marktext/releases/download/v0.17.1/marktext-amd64.deb -P ~/Downloads
sudo dpkg -i ~/Downloads/marktext-amd64.deb
marktext $user_file  # type in terminal to open
sudo dpkg -P marktext  # Uninstall

# How to install ghostwriter for reading markdown
sudo add-apt-repository ppa:wereturtle/ppa
sudo apt update
sudo apt install ghostwriter

```



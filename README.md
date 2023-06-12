# Pool Controller - Raspberry Pi Code

This is the code that runs on the Raspberry Pi which controls a camera, motors, lights, and limits switches which are used to press physical buttons on a pool remote. This code waits for commands from an Android app which can be anywhere in the world with internet access. This allows the user to control their pool from anywhere in the world. Below are the instructions for how to setup the code on the Raspberry Pi along with using Ubuntu to cross compile in order to speed up development.

# Setup devices

## Image Raspberry Pi

1. Install raspberry pi imager from [here](https://www.raspberrypi.com/software/)
2. Select "CHOOSE OS"->"Rasbperry Pi OS (other)"->"Raspberry Pi OS (64-bit)"
3. Select "CHOOSE STORAGE"->desird sd card
4. Optional: Select the gear icon to change the set a hostname, set username and password, configure wifi, and/or set location setting
5. Then click the "WRITE" button
6. Install the sd card into the raspberrypi and boot it up

## Setup Raspberry Pi

1. Update the system using 

```
sudo apt update && sudo apt upgrade
```
2. Install a gpio manipulation library

```
sudo apt install pigpio
```

3. Install packages that are required to compile OpenCV 

```
sudo apt install cmake build-essential pkg-config git
```

4. Install packages that support different image and video formats for OpenCV 

```
sudo apt install libjpeg-dev libtiff-dev libpng-dev libwebp-dev libopenexr-dev
sudo apt install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev libdc1394-22-dev libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev
```

5. Install packages required for OpenCV interfaces 

```
sudo apt install libgtk-3-dev libqt5gui5 libqt5webkit5 libqt5test5 python3-pyqt5
```

6. Install packages to help OpenCV run faster on raspbery pi 

```
sudo apt install libatlas-base-dev liblapacke-dev gfortran
```

7. Install packages that OpenCV uses to manage data 

```
sudo apt install libhdf5-dev libhdf5-103
```

8. Install packages for support of Python 

```
sudo apt install python3-dev python3-pip python3-numpy
```

9. Set up ssh keys 

```
ssh-keygen -t rsa
```

## Setup Ubuntu virtual machine

This setup requires a linux machine to cross compile for raspberry pi. If the user is using a Windows machine the following steps show how to setup a virtual machine to run Linux

1. Download and install VirtualBox [here](https://www.virtualbox.org/wiki/Downloads)
2. Download a Ubuntu iso from [here](https://ubuntu.com/download/desktop)
3. Create a new OS in VirtualBox using the Ubuntu iso
4. When creating the new os in VirtualBox make sure the check the "Skip Unattended Installation" check box. So the user will be created during installation and allow user to be sudo group
5. It is recommended to create the system with all available CPUs, 16GB of base memory, and 40GB of storage
6. After booting, follow the installation steps and choose default settings
7. Update the system using `sudo apt update` and `sudo apt upgrade`
8. Insert guest additions CD image. In the top menu bar "Devices"->"Insert Guest Additions CD image..."
9. Open the cd image and run `autorun.sh`
10. Eject the cd
11. Power off the system and click on the newly created OS in VirtualBox menu. "Settings"->"Display"->"Video Memory" increase to a max of 128MB to prevent OS screen from going black
12. Enable bidirectional clipboard "Devices"->"Shared Clipboard"->"Bidirectional"

## Setup Ubuntu environment

1. Make sure the system is up-to-date `sudo apt update && sudo apt upgrade`

2. Install packages to allow for building and cloning code

```
sudo apt install git cmake
```

3. Install packages to allow for debugging

```
sudo apt install libncurses5 python2.7-dev
```

4. For some reason the system gtk is required 

```
sudo apt install libgtk-3-dev
```

5. Create a series of variables to store project locations and ssh variables. Make sure to use an absolute path and to not include the tilde "~" as it won't work in cmake. Make sure to change these variables to match the desired user preference.

```
echo export PoolControllerProjectPath=/home/keenan/Documents/PoolController >> ~/.bashrc  # Project path on Linux cross compiler computer
echo export PoolControllerProjectPiPath=/home/jeff/Documents/PoolController >> ~/.bashrc  # Project path on Raspberry Pi
echo export PiUsername=jeff >> ~/.bashrc  # Raspberry Pi username
echo export PiHostname=jeffpi >> ~/.bashrc  # Raspberry Pi hostname
echo export PiSshPort=22 >> ~/.bashrc  # Raspberry Pi port
source ~/.bashrc
mkdir -p $PoolControllerProjectPath
```

### Setup connection to Raspberry Pi

1. Setup ssh key by using the below command. To make things easier leave all the prompts blank while pressing enter

```
ssh-keygen -t rsa
```

2. Copy public key to Raspberry Pi by doing the following: 
- ssh from Ubuntu machine to raspberry pi `ssh ${PiUsername}@${PiHostname}`
- Then append the contents in `~/.ssh/id_rsa.pub` from Ubuntu machine to Raspberry Pi machine at `~/.ssh/authorized_keys`

### Setup cross compiler

1. Download a raspberry pi cross-compiler [here](https://github.com/abhiTronix/raspberry-pi-cross-compilers). Select a version that Host OS="any x64/x86 Linux machine", Target OS="Bullseye 64-bit OS (Debian Version 11) only". As of writing this that version would be [Raspberry Pi GCC 64-Bit Cross-Compiler Toolchains (Bullseye) 10.3.0](https://sourceforge.net/projects/raspberry-pi-cross-compilers/files/Bonus%20Raspberry%20Pi%20GCC%2064-Bit%20Toolchains/Raspberry%20Pi%20GCC%2064-Bit%20Cross-Compiler%20Toolchains/Bullseye/GCC%2010.3.0/)
2. Extract using `tar -xvzf cross-gcc-10.3.0-pi_64.tar.gz`
3. Move cross compiler to project location `mv cross-pi-gcc-10.3.0-64 $PoolControllerProjectPath`
4. Overwrite the Ubuntu cross compiler dependencies with the Raspberry Pi dependencies so the environments match exactly using the below command. This command can take around 20 minutes to copy so be patient

```
rsync -az --delete-after ${PiUsername}@${PiHostname}:/{lib,usr} $PoolControllerProjectPath/cross-pi-gcc-10.3.0-64/aarch64-linux-gnu/libc
```

### Setup Opencv

1. Download opencv to `$PoolControllerProjectPath` location 

```
git clone https://github.com/opencv/opencv.git $PoolControllerProjectPath/opencv
```

2. Create a cmake toolchain file for cross compiling called `PiToolchain.cmake` with the following contents and save it to $PoolControllerProjectPath

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

3. Create a file called `BuildOpencv.sh` and copy the following code into it and save it to `$PoolControllerProjectPath` directory

```
#!/bin/bash

BUILD_DIR=$PoolControllerProjectPath/opencv/build
rm -r $BUILD_DIR
mkdir -p $BUILD_DIR
cd $BUILD_DIR
cmake -DOPENCV_ENABLE_PKG_CONFIG=ON -DCMAKE_TOOLCHAIN_FILE=../../PiToolchain.cmake .. && make -j20 && make install
```

4. Change permissions to allow to run it `chmod 777 BuildOpencv.sh`
5. Run `./BuildOpencv.sh` and wait for opencv to compile
6. Copy Opencv installed build into cross compiler installation directory

```
rsync -az $PoolControllerProjectPath/opencv/build/install/* $PoolControllerProjectPath/cross-pi-gcc-10.3.0-64/aarch64-linux-gnu/libc/usr
```

7. Also install(copy) the library to the Raspberry Pi using the command below

```
ssh ${PiUsername}@${PiHostname} "mkdir -p ${PoolControllerProjectPiPath}/Dependencies"
rsync -az $PoolControllerProjectPath/opencv/build/install/* ${PiUsername}@${PiHostname}:${PoolControllerProjectPiPath}/Dependencies
ssh ${PiUsername}@${PiHostname} "sudo rsync -az ${PoolControllerProjectPiPath}/Dependencies/* /usr"
```

### Setup Boost

1. Download the latest Boost library [here](https://www.boost.org/users/download/) to `${PoolControllerProjectPath}` or use the following command

```
wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.bz2 -P ~/Downloads
tar --bzip2 -xf ~/Downloads/boost_1_82_0.tar.bz2 --directory ${PoolControllerProjectPath}
```

2. Boost is a header mostly library so nothing needs to be built

### Setup code

1. Get the source code that will run on the Raspberry Pi

```
git clone https://github.com/keenanhnelson/PoolController_RaspberryPiCode.git ${PoolControllerProjectPath}/RaspberryPiCode
```

2. Now build the code

```
cd ${PoolControllerProjectPath}/RaspberryPiCode
make
```

3. Create the project directory structure on the Raspberry Pi. Which will support Raspberry Pi self build later

```
ssh ${PiUsername}@${PiHostname} "mkdir -p ${PoolControllerProjectPiPath}/RaspberryPiCode/Img"
ssh ${PiUsername}@${PiHostname} "mkdir -p ${PoolControllerProjectPiPath}/RaspberryPiCode/Build"
```

4. Transfer the executable to the Raspberry Pi

```
rsync ./Build/out ${PiUsername}@${PiHostname}:${PoolControllerProjectPiPath}/RaspberryPiCode/Build
```

4. Start the program manually

```
ssh ${PiUsername}@${PiHostname} "cd ${PoolControllerProjectPiPath} && sudo ${PoolControllerProjectPiPath}/RaspberryPiCode/Build/out"
```

5. Setup autostart by sshing into the Raspberry Pi and adding the following line in /etc/rc.local right before `exit 0`. Make sure to substitute `${PoolControllerProjectPiPath}` with the expanded path since that variable doesn't exist on the Rasbperry Pi

```
cd ${PoolControllerProjectPiPath}/RaspberryPiCode && sudo ./Build/out &
```

## Setup Raspberry Pi native build

Cross compiling should be used most of the time in order to quickly build, but sometimes it isn't available so building the code on the Raspberry Pi is required. This assumes that the cross compiled steps were already followed

1. SSH into Raspberry Pi

```
ssh ${PiUsername}@${PiHostname}
```

2. Setup project folder location so the Raspberry Pi knows where the project is

```
echo export PoolControllerProjectPiPath=/home/jeff/Documents/PoolController >> ~/.bashrc  # Project path on Raspberry Pi
source ~/.bashrc
mkdir -p $PoolControllerProjectPiPath
```

3. Setup boost. Extracting the boost library compressed file can take around 5 minutes

```
wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.bz2 -P ~/Downloads
tar --bzip2 -xf ~/Downloads/boost_1_82_0.tar.bz2 --directory ${PoolControllerProjectPiPath}
```

4. Remove any existing files in the project code location. Make sure there is nothing important in it before deleting

```
sudo rm -r ${PoolControllerProjectPiPath}/RaspberryPiCode/*
```

5. Download the source code

```
git clone https://github.com/keenanhnelson/PoolController_RaspberryPiCode.git ${PoolControllerProjectPiPath}/RaspberryPiCode
cd ${PoolControllerProjectPiPath}/RaspberryPiCode
```

6. Configure the makefile. Comment out lines 3, 4, and 5 inside the `Makefile` file and uncomment out lines 7, 8, and 9 which will setup compilation on the Raspberry Pi

7. Make sure to modify the server port number in `Inc/ServerInfoPrivate.hpp` to something unique to improve security

8. Build the project

```
cd ${PoolControllerProjectPiPath}/RaspberryPiCode
make
```

9. Run the code

```
sudo ./Build/out
```

## Portforwarding

Port forwarding is necessary to allow the Raspberry Pi to be accessed from outside the network. This can be done through a homes WIFI router by allow one of the ports to accept packets from the outside world.

## Working with dynamic IP address

The IP address of a residential home can change so it is recommended to use a service like [DuckDNS](https://www.duckdns.org/) to allow for a name to be attached to an dynamic IP address.

## Notes

### Useful websites

- https://tttapa.github.io/Pages/Raspberry-Pi/C++-Development/index.html
- https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/CrossCompiling
- https://visualgdb.com/tutorials/raspberry/opencv/build/

### Useful commands (for reference only, don't run)

```
# Log in as root and add library path in a new file created in /etc/ld.so.conf.d then run the following command
# To add a shared library path run the following commands
echo /home/jeff/Documents/OpencvInstall/lib > /etc/ld.so.conf.d/PoolController.conf
ldconfig -v  # Log in on the remote computer as root and run this command

make && rsync ./Build/out ${PiUsername}@${PiHostname}:~/Documents/Remote

ssh ${PiUsername}@${PiHostname} 'cd ~/Documents/Remote && sudo ./out'
ssh ${PiUsername}@${PiHostname} 'cd ~/Documents/Remote && sudo gdbserver localhost:2000 ./out'

ssh ${PiUsername}@${PiHostname} 'sudo kill $(sudo lsof -ti :2000)'  # Kill gdbserver if not stopped properly before
ssh ${PiUsername}@${PiHostname} 'sudo killall out'  # Kill a running exe if not stopped properly

# How to find installed packages
pkg-config --libs opencv
pkg-config --cflags opencv
pkg-config --libs gtk+-3.0
pkg-config --cflags gtk+-3.0
ldconfig -p | grep gtk

sudo apt install build-essential  # Useful for getting standard developer packages
sudo apt install libncurses5
sudo apt install python2.7-dev

target remote ${PiHostname}:2000  # gdb command

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

# How to change ssh settings
sudo vi /etc/ssh/sshd_config
Port 22  # Change port number to improve security
PasswordAuthentication no  # Removes password authentication to improve security
sudo /etc/init.d/ssh restart  # Restart ssh server to load settings changes

# How to change wifi settings
sudo raspi-config
    System Options -> Wireless LAN
```



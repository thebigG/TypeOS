These scripts are heavily based on the script I used for my OS Theory class at Rutgers University, which was taught by amazing [Dr. Kannan](https://github.com/SudarsunKannan)!


# QEMU setup for kernel hacking.

QEMU enables you to run a virtual machine either on a bare-metal OS or inside another virtual machine. 
More details of QEMU can be found here (https://www.qemu.org/).

The instructions and scripts below helps you to get started with a compiling a custom kernel and running a 
QEMU virtual machine using the compiled kernel.

For now, we will use Linux 4.17.

### Note:  

While the following  instructions to compile a custom kernel for QEMU will not affect or overwrite the original kernel of your system/laptop in which you are running, it is your responsibility to make sure you understand each of the script commands and know what you are doing. The kernels are compiled and written to the project directory rather than the "boot" folder.

You are more than welcome to use your own custom kernel compilation methods and running a QEMU!

### Set environmental variables
```
source qemu_scripts/setvars.sh "trusty"
```

### Instal required libraries
```
sudo apt-get install -y build-essential cmake libssl-dev bison flex
```

### Compiling and launching QEMU 
From the source directory set the environment variables.
trusty specifies the host systems linux version/codename; pass your own OS version name.
```
source qemu_scripts/setvars.sh "trusty"   
$BASE/qemu_scripts/compile_kern.sh
```

### Create the QEMU IMAGE  

Create the QEMU IMAGE only for the first time. You should
not create an image (which is your disk now) every time you will be
compiling and testing your kernel.

During installation, if prompted (y,n), enter yes
```
$BASE/qemu_scripts/qemu_create.sh  
```

### Setting your QEMU image 

Next, we will have to set password for the VM, and install required packages 
for the QEMU
```
cd $BASE 
source qemu_scripts/setvars.sh "trusty"
$BASE/qemu_scripts/mount_qemu.sh
cd $MOUNT_DIR   //Go to where the QEMU disk is mounted
sudo chroot .  //Set the disk as your root file system
passwd         //Add new password for the root user in the virtual machine
sudo apt-get install -y build-essential cmake libssl-dev  //Install packages
exit           //Unmount the QEMU disk
cd $BASE
$BASE/qemu_scripts/umount_qemu.sh  //Unmount the QEMU disk
```

### Now launch the QEMU
This script will use the compiled 4.17 kernel to run the QEMU
```
$BASE/qemu_scripts/run_qemu.sh
```

### Killing the QEMU process
```
$BASE/qemu_scripts/killqemu.sh
```

### Copying data from your host to QEMU
If you want to copy some data between your machine and QEMU disk image, then use the following script for convenience
This scripts copies directories/files from your host machine to QEMU's root folder
```
$BASE/qemu_scripts/copy_data_to_qemu.sh FULL_PATH_OF_DIRECTORY_TO_COPY
```

### Kernel error debugging

https://opensourceforu.com/2011/01/understanding-a-kernel-oops/




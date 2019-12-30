# scriptfs
A context-based file system(built on top of ramfs) that will hopefully make a writer's laptop last a 40-page short story without the annoying "you're laptop is about to die" warning.

**NOTE:**scriptfs is a project that started as a research project for my operating systems class at Rutgers University, and I hope to finish it one day. As of right now, I am NOT working on it. Given that this project  could potentially take from a couple months to (possibly) years to complete, I will be working on some other project that should take me less than six months to complete, this is a little something called Tasker. After I'm done with Tasker, and if nothing else pops up, I wil come back to work on scriptfs.

**Linux Changelog**

- struct page in "include/linux/mm_types.h" has a new field called **scriptfs_flag**
- inode.c has been changed to behave good for scriptfs in fs/ramfs/inode.c
- The file "linux-4.17_scriptfs_implementation_stable_0.1/kernel/scriptfs_util.c" has been added
- The file "linux-4.17_scriptfs_implementation_stable_0.1/include/linux/scriptfs_util.h" has been added
- syscalls 333, 334 and 335 have been added to the kernel.
- To find the syscall function definitions look at the "linux-4.17_scriptfs_implementation_stable_0.1/kernel/script_sys_calls.c" file

| Category  |Size   |Number of pages(4KB granularity)   |
|---|---|---|
| Poem  |32KB   |8   |
| Short Novel  |128KB   |32   |
| Small Novel  |256KB   |64   |
| Medium Novel  | 512KB  | 128  |
| Large Novel  | 2MB  | 512  |
|Super Novel   | 256MB  | 64K  |

#scriptfs stable changelog

| Version  | description  |
|---|---|
| 0.1  | Fixed issue where the kernel was not booting. This was caused by placing the new scriptfs_flag in struct page at the beginning of the struct. After doing some research, I found out this will surely break the kernel becasue of padding-related issues. So I placed the new flag at the very bottom of the struct and it seems to be working. By no means this "file system" is near completion. There are plenty of problems to be solved; crash consistency, optimizations for inode lookup and plenty of other polishing to be done. You are more than welcome to have a go at it, but I'm not responsible for any damage you may cause to your machine, you will be modifying a kernel after all. Realistically speaking, you really should not have anything major to worry about if you run this kernel on top of qemu(read the "QEMU - README" for instructions), but please act with caution. I HIGHYLY recommend keeping a [timeshift](https://github.com/teejee2008/timeshift) timeshift backup of your configuration and (if you want to be paranoid-careful) maybe a backup of your data. |



#Instructions to run scriptfs
Be sure to set up your kernel virtual machine by reading "QEMU - README" first!

**Make sure to copy the "TypeOS" folder to your new kernel before Booting your kernel**

The TypeOS folder has all of the useful scripts for scriptfs. You can copy the TypeOS folder like this:

	TypeOS/qemu_scripts/copy_data_to_qemu.sh ~/OS_Theory/Research_Final_Report/TypeOS






**1.** compile the kernel as such:
	
	./qemu_scripts/compile_kern.sh

**2**. run the kernel as such:

		./qemu_scripts/run_qemu.sh  

scriptfs runs on top of ramfs, so ramfs should be mounted first.

**3.** mount  ramfs as such:

		TypeOS/playground/make_regular_fs.sh 

This will mount an instance of ramfs with 512mb on your system at location "/mnt/scriptfs/".

**NOTE:** If you see an error that says something like this:

			EXT4-fs error (device sda): ext4_lookup:1576: inode #655361: comm
                                        mount: deleted inode referenced: 661157   
 This is perfectly fine, do not worry. We looked into this bug  and it is a well-known bug! We believe it has something to do with some SSD hardware.
 
 After mounting ramfs, your dmesg dump should look like this:
 
						[  284.386398] ramfs_mount 
	                                       [  284.386531] filling super block for
	                                       [  284.386624] s_maxbytes= 9223372036854775807
	                                       [  284.386640] s_blocksize= 4096
	                                       [  284.386649] s_blocksize_bits= 12
	                                       [  284.386657] s_magic X4= 2240043254
	                                       [  284.386712] >>scriptfs_state_NOOP(new)<< 
If it looks like that, great!

The important thing about this dump is the very last line **>>scriptfs_state_NOOP(new)<< **
This is scriptfs doing some magic behind the scenes :)


**4.** cd into scriptfs

	cd /mnt/scriptfs/


Now we can start making scriptfs files!

**5.** First let's write a poem like this:

	 python3 ~/TypeOS/playground/make_script.py -t 0  -s "mount" -c "poem"  1KB file1_scriptfs  seq wr_through
	 
This will write a poem for you with random words in it in the current directory, which is "/mnt/scriptfs"

Your dmesg dump should look like this:

	                                       [ 1047.163695] scriptfs success on file creation!
	                                       [ 1047.278180] Does this print#1-->simple_write_begin
	                                       [ 1047.278401] fetch_next_page time(in nanoseconds):73795
	                                       [ 1047.278482] scriptfs grab_cache_page_write_begin time(in nanoseconds):185564
	                                       for inode:8692
	                                       [ 1047.278516] checking correctness of page for scriptfs
	                                       [ 1047.278536] this page belongs to scrptfs!
	                                       [ 1047.281609] setting state to scripfs:3
	                                       
There are many important lines in the dump, but one of the MOST important ones is the **this page belongs to scrptfs!** line.


This script just created a custom-size block  through the syscall interface of 32KB in size, which any subsequent writes to the same file can write to these pre-allocated pages!



You can obviously look at the contents of this file by doing something like this:

	cat file1_scriptfs
	



Now for some proof that scriptfs actually works!

**6**. Check the current stats of memory with this:

	free -m
	
It should look like this:

		                 total       used       free     shared    buffers     cached
	      Mem:            3945      74         3871          0         15         23
	     -/+ buffers/cache:         35       3910
	       Swap:            0          0          0


The most important column here is **used**. Right now we have 74MB that are currently being used from our memory.
This will of couse be different on yuur machine, but that's ok. Jost down your current memory usage, whether is 74MB or 100MB.






**7.** Let's now create a super novel like this:

	python3 ~/TypeOS/playground/make_script.py -t 0  -s "mount" -c "super_novel"  4MB file1_scriptfs_super_novel  seq wr_through



This will take a while, so please wait. It shouldn't take more than 5 minutes. Go aget some coffee and now you should now have a 4MB file on your scriptfs filesystem!
 
 Actually that's a lie, because when you look at your memory usage with **free -m**  it will look like you're using WAY more memory than that(that's because you are)!
 
 For us , it looks like this right now:
 
 
                total       used       free     shared    buffers     cached
               Mem:          3945      324      3620   0         15         27
               -/+ buffers/cache:        281       3663
               Swap:            0          0          0



Remember the 74MB we had now we have about 256MB more than we previously had. We now  have 74MB + 256MB = 330MB. That's because our python scriptfs is using our syscall interface to inform scriptfs of the context which is the super_novel, which equals 256MB.
We know it's not EXACT, but given that scriptfs runs on top of ramfs, there are limitations such as memory size control of kernel memory.
We use a lot of ramfs mechanisms and having "less memory" is a size effect of that. The kernel keeps tight bookeping of these pages and, even ramfs, is rather complex when one wants to "pin down" pages.


**9.** Delete your super_novel as such:

	rm file1_scriptfs_super_novel
	
We have **320MB** of used memory now. You should have 4MB less on your machin too after running **free -m**. This is a bug that we were working hard to fix, but we didn't have time.  We suspect this might have to do with the issue of ACTUALLY pinning a page in the linux kernel. Which we found a lot of dispute about online, but not much that was conclusive. Ideally in the future scriptfs will delete ALL of the **256MB** that were allocated.


**NOTE:** If you would like to time the performance of scriptfs and compare it to ramfs  **skip step 10.** If you are intersted in that, jump to the **How did we get the time statistics** section.


**10.** Unmount scriptfs like such:

	 
	 cd ..
	umount scriptfs/



#How do we get time/performance statistics?

Assuming that you DID NOT unmount scriptfs in step 10, follow the following steps to time the performance of scriptfs.


**1.** For the first row on scriptfs, do the following:

	python3 ~/TypeOS/playground/run_many_times.py 50  "/root/TypeOS/playground/make_script.py -t 0 -s "mount" -c "poem"  1b /mnt/scriptfs/file1_scriptfs  seq wr_through"

This script will write 1 byte to the file1_scriptfs 50 times. Give it some time, and then it will output the average time it took to write that 1b to the file like this:

	time average(in seconds): 0.8378624725341797

**2** For ramfs, do the following:

	   python3 ~/TypeOS/playground/run_many_times.py 50  "/root/TypeOS/playground/make_script.py -t 0 -s "noop" -c "poem"  1b /mnt/scriptfs/file1_ramfs  seq wr_throug"

**NOTE:** MAKE Sure that you are passing the "noop" argument and NOT "mount". "noop" tells the kermel that that userspace is using ramfs and NOT scriptfs.

This should output something like this:

	time average(in seconds): 0.8371044206619263



To get the average time it takes to write for other types of contexts in **scriptfs**, replace the "poem" argument in 

		python3 ~/TypeOS/playground/run_many_times.py 50  "/root//TypeOS/playground/make_script.py -t 0 -s "mount" -c "poem"  1b /mnt/scriptfs/file1_scriptfs  seq wr_through"
		
with "short_novel", "small_novel", "medium_novel", "large_novel" or "super_novel".

**Be sure to also replace the file name argument with something of your liking. This MUST be a full path when running the  " ~/TypeOS/playground/run_many_times.py" script.**



To get the average time it takes to write for other types of contexts in **ramfs**, replace the "poem" argument in 

		python3 ~/TypeOS/playground/run_may_times.py 50  "/root//TypeOS/playground/make_script.py -t 0 -s "noop" -c "poem"  1b /mnt/scriptfs/file1_scriptfs  seq wr_through"
		
with "short_novel", "small_novel", "medium_novel", "large_novel" or "super_novel".

**Be sure to also replace the file name argument with something of your liking. This MUST be a full path when running the  "~/TypeOS/playground/run_many_times.py" script. Please NOTE the "noop" argument. This tells the kernel that this process from userspace wants to use ramfs, and NOT scriptfs**

	

























 

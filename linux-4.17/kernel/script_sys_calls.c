#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/scriptfs_util.h>
//I think it's best to initialize everything in one place, like this file.
int scriptfs_pid = -1;
int current_poem_index = 0;
enum scriptfs_state current_scriptfs_state = scriptfs_state_NOOP;
struct page* global_page = NULL;
struct poem* scriptfs_poems = NULL;
EXPORT_SYMBOL(scriptfs_pid);
EXPORT_SYMBOL(current_scriptfs_state);
EXPORT_SYMBOL(global_page);

SYSCALL_DEFINE1(is_my_process, int,  user_pid)
{
    printk("setting pid to scriptfs\n");
    scriptfs_pid = user_pid;
    return 0;
}
SYSCALL_DEFINE0(start_scriptfs)
{
    printk("setting MOUNT state to scripfs\n");
    current_scriptfs_state = scriptfs_state_MOUNT;
    // scriptfs_pid = current->pid;
    return 0;
}

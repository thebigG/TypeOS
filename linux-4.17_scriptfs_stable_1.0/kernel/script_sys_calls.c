#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/scriptfs_util.h>
//I think it's best to initialize everything in one place, like this file. 
int scriptfs_pid = -1;
int current_poem_index = 0;
enum scriptfs_state current_scriptfs_state = scriptfs_state_NOOP;
enum scriptfs_context current_scriptfs_context = scriptfs_context_NO_CONTEXT;
struct file_range_mapping* bit_range_table = NULL;
struct scriptfs_block_pool* global_scriptfs_block_pool = NULL;
struct scriptfs_block* current_scriptfs_file = NULL;
struct page* global_page = NULL;
struct poem* scriptfs_poems = NULL;
EXPORT_SYMBOL(scriptfs_pid);
EXPORT_SYMBOL(current_scriptfs_state);
EXPORT_SYMBOL(global_page);
EXPORT_SYMBOL(current_scriptfs_context);
EXPORT_SYMBOL(current_scriptfs_file);
EXPORT_SYMBOL(global_scriptfs_block_pool);
SYSCALL_DEFINE1(is_my_process, int,  user_pid)
{
    printk("setting pid to scriptfs(new)\n");
    scriptfs_pid = user_pid;
    return 0;
}
/**
post-condition:The process using scriptfs set the scriptfs_pid.
This function assumes that the pid has been set by the pid
of the process using scriptfs.

*/
SYSCALL_DEFINE1(set_scriptfs_state, enum scriptfs_state, new_state)
{
    printk("setting state to scripfs:%d\n", new_state);
    current_scriptfs_state = new_state;
    // scriptfs_pid = current->pid;
    return 0;
}

SYSCALL_DEFINE1(set_scriptfs_context, enum scriptfs_context, new_context)
{
  printk("setting scriptfs to:%d\n", new_context);
  current_scriptfs_context = new_context;
  return 0;
}

#include <linux/kernel.h>
#include <linux/scriptfs_util.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/pagemap.h>
#include <linux/mm.h>
/**
This checks if the current process is trying to write to
scriptfs.
*/
int is_scriptfs_pid()
{
  return current->pid == scriptfs_pid;
}
/**
This checks if the current state of scriptfs is set to mounted.
*/
int is_scriptfs_mounted()
{
  return current_scriptfs_state == scriptfs_state_MOUNT;
}
/**
This checks if the current process is trying to write to scriptfs AND
if scriptfs is mounted.
Most of the time we'll be using this function to check if scriptfs has been
mounted along with ramfs.
*/
int is_current_scriptfs_mounted()
{
  return is_scriptfs_pid() && is_scriptfs_mounted();
}
/**
This initializes ONE poem block, for now.
In the future we'll be polishing this function
to write a list of poem blocks to be used system-wide.
This is called on fs/ramfs/inode.c
*/
int init_poems()
{
  scriptfs_poems = kmalloc(sizeof(struct poem) * POEM_SIZE,GFP_KERNEL);
  gfp_t flags;
  flags = __GFP_THISNODE; //This flag seems to make it work
  if(scriptfs_poems == NULL)
  {
    printk("kmalloc failed\n");
  }
  else
  {
    printk("kmalloc successful\n");
  }
  int i = 0;
  while(i<POEM_SIZE)
  {
    scriptfs_poems[i].poem_page = __page_cache_alloc(flags);
    if(scriptfs_poems[i].poem_page !=NULL)
    {
      printk("poem_page allocation success \n");
    __SetPageReferenced(scriptfs_poems[i].poem_page);
    }
    else
    {
      printk("poem_page allocation success\n");
      return -1;
    }
    i++;
  }
  return 0;
}
/**
Frees an array of pages that.
pages:beginning of the array.
num_pages: number of pages to free.
put_page is called for each page.
*/
int free_pages(struct* pages, int num_pages)
{
  int i =0;
  while(i<num_pages)
  {
    put_page(pages[i]);
    i++;
  }
}

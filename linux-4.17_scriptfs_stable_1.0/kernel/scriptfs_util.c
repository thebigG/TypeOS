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

WARNING: Be very careful using this function, or any functin like is_scriptfs_pid. It can
break the kernel if one tries to access the current task while the kernel boots.
*/
int is_current_scriptfs_mounted()
{
  return is_scriptfs_pid() && is_scriptfs_mounted();
}
/**
This initializes an array of pages based on new_context.
For example if new_context is scriptfs_context_POEM, then POEM_PAGES_SIZE pages
are allocated.
This returns an array of struct page pointers where each struct page
pointer points to a page allocated by __page_cache_alloc
*/
struct page** init_pages(enum scriptfs_context new_context)
{
  int number_of_pages  = 0;
  switch(new_context){
    case scriptfs_context_POEM:
      number_of_pages = POEM_PAGES_SIZE;
      break;
    case scriptfs_context_SMALL_NOVEL:
      number_of_pages = SMALL_NOVEL_PAGES_SIZE;
      break;
    case scriptfs_context_MEDIUM_NOVEL:
      number_of_pages =  MEDIUM_NOVEL_PAGES_SIZE;
      break;
    case scriptfs_context_LARGE_NOVEL:
      number_of_pages = LARGE_NOVEL_PAGES_SIZE;
      break;
    case scriptfs_context_SUPER_NOVEL:
      number_of_pages = SUPER_NOVEL_PAGES_SIZE;
      break;
    case scriptfs_context_NO_CONTEXT:
    return NULL;
    }
  struct page** new_pages = kmalloc(sizeof( struct page*) * number_of_pages,GFP_KERNEL);
  gfp_t flags;
  flags = __GFP_THISNODE; //This flag seems to make it work

  if(new_pages == NULL)
  {
    printk("kmalloc failed\n");
    return -1;
  }
  else
  {
    printk("kmalloc successful\n");
  }
  int i = 0;
  // new_pages->head  = new_pages;
  // struct linked_list_pages* head = new_pages->head;
    struct timespec time_info;
    long noop_time_avg = 0;
    long alloc_pages_time_avg  = 0;
  while(i<number_of_pages)
  {

    // head->page = kmalloc(sizeof(struct page) * number_of_pages,GFP_KERNEL);
    // if(head->page == NULL)
    // {
    //   printk("kmalloc failed#2\n");
    //   return -1;
    // }
    // head->page =  __page_cache_alloc(flags);
    getnstimeofday(&time_info);
long start  = time_info.tv_nsec;
new_pages[i] =  __page_cache_alloc(flags);
getnstimeofday(&time_info);
long end  = time_info.tv_nsec;
if(&(new_pages[i]) == NULL)
{
  printk("pre-allocation failed!");
  return -1;
}
else
{
  printk("pre-allocation successful!\n");
  alloc_pages_time_avg +=  (end - start);
  printk("allocation time(in nanoseconds):%ld\n", end - start);
  getnstimeofday(&time_info);
  //NOOP comment
  start  = time_info.tv_nsec;
  getnstimeofday(&time_info);
  end  = time_info.tv_nsec;
  printk("NOOP time(in nanoseconds):%ld\n", end - start);
  noop_time_avg += (end - start);
}
new_pages[i]->scriptfs_flag = SCRIPTFS_PAGE_FLAG_KEY;
    // new_pages[i]->scriptfs_flag = SCRIPTFS_PAGE_FLAG_KEY;
    __SetPageReferenced(new_pages[i]);
    // __SetPageReferenced(scriptfs_poems[i].poem_page); //maybe doing this is not such a good idea(?)
    i++;
  }
  printk("time avg for alloc_pages:%ld", alloc_pages_time_avg/number_of_pages );
  printk("time avg for noop:%ld", noop_time_avg/number_of_pages );
  return new_pages;
}
struct scriptfs_block* init_scriptfs_block(enum scriptfs_context new_context, unsigned long new_inode)
{
  struct scriptfs_block* new_block  =kmalloc(sizeof(struct scriptfs_block),GFP_KERNEL );
  new_block->pages = init_pages(new_context);
  new_block->context = new_context;
  new_block->current_index = 0;
  new_block->inode = new_inode;
  return new_block;
}
int init_global_scriptfs_block_pool()
{
  global_scriptfs_block_pool = kmalloc(sizeof(struct scriptfs_block_pool), GFP_KERNEL);
  printk("init_global_scriptfs_block_pool#1\n");
  if(global_scriptfs_block_pool == NULL)
  {
    return -1;
  }
  printk("init_global_scriptfs_block_pool#2\n");
  global_scriptfs_block_pool->head = global_scriptfs_block_pool;
  return 0;
}
/**
Call this function to allocate pages for new file.
This will add a scriptfs_block to the global_scriptfs_block_pool;
it appends it at the end of the list.
*/
int create_new_scriptfs_file(unsigned long new_inode, enum scriptfs_context new_context)
{
  printk("create_new_scriptfs_file#1\n");
  if(global_scriptfs_block_pool == NULL)
  {
    if(init_global_scriptfs_block_pool() == -1)
    {
      printk("create_new_scriptfs_file#2\n");
      printk(PANIC_SCRIPTFS);
      return -1;
    }
    printk("create_new_scriptfs_file#3\n");
  }
  printk("create_new_scriptfs_file#4\n");
  struct scriptfs_block* new_block  = init_scriptfs_block(new_context,new_inode);
  printk("create_new_scriptfs_file#5\n");
  if(new_block == NULL)
  {
    printk("create_new_scriptfs_file#6\n");
    printk(PANIC_SCRIPTFS);
    return -1;
  }
  printk("create_new_scriptfs_file#7\n");
  append_new_block_to_global_pool(new_block);
  printk("create_new_scriptfs_file#8\n");
  current_scriptfs_file = new_block;
}
int append_new_block_to_global_pool(struct scriptfs_block* new_block)
{
  struct scriptfs_block_pool* head = global_scriptfs_block_pool->head;
  printk("append_new_block_to_global_pool#1\n");
  if(head->block_node== NULL)
  {
    printk("append_new_block_to_global_pool#2\n");
    head->block_node = new_block;
    head->scriptfs_block_inode = new_block->inode;
    printk("append_new_block_to_global_pool#3\n");
    head->head = global_scriptfs_block_pool;
    printk("append_new_block_to_global_pool#4\n");
    head->next = NULL;
    printk("append_new_block_to_global_pool#5\n");
    return 0;
  }
  while(head->next!=NULL)
  {
    printk("append_new_block_to_global_pool#6\n");
    head = head->next;
    printk("append_new_block_to_global_pool#7\n");
  }
  printk("append_new_block_to_global_pool#8\n");
  head->next  = kmalloc(sizeof(struct scriptfs_block_pool), GFP_KERNEL);
  printk("append_new_block_to_global_pool#9\n");
  head = head->next;
  head->scriptfs_block_inode = new_block->inode;
  printk("append_new_block_to_global_pool#10\n");
  head->block_node = new_block;
  printk("append_new_block_to_global_pool#11\n");
  head->head = global_scriptfs_block_pool->head;
  printk("append_new_block_to_global_pool#12\n");
  head->next  = NULL;
  printk("append_new_block_to_global_pool#13\n");
  return 0;
}

/**
pre-condition:the inode must belong to a an existent
file in scriptfs
*/
int delete_scriptfs_file(unsigned long inode)
{

  struct scriptfs_block_pool* head  = global_scriptfs_block_pool->head;
  printk("delete_scriptfs_file#1\n");
  if(head == NULL)
  {
    printk("delete_scriptfs_file#2\n");
    printk(PANIC_SCRIPTFS);
    return -1;
  }
  printk("delete_scriptfs_file#3\n");
  if(head->next == NULL)
  {
    printk("delete_scriptfs_file#4\n");
    if(head->scriptfs_block_inode != inode)
    {
      printk("delete_scriptfs_file#5\n");
      printk(PANIC_SCRIPTFS);
      return -1;
    }
    printk("delete_scriptfs_file#6\n");
    scriptfs_free_pages(head->block_node->pages, head->block_node->context);
    printk("delete_scriptfs_file#7\n");
    kfree(head->block_node);
    printk("delete_scriptfs_file#8\n");
    printk("deleting file with inode:%lu\n", inode);
    return 0;
  }
  while(head!=NULL)
  {
    printk("delete_scriptfs_file#9\n");
    if(head->next->scriptfs_block_inode == inode)
    {
      printk("delete_scriptfs_file#10\n");
      struct scriptfs_block_pool* old_next  = head->next;
      printk("delete_scriptfs_file#11\n");
      old_next->next  = NULL;
      printk("delete_scriptfs_file#12\n");
      struct scriptfs_block_pool* new_next =  head->next->next;
      printk("delete_scriptfs_file#13\n");
      head->next = new_next;
      printk("delete_scriptfs_file#4\n");
      scriptfs_free_pages(old_next->block_node->pages, head->block_node->context );
      printk("delete_scriptfs_file#14\n");
      kfree(old_next->next);
      printk("delete_scriptfs_file#15\n");
      kfree(old_next);
      printk("deleting file with inode:%lu\n", inode);
      return 0;
    }
    printk("delete_scriptfs_file#16\n");
    head = head->next;
    printk("delete_scriptfs_file#17\n");
  }
  printk("delete_scriptfs_file#18\n");
  printk(PANIC_SCRIPTFS);
  return -1;
}
/**
Smart function that fetches the next available page based on an
inode.
This inode usually comes from virtual memory.
The time complexity of this is O(N).

*/
struct page* fetch_next_page(unsigned long inode)
{
  struct scriptfs_block_pool* head  = global_scriptfs_block_pool->head;
  struct page* new_page;
  // printk("fetch_next_page#1\n");
  while(head!=NULL)
  {
    // printk("fetch_next_page#2\n");
    if(head->scriptfs_block_inode == inode)
    {
      // printk("fetch_next_page#3\n");
      new_page = head->block_node->pages[head->block_node->current_index];
      // printk("fetch_next_page#4\n");
      head->block_node->current_index++;
      // printk("fetch_next_page#5\n");
      if(head->block_node->current_index>=get_context_page_size(head->block_node->context))
      {
        // printk("fetch_next_page#6\n");
        printk(PANIC_SCRIPTFS);
        return NULL;
      }
      // printk("fetch_next_page#7\n");
      return new_page;
    }
    // printk("fetch_next_page#8\n");
    head = head->next;
  }
  // printk("fetch_next_page#9\n");
  return NULL;
}
int get_context_page_size(enum scriptfs_context new_context)
{
  switch(new_context){
    case scriptfs_context_POEM:
      return POEM_PAGES_SIZE;
    case scriptfs_context_SMALL_NOVEL:
      return  SMALL_NOVEL_PAGES_SIZE;
    case scriptfs_context_MEDIUM_NOVEL:
    return MEDIUM_NOVEL_PAGES_SIZE;
    case scriptfs_context_LARGE_NOVEL:
      return  LARGE_NOVEL_PAGES_SIZE;
    case scriptfs_context_SUPER_NOVEL:
      return SUPER_NOVEL_PAGES_SIZE;
    case scriptfs_context_NO_CONTEXT:
    return -1;

    }
}
/**
Frees an array of page pointers .
pages:beginning of the array.
num_pages: number of pages to free.
put_page is called for each page.
*/
int scriptfs_free_pages(struct page** pages, int num_pages)
{
  int i =0;
  printk("deleting %d on scriptfs_free_pages\n",num_pages);
  while(i<num_pages)
  {
    put_page(pages[i]);
    i++;
  }
  return 0;
}

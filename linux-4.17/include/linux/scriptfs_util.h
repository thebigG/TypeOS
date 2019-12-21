#ifndef SCRIPTFS_UTIL
#define SCRIPTFS_UTIL
#include <linux/mm_types.h>
//This is the number of pages a poem has
#define POEM_PAGES_SIZE 8
#define SHORT_NOVEL_PAGES_SIZE 32
#define SMALL_NOVEL_PAGES_SIZE 64
#define MEDIUM_NOVEL_PAGES_SIZE 128
#define LARGE_NOVEL_PAGES_SIZE 512
#define SUPER_NOVEL_PAGES_SIZE 64000
#define BIT_RANGE_TABLE_ROWS 1024
/**
scriptfs_state represents the current state of scriptfs
MOUNT=this means that scriptfs is being mounted. This is usually used for pre-allocation of pages.
WRITE=this a write operation is ocurring. This could be used to map the adress space of a
in-memory file to that of a page in scriptfs.
READ=this means a file is being read.
NOOP= this is used to represent "No-scriptfs-operation". Basically
scriptfs does not care about this state in the sense it does not take any allocation
upon it, but it's extrememly important so that sciprtfs does not get in the way of
other file systems that may use ramfs.
*/
enum scriptfs_state{scriptfs_state_MOUNT,
  scriptfs_state_WRITE,
  scriptfs_state_READ, scriptfs_state_NOOP};

enum scriptfs_context{scriptfs_context_POEM,
                      scriptfs_context_SHORT_NOVEL,
                      scriptfs_context_SMALL_NOVEL,
                      scriptfs_context_MEDIUM_NOVEL,
                      scriptfs_context_LARGE_NOVEL,
                      scriptfs_context_SUPER_NOVEL,
                      scriptfs_context_NO_CONTEXT
                      };

/**
context-based file system block.
*/
struct scriptfs_block
{
  struct page** pages;
  int current_index; // this is used to map the requested page to virtual memory
  unsigned long inode;
  enum scriptfs_context context;
};
/**
Linked list of scriptfs_blocks
*/
struct scriptfs_block_pool
{
  struct scriptfs_block_pool* head;
  struct scriptfs_block* block_node;
  unsigned long scriptfs_block_inode;
  struct scriptfs_block_pool* next;
};
struct poem
{
  struct page* poem_page;
};
struct linked_list_pages
{
  struct page* page;
  struct linked_list_pages* head;
  struct linked_list_pages* next;
};
struct two_d_coord
{
  int row; //inclusive
  int col; //inclusive
};
struct file_range_mapping
{
  int inode;
  int start_bit; // inclusive
  int end_bit; //exclusive
};


#define PANIC_SCRIPTFS "PANIC!!! SCRIPTFS on  %s:%d\n", __FILE__, __LINE__
//Variables to keep track of scriptfs)
extern int scriptfs_pid;
extern int current_poem_index;
extern enum scriptfs_state current_scriptfs_state;
extern enum scriptfs_context current_scriptfs_context;
extern struct scriptfs_block* current_scriptfs_file;
extern struct scriptfs_block_pool* global_scriptfs_block_pool;
extern struct page* global_page;
extern struct poem* scriptfs_poems; //an array of pages which represent a custom block for scriptfs
extern int is_scriptfs_pid(void);
extern int is_scriptfs_mounted(void);
extern int is_current_scriptfs_mounted(void);
extern struct page** init_pages(enum scriptfs_context);
extern struct scriptfs_block* init_scriptfs_block(enum scriptfs_context, unsigned long);
extern int scriptfs_free_pages(struct page** pages, int num_pages);
extern struct file_range_mapping* bit_range_table;
extern int init_file_range_mapping_table(void);
extern int add_entry_bit_range_table(unsigned long, int, int );
extern void remove_entry_bit_range_table(int);
extern int get_context_page_size( enum scriptfs_context);
extern int init_global_scriptfs_block_pool(void);
extern int create_new_scriptfs_file(unsigned long new_inode, enum scriptfs_context);
extern int delete_scriptfs_file(unsigned long new_inode);
extern int append_new_block_to_global_pool(struct scriptfs_block*);
extern struct page* fetch_next_page(unsigned long inode);
#endif

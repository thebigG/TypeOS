/*
 * Resizable simple ram filesystem for Linux.
 *
 * Copyright (C) 2000 Linus Torvalds.
 *               2000 Transmeta Corp.
 *
 * Usage limits added by David Gibson, Linuxcare Australia.
 * This file is released under the GPL.
 */

/*
 * NOTE! This filesystem is probably most useful
 * not as a real filesystem,  but as an example of
 * how virtual filesystems can be written.
 *
 * It doesn't get much simpler than this. Consider
 * that this file implements the full semantics of
 * a POSIX-compliant read-write filesystem.
 *
 * Note in particular how the filesystem does not
 * need to implement any data structures of its own
 * to keep track of the virtual data: using the VFS
 * caches is sufficient.

 */

#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/backing-dev.h>
#include <linux/ramfs.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/magic.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gfp.h>
#include "internal.h"
#include <linux/page_ref.h>
#include <linux/scriptfs_util.h>
int wirte_count = 0;
// extern struct page* global_page;
// extern enum scriptfs_state current_scriptfs_state;
// EXPORT_SYMBOL(global_page);
// EXPORT_SYMBOL(current_scriptfs_state);
struct ramfs_mount_opts {
	umode_t mode;
};

struct ramfs_fs_info {
	struct ramfs_mount_opts mount_opts;
};

#define RAMFS_DEFAULT_MODE	0755

static const struct super_operations ramfs_ops;
static const struct inode_operations ramfs_dir_inode_operations;


static const struct address_space_operations ramfs_aops = {
	.readpage	= simple_readpage,
	.write_begin	= simple_write_begin,
	.write_end	= simple_write_end,
	.set_page_dirty	= __set_page_dirty_no_writeback,
};

struct inode *ramfs_get_inode(struct super_block *sb,
				const struct inode *dir, umode_t mode, dev_t dev)
{
	struct inode * inode = new_inode(sb);

	if (inode) {
		inode->i_ino = get_next_ino();
		inode_init_owner(inode, dir, mode);
		inode->i_mapping->a_ops = &ramfs_aops;
		mapping_set_gfp_mask(inode->i_mapping, GFP_HIGHUSER);
		mapping_set_unevictable(inode->i_mapping);
		inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
		switch (mode & S_IFMT) {
		default:
			init_special_inode(inode, mode, dev);
			break;
		case S_IFREG:
			inode->i_op = &ramfs_file_inode_operations;
			inode->i_fop = &ramfs_file_operations;
			break;
		case S_IFDIR:
			inode->i_op = &ramfs_dir_inode_operations;
			inode->i_fop = &simple_dir_operations;

			/* directory inodes start off with i_nlink == 2 (for "." entry) */
			inc_nlink(inode);
			break;
		case S_IFLNK:
			inode->i_op = &page_symlink_inode_operations;
			inode_nohighmem(inode);
			break;
		}
	}
	return inode;
}

/*
 * File creation. Allocate an inode, and we're done..
 */
/* SMP-safe */
static int
ramfs_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{

	struct inode * inode = ramfs_get_inode(dir->i_sb, dir, mode, dev);
	printk("ramfs_mknod pid:%lu for  inode:%lu\n", current->pid, inode->i_ino);
	if(is_current_scriptfs_mounted())
	{
		printk("creating a  file for scriptfs with this inode:%lu\n", inode->i_ino);
		int return_val = 0;
		return_val = create_new_scriptfs_file(inode->i_ino, current_scriptfs_context);
	 if(return_val == -1)
	 {
		 printk("scriptfs failed to create a file!\n");
	 }
	 else
	 {
		 printk("scriptfs success on file creation!\n");
	 }
}
	int error = -ENOSPC;
	if (inode) {
		d_instantiate(dentry, inode);
		dget(dentry);	/* Extra count - pin the dentry in core */
		error = 0;
		dir->i_mtime = dir->i_ctime = current_time(dir);
	}
	return error;
}

static int ramfs_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode)
{
	int retval = ramfs_mknod(dir, dentry, mode | S_IFDIR, 0);
	if (!retval)
		inc_nlink(dir);
	return retval;
}

static int ramfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl)
{
	return ramfs_mknod(dir, dentry, mode | S_IFREG, 0);
}

static int ramfs_symlink(struct inode * dir, struct dentry *dentry, const char * symname)
{
	struct inode *inode;
	int error = -ENOSPC;

	inode = ramfs_get_inode(dir->i_sb, dir, S_IFLNK|S_IRWXUGO, 0);
	if (inode) {
		int l = strlen(symname)+1;
		error = page_symlink(inode, symname, l);
		if (!error) {
			d_instantiate(dentry, inode);
			dget(dentry);
			dir->i_mtime = dir->i_ctime = current_time(dir);
		} else
			iput(inode);
	}
	return error;
}

static const struct inode_operations ramfs_dir_inode_operations = {
	.create		= ramfs_create,
	.lookup		= simple_lookup,
	.link		= simple_link,
	.unlink		= simple_unlink,
	.symlink	= ramfs_symlink,
	.mkdir		= ramfs_mkdir,
	.rmdir		= simple_rmdir,
	.mknod		= ramfs_mknod,
	.rename		= simple_rename,
};

/*
 * Display the mount options in /proc/mounts.
 */
static int ramfs_show_options(struct seq_file *m, struct dentry *root)
{
	struct ramfs_fs_info *fsi = root->d_sb->s_fs_info;

	if (fsi->mount_opts.mode != RAMFS_DEFAULT_MODE)
		seq_printf(m, ",mode=%o", fsi->mount_opts.mode);
	return 0;
}

static const struct super_operations ramfs_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
	.show_options	= ramfs_show_options,
};

enum {
	Opt_mode,
	Opt_err
};

static const match_table_t tokens = {
	{Opt_mode, "mode=%o"},
	{Opt_err, NULL}
};

static int ramfs_parse_options(char *data, struct ramfs_mount_opts *opts)
{
	substring_t args[MAX_OPT_ARGS];
	int option;
	int token;
	char *p;

	opts->mode = RAMFS_DEFAULT_MODE;

	while ((p = strsep(&data, ",")) != NULL) {
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_mode:
			if (match_octal(&args[0], &option))
				return -EINVAL;
			opts->mode = option & S_IALLUGO;
			break;
		/*
		 * We might like to report bad mount options here;
		 * but traditionally ramfs has ignored all mount options,
		 * and as it is used as a !CONFIG_SHMEM simple substitute
		 * for tmpfs, better continue to ignore other mount options.
		 */
		}
	}

	return 0;
}

int ramfs_fill_super(struct super_block *sb, void *data, int silent)
{
	printk("filling super block for \n");
	struct ramfs_fs_info *fsi;
	struct inode *inode;
	int err;

	fsi = kzalloc(sizeof(struct ramfs_fs_info), GFP_KERNEL);
	sb->s_fs_info = fsi;
	if (!fsi)
		return -ENOMEM;

	err = ramfs_parse_options(data, &fsi->mount_opts);
	if (err)
		return err;

	sb->s_maxbytes		= MAX_LFS_FILESIZE;
	sb->s_blocksize		= PAGE_SIZE ;
	sb->s_blocksize_bits	= PAGE_SHIFT;
	sb->s_magic		= RAMFS_MAGIC;
	sb->s_op		= &ramfs_ops;
	sb->s_time_gran		= 1;
	printk("s_maxbytes= %ld\n",sb->s_maxbytes);
	printk("s_blocksize= %ld\n",sb->s_blocksize);
	printk("s_blocksize_bits= %ld\n",sb->s_blocksize_bits);
	printk("s_magic X4= %lu\n",sb->s_magic);
	inode = ramfs_get_inode(sb, NULL, S_IFDIR | fsi->mount_opts.mode, 0);
	gfp_t flags;
	flags = __GFP_THISNODE; //This flag seems to make it work
	sb->s_root = d_make_root(inode);
	pgoff_t index = 0;
	if(current_scriptfs_state == scriptfs_state_NOOP)
	{
		printk(">>scriptfs_state_NOOP(new)<<\n");
		// is_scriptfs_pid();
	}
	else if(current_scriptfs_state == scriptfs_state_READ)
	{
		printk(">>scriptfs_state_READ<<\n");
	}
	else if(current_scriptfs_state == scriptfs_state_WRITE)
	{
		printk(">>scriptfs_state_WRITE<<\n");
	}
	else if(current_scriptfs_state == scriptfs_state_MOUNT)
	{
		printk("MOUNT state for scripfs on ramfs\n");
		// int ret_val = init_poems();
		// if(ret_val == -1)
		// {
		// 	printk("init_poems failed  \n");
		// }
		// else
		// {
		// 	printk("init_poems success \n");
		// }
		// printk(">>scriptfs_state_MOUNT<<\n");
		// global_page =  __page_cache_alloc(flags);
		// __SetPageReferenced(global_page);
		// if(global_page == NULL)
		// {
		// 	printk("page allocation failed\n");
		// }
		// else
		// {
		//   printk("page allocation successful(new)\n");
		// 	if(global_page->next == NULL )
		// 	{
		// 		printk("next field is NULL in page\n");
		// 	}
		// 	else
		// 	{
		// 		printk("next field is NOT NULL in page\n");
		// 	}
		// 	// page_ref_add(global_page, 1);
		// 	printk("ref count for the global page:%d\n",page_count(global_page));
		// }
	}
	if(is_scriptfs_pid())
	{
		printk("scriptfs was here\n");
	}
/**
This block of code(starting with if(current_scriptfs_state == scriptfs_state_MOUNT)) might break everything,
to the point where the kernel won't boot.
You've been warned.
*/
// 	if(current_scriptfs_state == scriptfs_state_MOUNT)
// 	{
// 	global_page = pagecache_get_page(NULL, index, 0, flags);
// 	printk("MOUNT state on ramfs for pid:%d\n", current->pid);
// 	if(!global_page)
// 	{
// 		printk("There was an error allocating this page\n");
//
// 	}
// 	else
// 	{
// 		printk("No errors allocating page\n");
// 		printk("number of pages in inner struct:%d\n", global_page->pages);
// 		if(global_page->mapping!= NULL)
// 		{
// 			printk("Adreess space is NOT NULL for global_page on ramfs_fill_super\n");
// 			printk("inode host:%lu", global_page->mapping->host->i_ino);
// 		}
// 		else
// 		{
// 			printk("Adreess space is NULL for global_page on ramfs_fill_super\n");
// 		}
// 	}
// }
	if (!sb->s_root)
		return -ENOMEM;

	return 0;
}

struct dentry *ramfs_mount(struct file_system_type *fs_type,
	int flags, const char *dev_name, void *data)
{
	printk("ramfs_mount\n");
	return mount_nodev(fs_type, flags, data, ramfs_fill_super);
}
/**
Use this function for scriptfs clean up.

*/
static void ramfs_kill_sb(struct super_block *sb)
{
	current_poem_index = 0;
	// if(is_current_scriptfs_mounted())
	// {
	// if(scriptfs_poems !=NULL)
	// scriptfs_free_pages(scriptfs_poems, POEM_SIZE);
	// kfree();
	printk("killing ramfs???\n");
// }
	kfree(sb->s_fs_info);
	kill_litter_super(sb);
}

static struct file_system_type ramfs_fs_type = {
	.name		= "ramfs",
	.mount		= ramfs_mount,
	.kill_sb	= ramfs_kill_sb,
	.fs_flags	= FS_USERNS_MOUNT,
};

int __init init_ramfs_fs(void)
{
	static unsigned long once;
	printk("init_ramfs_fs\n");
	if (test_and_set_bit(0, &once))
		return 0;
	return register_filesystem(&ramfs_fs_type);
}
fs_initcall(init_ramfs_fs);

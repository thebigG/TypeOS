# scriptfs
A context-based file system that will hopefully make a writer's laptop last a 40-page short story without the annoying "you're laptop is about to die" warning.

**Linux Changelog**

- struct page in "include/linux/mm_types.h" has a new field called **scriptfs_flag**
- inode.c has been changed to behave good for scriptfs in fs/ramfs/inode.c
- The file kernel/scriptfs_util.c has been added
- The file "include/linux/scriptfs_util.h" has been added

| Category  |Size   |Number of pages(4KB granularity)   |
|---|---|---|
| Poem  |32KB   |8   |
| Short Story  |128KB   |32   |
| Small Novel  |256KB   |64   |
| Medium Novel  | 512KB  | 128  |
| Large Novel  | 2MB  | 512  |
|Super Novel   | 256MB  | 64K  |

#scriptfs stable changelog

| Version  | description  |
|---|---|
| 1.0  | Fixed issue where kernel was not booting. This was caused by trying to access the "current" macro boot time  |
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

enum scriptfs_context{
      scriptfs_context_POEM,
      scriptfs_context_SHORT_NOVEL,
      scriptfs_context_SMALL_NOVEL,
      scriptfs_context_MEDIUM_NOVEL,
      scriptfs_context_LARGE_NOVEL,
      scriptfs_context_SUPER_NOVEL,
      scriptfs_context_NO_CONTEXT
      };

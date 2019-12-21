from enum import Enum

class scriptfs_state(Enum):
    scriptfs_state_MOUNT = 0
    scriptfs_state_WRITE = 1
    scriptfs_state_READ = 2
    scriptfs_state_NOOP = 3

class scriptfs_context(Enum):
    scriptfs_context_POEM = 0
    scriptfs_context_SHORT_NOVEL = 1
    scriptfs_context_SMALL_NOVEL = 2
    scriptfs_context_MEDIUM_NOVEL = 3
    scriptfs_context_LARGE_NOVEL = 4
    scriptfs_context_SUPER_NOVEL = 5
    scriptfs_context_NO_CONTEXT = 6

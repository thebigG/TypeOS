import ctypes
libc = ctypes.CDLL(None)
syscall = libc.syscall
syscall_num = 334
syscall(syscall_num)

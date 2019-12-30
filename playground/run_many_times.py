import time
import sys
import subprocess
time_avg = 0
times  = int(sys.argv[1])
command  = sys.argv[2]
command_list = ["python3"]
for c in command.split():
    command_list.append(c)

for t in range(times):
    start = time.time()
    subprocess.call(command_list)
    end = time.time()
    time_avg += (end-start)

print("time average(in seconds):", time_avg/times)







import sys

file = open(sys.argv[1], "r")
lines = file.readlines()
avg = 0
final_avg = 0
for line in lines:
   if(len(line.split()) != 0):
      avg += sum( [len(word) for word in  line.split()])/len(line.split())


final_avg = avg/len(lines)
print("word length avgv:", final_avg)

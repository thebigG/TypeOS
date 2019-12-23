import sys
from decimal import Decimal

def cleanup_number_string(number_string):
    new_number_string = ""
    for char in number_string:
        if(char.isdigit()):
            new_number_string += char
        else:
            break
    return new_number_string


file_name = sys.argv[1]
f = open(file_name)
counter = 0
times = [ int(cleanup_number_string(line.split(":")[1].lstrip())) for line in f.read().lstrip().rstrip().split("\n") if int(cleanup_number_string(line.split(":")[1].lstrip()) )>0 ]
print("all times:", times)
print("times len:", len(times))
print("times len:{}".format(sum(times)/len(times)))

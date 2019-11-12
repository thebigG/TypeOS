import sys
import random

#I know there are better ways to generate words(like reading from a giant file of words, which linux has).
#But I don't want to interact with the filesystem unless it is for our intent and purposes, because that will bring inaccuracies we don't want.

character_pool = ['a','b','c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
't', 'u', 'v', 'w', 'x', 'y', 'z' ]

def get_size(size):
    num_string =  ""
    start_size_label_index = 0
    for char in size:
        if char.isdigit():
            num_string += char
            start_size_label_index += 1
    return (int(num_string), size[start_size_label_index:] )


def get_random_word(min_size, max_size):
    """
    This functon generates a random word. The size of this new word
    is a random number between "min_size" and "max_size".
    """
    size =  random.randint(min_size,max_size)
    word  = ''
    for i in range(size):
        word += character_pool[random.randrange(0,len(character_pool))]
    return word + " "

def sequential_writethrough( filepath, size ):
    """
    This function will sequentially write to a file the amount specified by "size".
    For example if size were 32kb, this function will write 32kb worth of words to
    the end of the file. It does not matter whether the file exists or not.
    If the file exists, it will simply append the new contents to the end of the file.
    If the file does not exist, the function will create a new file at "filepath" and
    write the contents to that new file.
    The content of this file is randomly generated words by calling the function get_random_word.
    """
    script_file  = open(filepath, 'a')
    size_info =  get_size(size)
    bytes = 0
    if size_info[1].upper() == 'KB':
        bytes = size_info[0]*1024
    elif size_info[1].upper() =="MB":
        bytes = size_info[0]*pow(2,20)
    for byte in range(bytes):
        word = get_random_word(3, 5)
        script_file.write(word)
    script_file.close()

if (len(sys.argv) < 2):
    print("USAGE: python3 ./make_script.py size ./file_path\n\
    For example:  python3 ./make_script.py 32kb 'How Alice met Bob' ")

file_name = sys.argv[2]
size = sys.argv[1]

sequential_writethrough(file_name, size)

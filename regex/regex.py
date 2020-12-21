#!/usr/bin/python3
import re

def main():
    f = open("gettysburg.txt", "r")
    for i in range(20):
        l = f.readline()
        if l == "":
            continue
        words = re.findall(r'\b[A-Z][a-z]*\b', l)
        print("line = {}".format(l))
        for w in words:
            print("w = {}".format(w))
        space = 0
        for j in l:
            if j == ' ':
                space += 1
        print("spaces = {}, via re = {}".format(space, len(words)))
    f.close()

if __name__ == "__main__":
    main()


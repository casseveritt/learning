#!/usr/bin/python3
import re


def main():
    f = open("gettysburg.txt", "r")
    s = f.read()
    f.close()
    print("file size = {}".format(len(s)))

    p = re.compile(r'\b[A-Za-z]+\b', re.MULTILINE)
    count = 0
    for m in p.finditer(s):
        count += 1
    
    print("found {} matches".format(count))

if __name__ == "__main__":
    main()

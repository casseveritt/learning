#!/usr/bin/python3
import re


def main():
    f = open("gettysburg.txt", "r")
    s = f.read()
    f.close()
    print("file size = {}".format(len(s)))

if __name__ == "__main__":
    main()

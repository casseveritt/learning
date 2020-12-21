#!/usr/bin/python3
import re


def main():
    with open("gettysburg.txt", "r") as f:
        for l in f:
            words = re.findall(r'\b[A-Za-z]+\b', l)
            print("re = {}".format(len(words)))


if __name__ == "__main__":
    main()

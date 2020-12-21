#!/usr/bin/python3
import re


def main():
    with open("gettysburg.txt", "r") as f:
        i = 0
        for l in f:
            i += 1
            if i >= 20:
                break
            words = re.findall(r'\b[A-Za-z]+\b', l)
            print("re = {}".format(len(words)))


if __name__ == "__main__":
    main()

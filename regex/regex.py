#!/usr/bin/python3
import re


def main():
    f = open("gettysburg.txt", "r")
    s = f.read()
    f.close()
    print("file size = {}".format(len(s)))

    p = re.compile(r'\b(a|an|the)\s([A-Za-z]+)\b', re.MULTILINE|re.IGNORECASE)

    count = 0
    d = {}
    for m in p.finditer(s):
        count += 1
        k = m.group(2)
        if k not in d:
            d[k] = 0
        d[k] += 1
    
    print("found {} matches".format(count))
    for k,v in d.items():
        print("{}: {}".format(k,v))

if __name__ == "__main__":
    main()

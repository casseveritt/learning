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
    
    top = sorted(d.items(), key = lambda i: -i[1])
    print("found {} matches\n".format(count))
    print("top five matches:")
    for v in top[0:5]:
        print("{}: {}".format(v[0], v[1]))

if __name__ == "__main__":
    main()

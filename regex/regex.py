#!/usr/bin/python3
import re
from operator import itemgetter
from hashlib import sha1

def make_sha1(s, encoding='utf-8'):
    return sha1(s.encode(encoding)).hexdigest()

def hist_dict_incr(d, k):
    if k not in d:
        d[k] = 0
    d[k] += 1

def main():
    with open("gettysburg.txt", "r") as f:
        s = f.read()

    p = re.compile(r'\b(a|an|the)\s([A-Za-z]+)\b', re.M|re.I)

    count = 0
    d = {}
    for m in p.finditer(s):
        count += 1
        hist_dict_incr(d, m.group(2))
    
    top = sorted(d.items(), reverse=True, key=itemgetter(1))[0:5]
    print("found {c} matches and {k} unique words\n".format(c=count, k=len(d)))
    print("top five matches:")
    for k, v in top:
        print("{}: {}".format(k, v))
    
    pat = r'\b(a|an|the)\s+({})\b'.format('|'.join([t[0] for t in top]))

    rs = re.sub(pat, lambda m: "{} {}".format(m.group(1), m.group(2).upper()), s)

    print(rs)
    print("sha1: {}".format(make_sha1(rs)))

if __name__ == "__main__":
    main()

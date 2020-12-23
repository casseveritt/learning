#!/usr/bin/python3
import re
from operator import itemgetter
from hashlib import sha1

def sha1sum(s, encoding='utf-8'):
    return sha1(s.encode(encoding)).hexdigest()

def hist_dict_incr(d, k):
    if k not in d:
        d[k] = 0
    d[k] += 1

def main():
    with open("x.h", "r") as f:
        s = f.read()

    rs = s

    count = [0]
    def r(m, count, r=' '):
        count[0] += 1
        print("{}: {}".format(count[0], m.group()))
        return r

    rs = re.sub(r'/\*.*?\*/', lambda m: r(m, count), rs, flags=re.DOTALL)
    rs = re.sub(r'//.*?\n', lambda m: r(m, count, '\n'), rs)
    block = r'\{[^{]*?\}'
    rs = re.sub(block, lambda m: r(m, count, "B{}".format(count[0])), rs, flags=re.DOTALL)
    rs = re.sub(block, lambda m: r(m, count, "B{}".format(count[0])), rs, flags=re.DOTALL)
    rs = re.sub(block, lambda m: r(m, count, "B{}".format(count[0])), rs, flags=re.DOTALL)
    rs = re.sub(block, lambda m: r(m, count, "B{}".format(count[0])), rs, flags=re.DOTALL)
    rs = re.sub(block, lambda m: r(m, count, "B{}".format(count[0])), rs, flags=re.DOTALL)
    rs = re.sub(block, lambda m: r(m, count, "B{}".format(count[0])), rs, flags=re.DOTALL)

    with open("y.h", "w") as f:
        f.write(rs)

    print("sha1: {}".format(sha1sum(rs)))

if __name__ == "__main__":
    main()

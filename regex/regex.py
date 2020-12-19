#!/usr/bin/python3

def main():
    print("Hello, World!")
    f = open("frankenstein.txt", "r")
    for i in range(20):
        l = f.readline()
        print(l, end='')
    f.close()

if __name__ == "__main__":
    main()


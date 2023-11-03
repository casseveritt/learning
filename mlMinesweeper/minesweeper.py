from dataclasses import dataclass
from random import random

w: int = 10
h: int = 10
b: int = 25

@dataclass
class tile:
    rev: bool = False
    bom: bool = False
    prox: int = 0 

board: list[tile]

def main():
    tiles: int = w*h
    for in range(w):
        for in range(h):
            r: float = random()
            t: tile()

    print("Hello World!")


main()
from dataclasses import dataclass
from random import random

w: int = 5
h: int = 5
b: int = 6

@dataclass
class tile:
    bom: bool
    rvl: bool = False
    prox: int = 0 

board = []

def index(x,y):
    i: int = x + (y*h)
    return i

def initBoard(x,y):
    tiles: int = w*h
    bombs: int = b
    for xCoord in range(w):
        for yCoord in range(h):
            rand: float = random()
            prob: float = bombs/tiles
            print(str(bombs), end=" ")
            print(str(rand), end = " ")
            print(str(prob))
            if rand <= prob:
                t: tile
                if False:
                    t = tile(False)
                    t.bom = False
                else:
                    t = tile(True)
                    t.bom = True
                    bombs = bombs - 1
                board.append(t)
                print(str(board[-1].bom))
            else:
                t: tile = tile(False)
                t.bom = False
                board.append(t)
            tiles = tiles - 1

def printBoard():
    count: int = 0
    bombCount: int = 0
    for til in board:
        if til.bom:
            print("x ", end="")
            bombCount = bombCount + 1
        else:
            print("o ", end="")
        count = count + 1
        if count == (w):
            print("")
            count = 0
    print(str(bombCount))

def main():

    initBoard(0,0)

    printBoard()
    print(str(len(board)))

main()
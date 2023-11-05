from dataclasses import dataclass
from random import random

w: int = 10
h: int = 10
b: int = 25
gameRunning: bool = True

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
            if rand <= prob:
                t: tile
                if abs(xCoord-x) <= 1 and abs(yCoord-y) <= 1:
                    t = tile(False)
                    t.bom = False
                else:
                    t = tile(True)
                    t.bom = True
                    bombs = bombs - 1
                board.append(t)
            else:
                t: tile = tile(False)
                t.bom = False
                board.append(t)
            tiles = tiles - 1
    xCoord: int = 0
    yCoord: int = 0
    for tie in board: # Increment count of tiles around a bomb
        if tie.bom:
            if xCoord > 0 and yCoord < h-1:
                board[index(xCoord-1,yCoord-1)].prox += 1
            if xCoord > 0:
                board[index(xCoord-1,yCoord)].prox += 1
            if xCoord > 0 and yCoord > 0:
                board[index(xCoord-1,yCoord+1)].prox += 1
            if yCoord < h-1:
                board[index(xCoord,yCoord-1)].prox += 1
            if yCoord > 0:
                board[index(xCoord,yCoord+1)].prox += 1
            if xCoord < w-1 and yCoord < h-1:
                board[index(xCoord+1,yCoord-1)].prox += 1
            if xCoord < w-1:
                board[index(xCoord+1,yCoord)].prox += 1
            if xCoord < w-1 and yCoord > 0:
                board[index(xCoord+1,yCoord+1)].prox += 1
        xCoord += 1
        if xCoord == 10:
            xCoord = 0
            yCoord += 1
    

def printBoard():
    count: int = 0
    for til in board:
        if til.bom:
            print("X", end=" ")
        else:
            print(str(til.prox), end=" ")
        count = count + 1
        if count == (w):
            print("")
            count = 0

def main():
        
    initBoard(3,3)
        
    printBoard()

main()
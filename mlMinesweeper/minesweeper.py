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

def printBoard(x,y):
    count: int = 0
    boardIndex: int = 0
    for til in board:
        if boardIndex == (y*w)+x:
            print("*", end=" ")
        elif til.bom:
            print("X", end=" ")
        else:
            print(str(til.prox), end=" ")
        count += 1
        boardIndex += 1
        if count == (w):
            print("")
            count = 0

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
    
    for i in range(h):
        for j in range(w):
            if board[(i*w)+j].bom:
                if i > 0 and j > 0:
                    board[((i-1)*w)+(j-1)].prox += 1
                if i > 0:
                    board[((i-1)*w)+j].prox += 1
                if i > 0 and j < (w-1):
                    board[((i-1)*w)+(j+1)].prox += 1
                
                if j > 0:
                    board[(i*w)+(j-1)].prox += 1
                if j < (w-1):
                    board[(i*w)+(j+1)].prox += 1
                
                if i < (h-1) and j > 0:
                    board[((i+1)*w)+(j-1)].prox += 1
                if i < (h-1):
                    board[((i+1)*w)+j].prox += 1
                if i < (h-1) and j < (w-1):
                    board[((i+1)*w)+(j+1)].prox += 1

def main():
    
    initBoard(3,3)
        
    printBoard(3,3)

def new_func():
    return 0

main()
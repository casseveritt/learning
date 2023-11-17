from dataclasses import dataclass
from random import random

w: int = 10
h: int = 10
b: int = 20
gameRunning: bool = True

@dataclass
class tile:
    bom: bool
    ind: int
    rvl: bool = False
    flg: bool = False
    checked: bool = False
    prox: int = 0

board = []
proximity = []

def index(x,y):
    i: int = x + (y*w)
    return i

def printBoard():
    count: int = 0
    boardIndex: int = 0
    for til in board:
        til.checked = False
        if til.rvl:
            if til.bom:
                print("X", end=" ")
            elif til.prox == 0:
                print(".", end=" ")
            else:
                print(str(til.prox), end=" ")
        elif til.flg:
            if til.bom:
                print("=", end=" ")
            else:
                print("/", end=" ")
        else:
            print("#", end=" ")
        count += 1
        boardIndex += 1
        if count == (w):
            print("")
            count = 0

def printTile(ind):
    if board[ind].rvl:
        if board[ind].bom:
            print("X", end=" ")
        elif board[ind].prox == 0:
            print(".", end=" ")
        else:
            print(str(board[ind].prox), end=" ")
    elif board[ind].flg:
        if board[ind].bom:
            print("=", end=" ")
        else:
            print("/", end=" ")
    elif board[ind].bom and board[ind].rvl == False:
        print("@", end=" ")
    else:
        print("#", end=" ")

def printProx():
    for index in range(len(proximity)-1):
        printTile(proximity[index])
        if abs(proximity[index] - proximity[index+1]) == 2:
            print("o", end=" ")
    printTile(proximity[len(proximity)-1])

def findProx(ind):
    x: int = ind % w
    y: int = ind // w
    proximity.clear()
    if y > 0 and x > 0:
        proximity.append(index(x-1, y-1))
    if y > 0:
        proximity.append(index(x, y-1))
    if y > 0:
        proximity.append(index(x+1, y-1))
            
    if x > 0:
        proximity.append(index(x-1, y))
    if x < (w-1):
        proximity.append(index(x+1, y))
    
    if y < (h-1) and x > 0:
        proximity.append(index(x-1, y+1))
    if y < (h-1):
        proximity.append(index(x, y+1))
    if y < (h-1) and x < (w-1):
        proximity.append(index(x+1, y+1))

def reveal(ind):
    board[ind].rvl = True
    if board[ind].prox == 0:
        findProx(ind)
        neighbors = []
        neighbors = proximity.copy()
        for proxCoord in neighbors:
            if board[proxCoord].rvl == False:
                reveal(proxCoord)

def initBoard(x,y):
    tiles: int = w*h
    bombs: int = b
    boardIndex: int = 0
    for xCoord in range(w):
        for yCoord in range(h):
            rand: float = random()
            prob: float = bombs/tiles
            if rand <= prob:
                t: tile
                if abs(xCoord-x) <= 1 and abs(yCoord-y) <= 1:
                    t = tile(False, boardIndex)
                    t.bom = False
                else:
                    t = tile(True, boardIndex)
                    t.bom = True
                    bombs = bombs - 1
                board.append(t)
            else:
                t: tile = tile(False, boardIndex)
                t.bom = False
                board.append(t)
            tiles = tiles - 1
            boardIndex += 1
    
    for ind in range(h*w):
        if board[ind].bom:
            findProx(ind)
            for proxCoord in proximity:
                board[proxCoord].prox += 1
    reveal(index(x,y))

def evalTile(ind):
    board[ind].checked = True
    revealedTiles: int = 0
    flaggedTiles: int = 0
    findProx(ind)
    localGroup = []
    localGroup = proximity.copy()
    for proxCoord in localGroup:
        if board[proxCoord].rvl:
            revealedTiles += 1
        if board[proxCoord].flg:
            flaggedTiles += 1
    
    printProx()
    
    print("Prox:", board[ind].prox, "Revealed:", revealedTiles, "Flagged:", flaggedTiles, "Tiles:", len(localGroup), end="\n")
    
    if len(localGroup) == (revealedTiles + flaggedTiles):
        for proxCoord in localGroup:
            if board[proxCoord].rvl == True and board[proxCoord].checked == False:
                evalTile(proxCoord)
                
    #if flaggedTiles == board[ind].prox:
    #    print("Revealing")
    #    for proxCoord in localGroup:
    #        if board[proxCoord].rvl == False and board[proxCoord].flg == False:
    #            reveal(proxCoord)
    
    elif (len(localGroup)-revealedTiles) == board[ind].prox:
        print("Flagging\n")
        for proxCoord in localGroup:
            if board[proxCoord].rvl == False:
                board[proxCoord].flg = True

def main():
    
    initBoard(3,3)
        
    printBoard()
    
    print("")
    
    evalTile(index(3,3))
    
    printBoard()
    
    #print("")
    
    #evalTile(3,3)
    
    #printBoard()

def new_func():
    return 0

main()
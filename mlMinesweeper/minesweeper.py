from dataclasses import dataclass
from random import random

w: int = 10
h: int = 10
b: int = 20
gameRunning: bool = True

@dataclass
class tile:
    bom: bool
    x: int
    y: int
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
            print("=", end=" ")
        else:
            print("#", end=" ")
        count += 1
        boardIndex += 1
        if count == (w):
            print("")
            count = 0
            
def printProx():
    count: int = 0
    boardIndex: int = 0
    for til in proximity:
        til.checked = False
        if til.rvl:
            if til.bom:
                print("X", end=" ")
            elif til.prox == 0:
                print(".", end=" ")
            else:
                print(str(til.prox), end=" ")
        elif til.flg:
            print("=", end=" ")
        else:
            print("#", end=" ")
        count += 1
        boardIndex += 1
        if count == (w):
            print("")
            count = 0

def findProx(x,y):
    del proximity [ : ]
    if y > 0 and x > 0:
        proximity.append(board[index(x-1, y-1)])
    if y > 0:
        proximity.append(board[index(x, y-1)])
    if y > 0:
        proximity.append(board[index(x+1, y-1)])
            
    if x > 0:
        proximity.append(board[index(x-1, y)])
    if x < (w-1):
        proximity.append(board[index(x+1, y)])
    
    if y < (h-1) and x > 0:
        proximity.append(board[index(x-1, y+1)])
    if y < (h-1):
        proximity.append(board[index(x, y+1)])
    if y < (h-1) and x < (w-1):
        proximity.append(board[index(x+1, y+1)])

def reveal(x,y):
    board[index(x,y)].rvl = True
    localProximity = []
    if board[index(x,y)].prox == 0:
        findProx(x,y)
        localProximity = proximity
        for proxTile in localProximity:
            if proxTile.rvl == False:
                printProx()
                print("\nRevealing ", proxTile.x, proxTile.y, proxTile.rvl)
                printBoard()
                reveal(proxTile.x, proxTile.y)
                printBoard()

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
                    t = tile(False, xCoord, yCoord)
                    t.bom = False
                else:
                    t = tile(True, xCoord, yCoord)
                    t.bom = True
                    bombs = bombs - 1
                board.append(t)
            else:
                t: tile = tile(False, xCoord, yCoord)
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
    reveal(x,y)

def evalTile(x,y):
    board[(y*w)+x].checked = True
    revealedTiles: int = 0
    flaggedTiles: int = 0
    tilesAround: int = 0
    if y > 0 and x > 0:
        tilesAround += 1
        if board[((y-1)*w)+(x-1)].rvl:
            revealedTiles += 1
        elif board[((y-1)*w)+(x-1)].flg:
            flaggedTiles += 1
    if y > 0:
        tilesAround += 1
        if board[((y-1)*w)+x].rvl:
            revealedTiles += 1
        elif board[((y-1)*w)+x].flg:
            flaggedTiles += 1
    if y > 0 and x < (w-1):
        tilesAround += 1
        if board[((y-1)*w)+(x+1)].rvl:
            revealedTiles += 1
        elif board[((y-1)*w)+(x+1)].flg:
            flaggedTiles += 1
            
    if x > 0:
        tilesAround += 1
        if board[(y*w)+(x-1)].rvl:
            revealedTiles += 1
        elif board[(y*w)+(x-1)].flg:
            flaggedTiles += 1
    if x < (w-1):
        tilesAround += 1
        if board[(y*w)+(x+1)].rvl:
            revealedTiles += 1
        elif board[(y*w)+(x+1)].flg:
            flaggedTiles += 1
    
    if y < (h-1) and x > 0:
        tilesAround += 1
        if board[((y+1)*w)+(x-1)].rvl:
            revealedTiles += 1
        elif board[((y+1)*w)+(x-1)].flg:
            flaggedTiles += 1
    if y < (h-1):
        tilesAround += 1
        if board[((y+1)*w)+x].rvl:
            revealedTiles += 1
        elif board[((y+1)*w)+x].flg:
            flaggedTiles += 1
    if y < (h-1) and x < (w-1):
        tilesAround += 1
        if board[((y+1)*w)+(x+1)].rvl:
            revealedTiles += 1
        elif board[((y+1)*w)+(x+1)].flg:
            flaggedTiles += 1
    
    if tilesAround == (revealedTiles + flaggedTiles):
        if y > 0 and x > 0 and board[((y-1)*w)+(x-1)].rvl == True and board[((y-1)*w)+(x-1)].checked == False:
            evalTile(x-1, y-1)
        if y > 0 and board[((y-1)*w)+x].rvl == True and board[((y-1)*w)+x].checked == False:
            evalTile(x, y-1)
        if y > 0 and x < (w-1) and board[((y-1)*w)+(x+1)].checked == False:
            evalTile(x+1, y-1)
                
        if x > 0 and board[(y*w)+(x-1)].rvl == True and board[(y*w)+(x-1)].checked == False:
            evalTile(x-1, y)
        if x < (w-1) and board[(y*w)+(x+1)].rvl == True and board[(y*w)+(x+1)].checked == False:
            evalTile(x+1, y)
        
        if y < (h-1) and x > 0 and board[((y+1)*w)+(x-1)].rvl == True and board[((y+1)*w)+(x-1)].checked == False:
            evalTile(x-1, y+1)
        if y < (h-1) and board[((y+1)*w)+x].rvl == True and board[((y+1)*w)+x].checked == False:
            evalTile(x, y+1)
        if y < (h-1) and x < (w-1) and board[((y+1)*w)+(x+1)].rvl == True and board[((y+1)*w)+(x+1)].checked == False:
            evalTile(x+1, y+1)
    
    if (tilesAround-revealedTiles) == board[(y*w)+x].prox:
        print("Flagging")
        if y > 0 and x > 0 and board[((y-1)*w)+(x-1)].rvl == False:
            board[((y-1)*w)+(x-1)].flg = True
        if y > 0 and board[((y-1)*w)+x].rvl == False:
            board[((y-1)*w)+x].flg = True
        if y > 0 and x < (w-1) and board[((y-1)*w)+(x+1)].rvl == False:
            board[((y-1)*w)+(x+1)].flg = True
                
        if x > 0 and board[(y*w)+(x-1)].rvl == False:
            board[(y*w)+(x-1)].flg = True
        if x < (w-1) and board[(y*w)+(x+1)].rvl == False:
            board[(y*w)+(x+1)].flg = True
        
        if y < (h-1) and x > 0 and board[((y+1)*w)+(x-1)].rvl == False:
            board[((y+1)*w)+(x-1)].flg = True
        if y < (h-1) and board[((y+1)*w)+x].rvl == False:
            board[((y+1)*w)+x].flg = True
        if y < (h-1) and x < (w-1) and board[((y+1)*w)+(x+1)].rvl == False:
            board[((y+1)*w)+(x+1)].flg = True
    
    elif flaggedTiles == board[(y*w)+x].prox:
        if y > 0 and x > 0 and board[((y-1)*w)+(x-1)].rvl == False and board[((y-1)*w)+(x-1)].flg == False:
            reveal(x-1, y-1)
        if y > 0 and board[((y-1)*w)+x].rvl == False:
            reveal(x, y-1)
        if y > 0 and x < (w-1) and board[((y-1)*w)+(x+1)].rvl == False and board[((y-1)*w)+(x+1)].flg == False:
            reveal(x+1, y-1)
                
        if x > 0 and board[(y*w)+(x-1)].rvl == False and board[(y*w)+(x-1)].flg == False:
            reveal(x-1, y)
        if x < (w-1) and board[(y*w)+(x+1)].rvl == False and board[(y*w)+(x+1)].flg == False:
            reveal(x+1, y)
        
        if y < (h-1) and x > 0 and board[((y+1)*w)+(x-1)].rvl == False and board[((y+1)*w)+(x-1)].flg == False:
            reveal(x-1, y+1)
        if y < (h-1) and board[((y+1)*w)+x].rvl == False and board[((y+1)*w)+x].flg == False:
            reveal(x, y+1)
        if y < (h-1) and x < (w-1) and board[((y+1)*w)+(x+1)].rvl == False and board[((y+1)*w)+(x+1)].flg == False:
            reveal(x+1, y+1)

def main():
    
    initBoard(3,3)
        
    printBoard()
    
    print("")
    
    #evalTile(3,3)
    
    #printBoard()
    
    #print("")
    
    #evalTile(3,3)
    
    #printBoard()

def new_func():
    return 0

main()
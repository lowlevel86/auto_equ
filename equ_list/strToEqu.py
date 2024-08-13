#!/bin/python3
from sympy import *

A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z = symbols('A B C D E F G H I J K L M N O P Q R S T U V W X Y Z')

# change symbols
#A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R = symbols('x y lgth h t tCnt h_prior t_prior tCnt_prior l->x l->y l->offset l->headSz l->tailSegSz l->tailSegCnt l->headSzPrior l->tailSegSzPrior l->tailSegCntPrior')


# load the file
with open('equList.txt', 'r') as file:
    file_contents = file.read()

# split the contents into an array of strings and remove newlines
equList = [line.strip() for line in file_contents.splitlines()]

for equ in equList:
    ptr = len(equ) - 3
    newEqu = equ[ptr:]

    for i in range(ptr, 0, -6):
        newEqu = newEqu.replace(equ[i-2:i-1], "("+equ[i-6:i-3]+")")

    print(eval(newEqu))


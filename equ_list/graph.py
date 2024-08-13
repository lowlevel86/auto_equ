#!/bin/python3

import matplotlib.pyplot as plt
from math import *

with open('coverage.txt', 'r') as file:
    val_strs = file.read()

yVals = []
valsInc = 1
for val_str in val_strs.split('\n'):
    for n in val_str.split(','):
        if n:
            yVals.append(int(n))
    xVals = [valsInc] * len(yVals)
    valsInc += 1
    plt.plot(yVals, xVals, '.', markersize=3, alpha=0.5)    

plt.xlabel("Coverage")
plt.ylabel("Equation")

plt.show()


import numpy as np
import pylab as pl
import sys

F = np.load(sys.argv[1])

R=F['R']    # final recalled sequences
D=F['D']    # distance of final recalled state from target
C=F['C']    # correct
T=F['T']    # final trial in which error incremented
E=F['E']    # final error
N=F['N']    # number of nodes in network

print(np.sum(C)/len(C))

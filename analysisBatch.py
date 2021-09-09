import numpy as np
import sys

F = np.load(sys.argv[1])

R=F['R']    # final recalled sequences
D=F['D']    # distance of final recalled state from target
T=F['T']    # final trial in which error incremented  (NOT necessarily successful!)
E=F['E']    # final error (average over contexts)
N=F['N']    # number of nodes in network


Nsims = len(D)

IndexGoodAll = np.where(E==-N)[0]
NGoodAll = len(IndexGoodAll)
probGoodAll = NGoodAll/Nsims
print('Perfect at all associations:\n'+str(probGoodAll*100)+'%')

IndexGoodFinal = np.where(D==-N)[0]
NGoodFinal = len(IndexGoodFinal)
probGoodFinal = NGoodFinal/Nsims
print('Perfect at last pattern in recall:\n'+str(probGoodFinal*100)+'%')

import h5py
import numpy as np
import os
import sys

path = sys.argv[1] # e.g., 'logs'
subdirs = [f.path for f in os.scandir(path) if f.is_dir()]

tag = "sentence: "

R = np.array('')
D = np.array([0])
T = np.array([0])
E = np.array([0])
N = np.array([0])

for fname in subdirs:

    f = h5py.File(fname+'/data.h5')

    n = f['N'][:][0]
    N = np.hstack([N,n])

    d = f['finalDistance'][:][0]
    D = np.hstack([D,d])

    T = np.hstack([T,f['trialInc'][:][-1]])
    E = np.hstack([E,f['errorInc'][:][-1]])

    for p in f.keys():
        if(~p.find(tag)):
            recall = p.split(tag)[1]
    f.close()
    R = np.hstack([R,recall])

R = R[1:]   # final recalled sequences
D = D[1:]   # distance of final recalled state from target
T = T[1:]   # final trial in which error incremented (NOT necessarily successful!)
E = E[1:]   # final error (average over contexts)
N = N[1:]   # number of nodes in network

np.savez(sys.argv[2]+'.npz',R=R,D=D,T=T,E=E,N=N,subdirs=subdirs,path=path)

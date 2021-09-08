import h5py
import numpy as np
import pylab as pl
import sys

fname = sys.argv[1]

f = h5py.File(fname)
t = f['trialInc'][:]
e = f['errorInc'][:]
s = f['stepsInc'][:]
c = f['cyLenInc'][:]
d = f['cyDisInc'][:]

ef = f['errorFin'][:]
sf = f['stepsFin'][:]
cf = f['cyLenFin'][:]
df = f['cyDisFin'][:]

w = f['weights'][:]

r = f['finalDistance'][:][0]

tag = "sentence: "
for p in f.keys():
    if(~p.find(tag)):
        recall = p.split(tag)[1]
f.close()

print(recall)

n = int(np.sqrt(w.size))
w = w.reshape(n,n)
print("weights: ")
print(w)

F = pl.figure()
f = F.add_subplot(311)
f.plot(t,e)
f.set_xlabel('trials')
f.set_ylabel('error')

f = F.add_subplot(312)
f.plot(t,s)
f.set_xlabel('trials')
f.set_ylabel('steps before cycle')

f = F.add_subplot(313)
f.plot(t,c)
f.set_xlabel('trials')
f.set_ylabel('cycle length')


'''
F = pl.figure()
f = F.add_subplot(111)
f.pcolor(w)
f.set_title('weights')
f.set_xlabel('node i')
f.set_ylabel('node j')
'''

pl.show()

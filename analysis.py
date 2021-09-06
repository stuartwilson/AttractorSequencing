import h5py
import numpy as np
import pylab as pl

f = h5py.File('logs/data.h5')
t = f['generations'][:]
x = f['magnitude'][:]
w = f['weights'][:]
f.close()

n = int(np.sqrt(w.size))
w = w.reshape(n,n)
print("weights: ")
print(w)

F = pl.figure()
f = F.add_subplot(121)
f.plot(t,x)
f.set_title('evolution')
f.set_xlabel('gens')
f.set_ylabel('error')

f = F.add_subplot(122)
f.pcolor(w)
f.set_title('weights')
f.set_xlabel('node i')
f.set_ylabel('node j')

pl.show()

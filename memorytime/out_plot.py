import scipy as sp
import matplotlib.pyplot as plt

out = sp.loadtxt('out_10000000')

ind = sorted(list(set(out[:, 0])))

#styles = ['b', ]
for i in range(len(ind)):
	data = out[out[:, 0] == ind[i], 1:3]
	plt.plot(data[:, 0], data[:, 1], marker=i, label='Strides of ' + str(ind[i]) + ' bytes')

plt.xlabel('$log_2$ of Array Size')
plt.ylabel('Cycles')
plt.legend(loc=2)
plt.axvline(15,color='r')
plt.axvline(18,color='r')
plt.axvline(21.5,color='r')
plt.show()


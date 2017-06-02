from collections import deque
from math import sqrt

W = [ 1,  1,  1,  1,  1,  3,  6, 11, 13, 14, 14, 15, 15, 14, 14, 13, 11,  6,  7,  6,  3,  1,  1,  1,  1]
G = [ 0,  1,  2,  3,  4,  7, 11, 17, 20, 22, 23, 25, 26, 26, 27, 27, 26, 22, 24, 24, 22, 21, 22, 23, 24]
M = [ 1,  1,  1,  1,  1,  2,  5, 10, 12, 13, 13, 14, 14, 13, 13, 12, 10,  5,  6,  5,  2,  0,  0,  0,  0]

def avg(x=0, idx='W'):
	# values = tmp_values[idx]
	# values.append(x)
	s = 0
	for x in tmp_values[idx]:
		s += x
	res = s/len(tmp_values[idx])
	
	return res

def shift(**kwargs):
	global tmp_values
	for idx in tmp_values.keys():
		if len(tmp_values[idx]) > 5:
			# tmp_values[idx].popleft()
			tmp_values[idx] = tmp_values[idx][1:]
		tmp_values[idx].append(kwargs[idx])

tmp_values = {
	'W': [],
	'G': [],
	'M': [],
}

for t in range(len(W)):
	w = W[t]
	g = G[t]
	m = M[t]

	shift(W=w, G=g, M=m)
	w_ = avg(w, 'W')
	g_ = avg(g, 'G')
	m_ = avg(m, 'M')

	W1,W2,G1,G2,M1,M2 = [0]*6

	# TODO: 1: if sqrt(0)? 2: correlation of 3 values? 3: if correlationv > 1?
	for i in range(len(tmp_values['W'])):
		W1+=(tmp_values['W'][i] - w_)
		G1+=(tmp_values['G'][i] - g_)
		M1+=(tmp_values['M'][i] - m_)
		W2+=((tmp_values['W'][i] - w_)**2)
		G2+=((tmp_values['G'][i] - g_)**2)
		M2+=((tmp_values['M'][i] - m_)**2)
	try:
		r1 = W1*G1/sqrt(W2*G2)
	except ZeroDivisionError:
		r1 = 'NaN'
	try:
		r2 = W1*M1/sqrt(W2*M2)
	except ZeroDivisionError:
		r2 = 'NaN'
	try:
		r3 = G1*M1/sqrt(G2*M2)
	except ZeroDivisionError:
		r3 = 'NaN'
	
	print('W and G: %s\t W and M: %s\t G and M: %s'%(r1, r2, r3))


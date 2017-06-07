from collections import deque
from math import sqrt

COUNT = 10
EDGE = 0.8

I = [ 1,  1,  1,  1,  1,  3,  8, 12, 14, 15, 14, 15, 16, 15, 13, 13, 11,  6,  7,  6,  3,  1,  1,  1,  1]#
W = [ 1,  1,  1,  1,  1,  3,  6, 11, 13, 14, 14, 15, 15, 14, 14, 13, 11,  6,  7,  6,  3,  1,  1,  1,  1]
WS = 0
WA = 0
G = [ 0,  1,  2,  3,  4,  5, 11, 17, 20, 22, 23, 25, 26, 26, 27, 27, 26, 22, 24, 24, 22, 21, 22, 23, 24]
GS = 0
GA = 0
M = [ 1,  1,  1,  1,  1,  2,  5, 10, 12, 13, 13, 14, 14, 13, 13, 12, 10,  5,  6,  5,  2,  0,  0,  0,  0]
MS = 0
MA = 0

tmp_values = {
	'W': [],
	'G': [],
	'M': []
}

for t in range(len(W)):
	w = W[t]
	g = G[t]
	m = M[t]

	WS += w - WA
	WA = WS / min(t+1, COUNT)
	GS += g - GA
	GA = GS / min(t+1, COUNT)
	MS += m - MA
	MA = MS / min(t+1, COUNT)

	W1,W2,G1,G2,M1,M2 = [0]*6

	tmp_values['W'] = ([w] + tmp_values['W'])[:COUNT]
	tmp_values['G'] = ([g] + tmp_values['G'])[:COUNT]
	tmp_values['M'] = ([m] + tmp_values['M'])[:COUNT]

	# TODO: 1: if sqrt(0)? 2: correlation of 3 values? 3: if correlationv > 1?
	for i in range(len(tmp_values['W'])):
		W1+=(tmp_values['W'][i] - WA)*(tmp_values['G'][i] - GA)
		M1+=(tmp_values['G'][i] - GA)*(tmp_values['M'][i] - MA)
		G1+=(tmp_values['M'][i] - MA)*(tmp_values['W'][i] - WA)
		W2+=((tmp_values['W'][i] - WA)**2)
		G2+=((tmp_values['G'][i] - GA)**2)
		M2+=((tmp_values['M'][i] - MA)**2)
	try:
		r1 = W1/sqrt(W2*G2)
	except ZeroDivisionError:
		r1 = 'NaN'
	try:
		r2 = G1/sqrt(W2*M2)
	except ZeroDivisionError:
		r2 = 'NaN'
	try:
		r3 = M1/sqrt(G2*M2)
	except ZeroDivisionError:
		r3 = 'NaN'

	course = 0
	if r1 > EDGE:
		course = (w + g)/2
	elif r2 > EDGE:
		course = (w + m)/2
	elif r3 > EDGE:
		course = (g + m)/2
	
	print('W and G: %s\t W and M: %s\t G and M: %s'%(r1, r2, r3))
	print('Course: %s\t' % course)


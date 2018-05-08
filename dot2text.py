
import re
from math import exp
from json import dumps
from collections import defaultdict
from random import uniform
from operator import itemgetter

# from pymystem3 import Mystem as M

r_alphabet = re.compile(u'[а-яА-Я0-9-]+|[.,:;?!]+')

def gen_lines(corpus):
    data = open(corpus)
    for line in data:
        yield line.lower()

def gen_tokens(lines):
    for line in lines:
        for token in r_alphabet.findall(line):
            yield token

def gen_trigrams(tokens):
    t0, t1 = '$', '$'
    for t2 in tokens:
        yield t0, t1, t2
        if t2 in '.!?':
            yield t1, t2, '$'
            yield t2, '$','$'
            t0, t1 = '$', '$'
        else:
            t0, t1 = t1, t2

def train(text):
	lines = gen_lines(text)
	tokens = gen_tokens(lines)
	trigrams = gen_trigrams(tokens)

	bi, tri = defaultdict(lambda: 0.0), defaultdict(lambda: 0.0)
	
	for t0, t1, t2 in trigrams:
		bi[t0, t1] += 1
		tri[t0, t1, t2] += 1

	model = {}
	for (t0, t1, t2), freq in tri.items():
		if (t0, t1) in model:
			model[t0, t1].append((t2, freq/bi[t0, t1]))
		else:
			model[t0, t1] = [(t2, freq/bi[t0, t1])]
	return model

# words=[('я', 1), ('поражают', 0.1), ('деревья', 0.1), ('впечатление', 0.1)]

def generate_sentence(words, model):
	phrase = []
	t0, t1 = '$', '$'
	while 1:
		t0, t1 = t1, uni_not_rand(words=words, seq=model[t0, t1])
		if t1 == '$': break
		phrase += [t1]
	return phrase

def uni_not_rand(words, seq):
	max_fig = max(words.items(), key=itemgetter(1))[0]
	for w, freq in seq:
		if w == max_fig:
			words.update({max_fig: words[max_fig]/2})
			return w

	return unirand(seq)

def unirand(seq):
	sum_, freq_ = 0, 0
	for item, freq in seq:
		sum_ += freq
	rnd = uniform(0, sum_)
	for token, freq in seq:
		freq_ += freq
		if rnd < freq_:
			return token

def compare(S1,S2):
	ngrams = [S1[i:i+3] for i in range(len(S1))]
	# print(ngrams)
	count = 0
	for ngram in ngrams:
		count += S2.count(ngram)

	return count/max(len(S1), len(S2))

def compare_sentence(sentence_words, words):
	def sigma(x):
		return (1/(1 + exp(-x)) - 0.5)*4

	res = 0
	for w1 in sentence_words:
		for w2 in words:
			c = compare(w1, w2)
			if c > 0.66:
				res += c
			# print(w1,w2,c, res)

	return sigma(res/max(len(words), len(sentence_words)))

if __name__=='__main__':

	import pickle
	try:
		model = pickle.load(open('model', 'rb'))
	except FileNotFoundError:
		model = train('uot')
		pickle.dump(model, open('model', 'wb'))

	# m = M()

	words_of_words_initial = [ x for x in open('hands_desc.txt').readlines() ]
	words_of_words = []
	for w in words_of_words_initial:
		words_with_weight = {}
		w_splitted = [ x for x in re.split('[; ]', w) if x ]
		print(w_splitted)
		for w1 in w_splitted:
			words_with_weight.update({w1.strip(): 1})
		words_of_words.append(words_with_weight)
	# from pprint import pprint

	# pprint(words_of_words)
	# exit(0)
	# words_of_words = [{'робототехника': 1, 'робот': 1, 'деревья': 1, 'избенка': 1}]
	# print(unirand(model['$', '$']))
	for words in words_of_words:
		sent = generate_sentence(words, model)
		cnt = 0
		while compare_sentence(sent, list(words.keys())) < 0.5 and cnt < 1000:
			sent = generate_sentence(words, model)
			cnt += 1
			# if sent == ['.']:
				# print(compare_sentence(sent, list(words.keys())))
				# exit(0)
			
		print(sent, words)	


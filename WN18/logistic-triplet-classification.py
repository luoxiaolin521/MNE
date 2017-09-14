#coding:utf-8
# from numpy import *
import numpy as np
from sklearn.cross_validation import train_test_split

# import the LogisticRegression
from sklearn.linear_model import LogisticRegression 
def read_vec(filename,dic):
	f = open(filename)
	index=0
	for cc in f:
		ss=cc.strip().split('\t')
		if str(index) not in dic:
			dic[str(index)] = []
			for i in range(100):
				dic[str(index)].append(float(ss[i]))
		index+=1
	return dic

def load_train(entity_vec,relation_vec):
	f_i = open("train_link.txt",'r')
	target=[]
	samples = []
	for cc in f_i:
		ss = cc.strip().split('\t')
		target.append(ss[0])
		list1=[]
		if ss[1] in entity_vec.keys():
			list1 += entity_vec[ss[1]]
		if ss[2] in entity_vec.keys():
			list1 += entity_vec[ss[2]]
		if ss[3] in relation_vec.keys():
			list1 += relation_vec[ss[3]]
		# else:
			# print("benlinlin")
		samples.append(list1)
	# m = len(samples)
	# n = len(samples[0])
	# smp = np.array([ [samples[x][y] for y in range(n)] for x in range(m)])

	# length = len(target)
	# tar = np.array(target)
	# print("c",len(tar))
	# return smp,tar
	return samples,target


def LR(samples,target):
	f=open("result2.txt",'w')
	ratio=0.1
	while(ratio<=0.9):


		# samples,target = load_train()
		# # ss
		# target = load_target()
		print("ratio=",ratio)
		print("a",len(samples))
		print("b",len(target))
		bound = int(len(samples) * ratio)
		X_train = samples[:bound]
		X_test_1 = samples[bound:]
		y_train = target[:bound]
		y_test_1 = target[bound:]
		# m = len(X_test_1)
		# n = len(X_test_1[0])
		# X_test = np.array([ [X_test_1[x][y] for y in range(n)] for x in range(m)],dtype=np.float)

	
		# y_test = np.array(y_test_1,dtype=np.float)
		X_test = X_test_1
		y_test = y_test_1
		print("X_test",len(X_test))
		print("y_test",len(y_test))

		# X_train, X_test, y_train, y_test = train_test_split(samples, target, test_size=0.2, random_state=0)	
		# print(X_train[0])
		#classifier = LogisticRegression()  
		classifier = LogisticRegression() 
		classifier.fit(X_train,y_train)  
		print("fit success")
		x = classifier.predict(X_test) 
		print("predict success")

		# length = len(x)
		# num = 0
		TP = 0
		FP = 0
		TN = 0
		FN = 0

		length = len(x)
		num = 0
		for i in range(length):
			if int(x[i]) == 1 and int(y_test[i]) == 1:
				TP += 1
				num+=1
			elif int(x[i]) == 1 and int(y_test[i]) == -1:
				FP += 1
			elif int(x[i]) == -1 and int(y_test[i]) == 1:
				FN += 1
			elif int(x[i]) == -1 and int(y_test[i]) == -1:
				TN += 1
				num+=1
		N = TN + FP
		P = TP + FN
		Acc = float(TP+TN) / (N+P)
		Recall = float(TP) / P
		Precision = float(TP) / (TP + FP)
		Micro_F1 = float(2 * TP) / (2*TP + FN + FP)
		# Macro_F1 = 
		result=''
		result+= "Acc = "+str(Acc)+'\t'+'Recall = '+str(Recall)+'\t'+'Precision = '+str(Precision)+'\t'+'Micro_F1 = '+str(Micro_F1)+'\n'
		f.write(result)
			# for i in range(length):
		# 	if int(x[i]) == int(y_test[i]):
		# 		num += 1
		# MSE=float(num)/length
		# print("MSE = ",MSE)
		# result=''
		# result+= "ratio = "+str(ratio)+'\t'+'MSE = '+str(MSE)+'\n'
		# f.write(result)
		ratio += 0.1
	f.close()
	# return MSE
	 
	
if __name__ == '__main__':
	#ratio = 0.8
	entity_vec = {}
	relation_vec = {}
	entity_file = input("input entity file : ")
	relation_file = input("input relation file : ")
	entity_vec = read_vec("result2/"+entity_file,entity_vec)
	relation_vec = read_vec("result2/"+relation_file,relation_vec)
	print("dddd",len(entity_vec))
	samples,target = load_train(entity_vec,relation_vec)
	print(type(samples))
	print(type(target))

	LR(samples,target)
	#print("MSE = ",MSE)

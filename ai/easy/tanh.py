import math

def sigmoid(x):
    return 1 / (1 + math.exp(-x))

def tanh(x):
	return (math.exp(x) - math.exp(-x)) / (math.exp(x) + math.exp(-x))

#a = sigmoid(0.5)
a = tanh(0.32)
print(a)
print(1-a*a)
print(-0.413 * (1-a*a))


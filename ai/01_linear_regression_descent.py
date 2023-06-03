import numpy as np

# y : 1*m  x : n*m  w: n*1

class LinearRegression():
	def __init__(self):
		self.learning_rate = 0.0001
		self.total_iteration = 10000
	
	def get_yhat(self, X, w): # yhat = w1*x1 + w2*x2 + ...
		return np.dot(w.T, X)

	def loss(self, yhat, y):  
		l =  np.sum((yhat - y) * (yhat - y)) / self.m # np.power(..., 2)
		return l

	def gradient_descent(self, w, X, y, yhat): # d(yhat-y)^2)/m = (2(yhat-y)/m)
		# (1*m) (n*m) => (n*1)
		dL_to_dW = 2 * np.dot(X, (yhat - y).T) / self.m
		w = w - self.learning_rate * dL_to_dW
		return w

	def main(self, X, y):
		x1 = np.ones((1, X.shape[1]))
		X = np.append(X, x1, axis=0)

		self.m = X.shape[1]
		self.n = X.shape[0]

		w = np.zeros((self.n, 1))

		for it in range (self.total_iteration):
			yhat = self.get_yhat(X, w)
			loss = self.loss(yhat, y)

			if (it + 1) % (self.total_iteration / 10) == 0:
				print(f'lost at iteration {it} is {loss}')

			w = self.gradient_descent(w, X, y, yhat)

		return w

if __name__ == '__main__':
	X = np.random.rand(1, 500)
	y = 3 * X + np.random.rand(1, 500) * 0.1 # noise
	regression = LinearRegression()
	w = regression.main(X, y)


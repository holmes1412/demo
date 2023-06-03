# 01. 线性回归简单实现

假设n个数据，拆m个维度的feature，我们构造X是个m*n的矩阵表示每个提取的feature的内容，结果suppose是y（n列），W是每类feature的参数所以W的shape为m*1

现在我们使用最基本的均方误差，也就是L2损失函数，MSE(mean squared error)，作为预测值yhat与y的损失函数。

```
MSE = 1/m * sum((yhat - y)^2)
```

它对W求导是：

```
dL/dW = 2/m * sum(yhat - y)
```

每次用求导后的值成乘以学习率，更新到新的W上。

直到若干次迭代完成。

demo中如果学习率比较低，比如0.0001，那么就会慢慢收敛：

```
simple way with L2 mean squared error:
lost at iteration 999 is 2.1052119943998724
lost at iteration 1999 is 1.4246797645211182
lost at iteration 2999 is 1.0121155502276096
lost at iteration 3999 is 0.7604868975929371
lost at iteration 4999 is 0.6055519725643174
lost at iteration 5999 is 0.508751241470756
lost at iteration 6999 is 0.44693933532751867
lost at iteration 7999 is 0.40622228077701467
lost at iteration 8999 is 0.3782587050744088
lost at iteration 9999 is 0.35803987809289445
```
最终算出来模型的参数为：
```
[[0.92964139]
 [1.04622105]]
```

# 02.正规方程

同样是求解W，现在我们y=X*W使用求解线性回归参数的方法。

我们想要最小化损失函数，个人感觉均方误差就是最小二乘法（至少形式一样），ChatGPT说前者用于最小化损失函数，后者是一种衡量模型预测精度的指标。

Anyway，我们可以用最小二乘法的闭式解求解最小二乘法。

所以根据以下数学公式，我们直接算W是啥：

```
W = (X^T X)^(-1) X^T y
```

-1表示矩阵的逆，虽然不太记得什么意思，感觉X*W = y，那么W=y/X。这个意思，矩阵本质就是来解多项式方程的。

其中用到了numpy的一个函数：`linalg.pinv`，用于求矩阵的伪逆，也就是参数为X，解为y的方程组X*w=y的解w。

```
X = np.array([[1, 2], [3, 4], [5, 6]])
y = np.array([1, 2, 3])
w = np.linalg.pinv(X) @ y # [0.0, 0.5]
```

可以直观感受到，`LinearRegressionNormalEquation`用一个函数的解，就比`LinearRegression`迭代快且简单。

```
normal equation:
[[0.05387492]
 [2.99690983]]
```


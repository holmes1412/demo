# 两层全连接网络的简单要素

cs231简单的例子：out = x * w + b, 然后看out和y的loss

```
jupyter notebook layers.ipynb
```

## 01. 前向求out

notebook里设定x为两个batch，每个batch都是shape为(4,5,6)的vector，所以要对两次batch都求out。

需要做一个转换：把(2, 4, 5, 6)展开成(2, 120)

这个cache的用法还挺神奇的，但是这个例子里并没有cache到有用的东西，比如转换后的x，否则notebook里维度对不上。

## 02. 反向

根据back propagation那章的介绍：

```
dx = dout * w.T # (10, 5) * (5, 6) -> (10, 5)
dw = x.T * dout # (6, 10) * (10, 5) -> (6, 5)
```

需要转置是因为要让矩阵大小可以对得上。

b是一个偏置项，它被加到了w*x的每一行上。因此，计算db时，我们需要将dout的每一列求和：

```
db = np.sum(dout, axis=0) # axis=0按列；axis=1按行；
```

## 03. relu的计算

relu：

```
out = x > 0 ? x : 0;
```

所以求导的话就是:

```
d = x > 0 ? 1 : 0;
```

## 0.4 svm的计算

svm以hinge loss为损失函数：

```
L = 1/N ∑i ∑j≠yi max(0, sj - syi + Δ)
```

N个样本，sj是第i类的得分，syi是它实际分类的得分。增加delta表示我们希望正确得分至少比错误得分高多少，有这么大的差距的话就不用算惩罚，取0；否则就要算惩罚。

最后把所有错误分类且区别不大的那些都加起来，再除以N，作为损失。

这里的矩阵操作可以mark一下：

```
np.maximum(0, x - correct_class_scores[:, np.newaxis] + 1.0)
```

所以求导的话就是：

```
d = margin > 0 ? 1 : 0;
```

先计算与所有其他类别的得分之差的梯度，然后对这些梯度求和：
```
np.sum(dx, axis=1)
```

然后更新正确类别的梯度，为它与所有其他类别的得分之差的梯度之和的负数:

```
dx[np.arange(N), y] -= np.sum(dx, axis=1)
```

补充说明：np.arange(N)生成一个从0到N-1的整数序列，其中N是样本的数量。y是一个数组，包含了每个样本的正确类别的索引。所以，dx[np.arange(N), y]选择了每个样本的正确类别对应的梯度。

## 0.5 softmax的计算

softmax 以交叉熵作为损失函数：

```
L = -1/N ∑i log(e^fyi / ∑j e^fj)
```

为了避免数值不稳定性，我们还可以在计算softmax概率时减去了每个样本的最大得分：
```
probs = np.exp(x) -> probs = np.exp(x - np.max(x, axis=1, keepdims=True))
```

然后做归一化，使得每行（也就是每个样本）的所有分类的概率加起来等于1。具体做法是：按行加起来的到sum，再把每个样本的每个分类都除以这个样本刚才的sum👇
```
sum_probs = np.sum(probs, axis=1, keepdims=True)
probs = probs / sum_probs

```
keepdims=True表示保持输出的维度，使得可以进行广播操作。

为了避免计算log(0)，我们在计算损失时加上了一个很小的常数:
```
loss = -np.mean(np.log(probs[np.arange(N), y] + 1e-12))
```

指数的导数也是它自己，所以:

对于正确类别，其dx是softmax概率减1；对于其他类别，其dx就是softmax概率。最后，函数将dx除以样本数量N，得到平均的梯度。

比如下面这一行：
```
dx[np.arange(N), y] -= 1
```

np.arange(N)生成一个从0到N-1的整数序列，y是一个形状为(N,)的数组，表示每个样本的正确类别。所以，dx[np.arange(N), y]选择了每个样本的正确类别对应的梯度。

dx[np.arange(N), y] -= 1将每个样本的正确类别对应的梯度减1。这是因为在计算交叉熵损失函数的梯度时，正确类别的梯度是softmax概率减1。

这里y是一个数组，它包含了每个样本的正确类别的索引。例如，如果我们有3个样本，它们的正确类别分别是类别2，类别0和类别1，那么y可能就是一个这样的数组：[2, 0, 1]。

np.arange(N)生成一个从0到N-1的整数序列，其中N是样本的数量。如果我们有3个样本，那么np.arange(N)就会生成一个这样的数组：[0, 1, 2]。

当我们使用dx[np.arange(N), y]时，我们实际上是在对dx进行索引。对于第i个样本（由np.arange(N)中的第i个元素给出），我们选择其正确类别对应的梯度（由y中的第i个元素给出）。所以，dx[np.arange(N), y]选择了每个样本的正确类别对应的梯度。

然后，dx[np.arange(N), y] -= 1将每个样本的正确类别对应的梯度减1。这是因为在计算交叉熵损失函数的梯度时，正确类别的梯度是softmax概率减1。

所以，这行代码的意思是，对于每个样本，将其正确类别对应的梯度减1。



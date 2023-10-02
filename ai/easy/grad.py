#import sys
import torch

#print(sys.path)

x = torch.arange(4.0)#, requires_grad_=True)
x.requires_grad_(True)
print(x.grad)
print()

### dot
y = 2 * torch.dot(x, x)
print(y)

y.backward()
print(x.grad)
print()

### sum
x.grad.zero_() # if not clear, will accumulate
y = x.sum()
y.backward()
print(x.grad)
print()

### x^2
x.grad.zero_()
y = x * x
y.sum().backward() # y.backward(torch.ones(len(x)))
print(x.grad)
print()

### detach u as const, not func to x
x.grad.zero_()
y = x * x
u = y.detach()
z = u * x # so u is the const for z
z.sum().backward()
#print(x.grad)
#print(u)
print(x.grad == u)
print()

###
x.grad.zero_()
y.sum().backward()
print(x.grad == 2 * u)
print()

### control branches
def f(a):
	b = a * 2
	while b.norm() < 1000:
		b = b * 2
	if b.sum() > 0:
		c = b
	else:
		c = 100 * b
	return c;

a = torch.randn(size=(), requires_grad=True)
d = f(a)
d.backward()
print(a.grad)
print(a.grad == d / a)


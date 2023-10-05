from builtins import range
import numpy as np

def affine_forward(x, w, b):
    """
    Computes the forward pass for an affine (fully-connected) layer.

    The input x has shape (N, d_1, ..., d_k) and contains a minibatch of N
    examples, where each example x[i] has shape (d_1, ..., d_k). We will
    reshape each input into a vector of dimension D = d_1 * ... * d_k, and
    then transform it to an output vector of dimension M.

    Inputs:
    - x: A numpy array containing input data, of shape (N, d_1, ..., d_k)
    - w: A numpy array of weights, of shape (D, M)
    - b: A numpy array of biases, of shape (M,)

    Returns a tuple of:
    - out: output, of shape (N, M)
    - cache: (x, w, b)
    """
    out = None
#    print(x[0].shape) # x.shape = (2, 4, 5, 6)
#    print(x[1].shape) # x[0].shape = x[1].shape = (4, 5, 6)
#    print(w.shape)    # w.shape = (3, 120)
#    print(b.shape)    # b.shape = (3, )

    product = np.prod(x.shape[1:])    # x[1]*x[2]*...*x[n-1] = 120
    new_shape = (x.shape[0], product) # (2, 4, 5, 6) -> (2, 120)
    new_x = x.reshape(new_shape)
    
    out = np.dot(new_x, w) + b        # dot() can calc x[0] and x[1] each

    cache = (x, w, b)
    return out, cache


def affine_backward(dout, cache):
    """
    Computes the backward pass for an affine layer.

    Inputs:
    - dout: Upstream derivative, of shape (N, M)
    - cache: Tuple of:
      - x: Input data, of shape (N, d_1, ... d_k)
      - w: Weights, of shape (D, M)
      - b: Biases, of shape (M,)

    Returns a tuple of:
    - dx: Gradient with respect to x, of shape (N, d1, ..., d_k)
    - dw: Gradient with respect to w, of shape (D, M)
    - db: Gradient with respect to b, of shape (M,)
    """
    x, w, b = cache
    dx, dw, db = None, None, None

    product = np.prod(x.shape[1:])
    new_shape = (x.shape[0], product)
    new_x = x.reshape(new_shape)

#    dout = np.random.randn(*out.shape)
#    out = x.dot(w)
#    print(x.shape)
#    print(dout.shape)

    # refer to back propagation
    dw = np.dot(new_x.T, dout)    # NOT dout.dot(x.T)
    dx = np.dot(dout, w.T)    # NOT  w.T.dot(dout)
    db = np.sum(dout, axis=0) # sum each column

    # remember to reshape
    dx = dx.reshape(*x.shape) # (10, 6) -> (10, 2, 3,)
    dw = dw.reshape(*w.shape) # (6, 5) -> (2, 3, 5)

    return dx, dw, db


def relu_forward(x):
    """
    Computes the forward pass for a layer of rectified linear units (ReLUs).

    Input:
    - x: Inputs, of any shape

    Returns a tuple of:
    - out: Output, of the same shape as x
    - cache: x
    """
    out = None

    out = np.where(x > 0, x, 0)

    cache = x
    return out, cache


def relu_backward(dout, cache):
    """
    Computes the backward pass for a layer of rectified linear units (ReLUs).

    Input:
    - dout: Upstream derivatives, of any shape
    - cache: Input x, of same shape as dout

    Returns:
    - dx: Gradient with respect to x
    """
    dx, x = None, cache

    dx = np.where(x > 0, dout, 0)

    return dx

def svm_loss(x, y):
    """
    Computes the loss and gradient using for multiclass SVM classification.

    Inputs:
    - x: Input data, of shape (N, C) where x[i, j] is the score for the jth
      class for the ith input.
    - y: Vector of labels, of shape (N,) where y[i] is the label for x[i] and
      0 <= y[i] < C

    Returns a tuple of:
    - loss: Scalar giving the loss
    - dx: Gradient of the loss with respect to x
    """
    loss, dx = None, None

    N = x.shape[0]
    correct_class_scores = x[np.arange(N), y]
    margins = np.maximum(0, x - correct_class_scores[:, np.newaxis] + 1.0)  # Δ=1
    margins[np.arange(N), y] = 0  # do not consider correct class in loss
    loss = np.sum(margins) / N
    dx = np.zeros_like(x)
    dx[margins > 0] = 1
    dx[np.arange(N), y] -= np.sum(dx, axis=1)
    dx /= N

    return loss, dx


def softmax_loss(x, y):
    """
    Computes the loss and gradient for softmax classification.

    Inputs:
    - x: Input data, of shape (N, C) where x[i, j] is the score for the jth
      class for the ith input.
    - y: Vector of labels, of shape (N,) where y[i] is the label for x[i] and
      0 <= y[i] < C

    Returns a tuple of:
    - loss: Scalar giving the loss
    - dx: Gradient of the loss with respect to x
    """
    loss, dx = None, None

    N = x.shape[0]

    # remove the max one, to avoid numerical instability
    # probs = np.exp(x - np.max(x, axis=1, keepdims=True))
    probs = np.exp(x)
#    print(x.shape)
#    print(probs.shape)

    #### first normalization
    # accumulate along with each row
    sum_probs = np.sum(probs, axis=1, keepdims=True)
    # must keepdimes, or will get: operands could not be broadcast together with shapes (50,10) (50,)
    probs = probs / sum_probs

    loss = -np.mean(np.log(probs[np.arange(N), y] + 1e-12))  # avoid log(0)
    dx = probs.copy() # for those incorrect yhat[i], dx == softmax(x[i])
    dx[np.arange(N), y] -= 1 # index dx : if np.arange(N) == y ? dx[i]-=1
    dx /= N

    return loss, dx

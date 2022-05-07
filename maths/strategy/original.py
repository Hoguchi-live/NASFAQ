#import numpy as np
#import itertools
#import json
#import math
#import collections

import matplotlib
import matplotlib.pyplot as plt

from scipy.optimize import minimize

from gekko import GEKKO

"""
    Global Parameters
    S: Stock price at adjustment
    T: Tax baseline
    a: Slope
    M: Number of cycles (maximum of 24*6 = 144)
"""
S = 11360
T = 0.045
a = 20
M = 144


"""
    Function to minimize
    This one is for python
"""
def to_optimize_(x, m):
    global S, T, M
    res = 0
    for i in range(m): #Ineffective when plotting (O(m^2))
        res += x[i]*( a*(M - (i+1)) - x[i]*T*(S + a*i))
    return res # We minimize the opposite function

"""
    Gekko function.
    Function to minimize
"""
def to_optimize(x, m):
    global S, T
    res = 0
    for i in range(M):
        res += x[i]*( a*(M - (i+1)) - x[i]*T*(S + a*i))
    return -res # We minimize the opposite function

"""
    GEKKO optimizer
"""
def optimize():
    global L, T

    m = GEKKO(remote=False)
    x = m.Array(m.Var, M, lb=0, ub=10, integer=True) # Lower bound is 0: no order, upper bound is 10: hyper-multicoining

    m.options.SOLVER=1
    m.options.IMODE = 3
    m.options.COLDSTART=1
    m.solver_options = ['maximum_iterations 10000']
    m.Minimize(to_optimize(x, m))
    m.solve(disp=True)
    return x


#def save_result(res):
#
#    print(res)
#    d = dict()
#    for i in range(len(L)):
#        d[L[i]] = round(int(res[i].value[0]))
#
#
#    with open(PATH_OUT, "w") as f:
#        json.dump(d, f)

def display_strategy(x):
    s = {}
    m = -1
    index = 0
    toprint = ""
    for i in range(len(x)):
        if x[i] != m:
            toprint = "[Change@" + str(i) + "\t:x" + str(int(x[i])) + "]"
            m = x[i]
            s[i] = x[i]
            print(toprint)
    return s

def plot_results(x, s):
    fig, ax = plt.subplots()

    M = len(x)
    X = [i for i in range(M)]

    # Naive Strategies to plot
    n_strats = [i for i in range(1, 5)]

    # Plot the straight strategies
    for n in n_strats:
        xx = [n for _ in range(M)]
        Y = [to_optimize_(xx, m) for m in X]
        ax.scatter(X, Y, marker  = "+", label = "Constant x{}".format(n))

    # Plot optimized strategy
    Y = [to_optimize_(x, m) for m in X]
    ax.scatter(X, Y, marker  = "^", label = "Optimized")

    for cycle, n in s.items():
        ax.annotate("x{}".format(int(n)), textcoords = "offset pixels", xytext = (0, 10), xy = (X[cycle], Y[cycle]))

    # Display parameters
    textstr = '\n'.join((
        r'Coin price at adjustment: {}'.format(S),
        r'Tax baseline: {}'.format(T),
        r'Slope extrapolation: {}'.format(a),
        r'Number of cycles: {}'.format(M)))
    props = dict(boxstyle='round', facecolor='white', alpha=0.5)

    # place a text box in upper left in axes coords
    ax.text(0.12, 0.98, textstr, transform=ax.transAxes, fontsize=14,
            verticalalignment='top', bbox=props)

    #ax.set_yscale("log")
    #ax.set_xscale("log")
    ax.legend(loc="upper left")
    ax.set_xlabel("Cycle")
    ax.set_ylabel("Profit")
    ax.set_title("Profit using naive and optimized strategies")
    ax.grid(True)
    plt.show()


def main():
    #x = [3 for _ in range(M)]
    #res = to_optimize(x, 0)
    #print(res)
    res = optimize()
    res_ = [x.value[0] for x in res]
    print("Net profit with strategy x: ", to_optimize_(res_, M))
    print("Strategy vector:")
    s = display_strategy(res_)
    plot_results(res_, s)

if __name__ == "__main__":
    main()

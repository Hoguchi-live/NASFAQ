#import numpy as np
#import itertools
#import json
#import math
#import collections

from common import *

import matplotlib
import matplotlib.pyplot as plt

from scipy.optimize import minimize

from gekko import GEKKO
"""
    Gekko function.
    Function to minimize
"""
def to_optimize(x, S, a, nb_cycles, M, m):
    local_gain = 0

    t = m.if2(a, -1, 1)             # type of trade
    tax = m.if2(a, 0.035, 0.045)    # type of tax

    for i in range(1, M):
        mult = m.if2( x[0] - t*i, 0, x[0] - t*i)
        delta = x[i+1] - x[i]
        local_gain += t*mult*delta*( (S+a*nb_cycles) - (S+a*(x[i] + x[i+1] + 1)/2)*(1+t*tax*mult))

    return -local_gain # We minimize the opposite function

"""
    GEKKO optimizer
"""
def optimize(wma_coins, coin_prices, nb_cycles, M):

    nb_rows, nb_cols = len(wma_coins.keys()), M

    # Transform dict to lists
    coin_prices_list = [coin_prices[coin]["price"] for coin in coin_prices.keys()]
    wma_coins_list = [wma_coins[coin] for coin in coin_prices.keys()]

    m = GEKKO(remote=False)
    x = m.Array(m.Var, (nb_rows, nb_cols + 1), lb=0, ub=144, integer=True) # Lower bound is 0: no order, upper bound is 10: hyper-multicoining

    # Set multicoin bounds
    for i in range(nb_rows):
        x[i][0].value = 0
        x[i][0].lower = -10
        x[i][0].upper = 10

    # constraints on cycles
    for i in range(nb_rows):
        for j in range(1, nb_cols):
            m.Equation(x[i][j] <= x[i][j+1])

    # EQUATIONS
    for i in range(nb_rows):
        m.Obj(to_optimize(x[i], coin_prices_list[i], wma_coins_list[i], nb_cycles, M, m))


    m.options.SOLVER=3
    m.options.IMODE = 3
    m.options.COLDSTART=1
    #m.solver_options = ['maximum_iterations 10000']

    #m.Minimize(to_optimize(x, nb_rows, nb_cols, coin_prices_list, wma_coins_list, nb_cycles, m))
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

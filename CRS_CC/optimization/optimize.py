import numpy as np
import itertools
import json
import math
import collections

from scipy.optimize import minimize

from gekko import GEKKO

PATH_IN = "files/timings.json"
PATH_OUT = "files/optimized.json"

def load_data():
    l_list, timings_list = [], []
    with open(PATH_IN, "r") as f:
        data = json.load(f)
        for key, value in data.items():
            l_list.append(key)
            timings_list.append(value)
    return l_list, timings_list

L, T = load_data()


"""
    Function to minimize: worst time case for generating the key
    This one is for python
"""
def to_optimize_(x):
    global L, T
    res = 0
    for i in range(len(L)): res += x[i] * T[i]

    return res

"""
    Gekko function.
    Function to minimize: worst time case for generating the key
"""
def to_optimize(x, m):
    global L, T
    res = 0
    for i in range(len(L)): res += x[i] * T[i]

    return res

"""
    Key-space size from GEKKO
"""
def keySpace_size(x, m):
    res = 0
    for i in range(len(x)): res += m.log(2*x[i]+1) / m.log(2)

    return res

"""
    Key-space size from python
"""
def keySpace_size_(x):
    res = 0
    for i in range(len(x)): res += math.log(2*x[i]+1) / math.log(2)

    return res

"""
    GEKKO optimizer
"""
def optimize():
    global L, T

    m = GEKKO(remote=False)
    x = m.Array(m.Var,len(L), lb=1, ub=1000, integer=False)

    for i in range(len(L)):
        f = T[i]
        if 5 <= f <= 50:
            x[i].value = 1
            x[i].lower = 0
            x[i].upper = 10
        if 0.5 <= f <= 5:
            x[i].value = 10
            x[i].lower = 0
            x[i].upper = 100
        elif 0.05 <= f and f < 0.5:
            x[i].value = 500
            x[i].lower = 0
            x[i].upper = 1000
        elif 0.005 <= f and f < 0.05:
            x[i].value = 5000
            x[i].lower = 0
            x[i].upper = 10000
        elif 0.001 <= f and f < 0.005:
            x[i].value = 10000
            x[i].lower = 0
            x[i].upper = 100000
        elif 0.0001 <= f and f < 0.001:
            x[i].value = 10000
            x[i].lower = 0
            x[i].upper = 100000

    print("Initial parameters:")
    y = [y.value.value for y in x]
    print(y)
    print("Initial time:")
    print(to_optimize_(y))
    print("Initial keySpace:")
    print(keySpace_size_(y))

    m.Equation(keySpace_size(x, m) >= 200)
    m.options.SOLVER=1
#    m.options.IMODE = 3
    m.options.COLDSTART=1
    #m.solver_options = ['minlp_maximum_iterations 10000']
    m.Minimize(to_optimize(x, m))
    m.solve(disp=True)
    return x


def save_result(res):

    print(res)
    d = dict()
    for i in range(len(L)):
        d[L[i]] = round(int(res[i].value[0]))


    with open(PATH_OUT, "w") as f:
        json.dump(d, f)

def main():
    L, T = load_data()
    res = optimize()
    save_result(res);

main()

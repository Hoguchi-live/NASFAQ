import json
import math
import matplotlib.pyplot as plt
font = {'family' : 'normal',
        'size'   : 18}

plt.rc('font', **font)

DIR = "./files/"
FILENAME = "timings.json"

EXTENSION_DEGREE = [1, 1, 1,     1, 1, 1, 1,     1, 1,
					3, 3,
					4, 4,
					5, 5, 5,
					7, 7, 7,
					8,
					9, 9]

with open(DIR+FILENAME) as f:
    timings = json.load(f)

x = [int(l) for l in timings.keys()]
y = [timings[str(l)] for l in x]

x_rad = x[0:3]
y_rad = y[0:3]

x_velu = x[3:]
y_velu = y[3:]


fig, ax = plt.subplots()

ax.scatter(x_rad, y_rad, marker = "2", label = "Radical isogeny")
ax.scatter(x_velu, y_velu, marker = "^", label = "Sqrt-Velu")
ax.set_yscale("log")
ax.set_xscale("log")

for i, txt in enumerate(x_rad):
    ax.annotate(" " + str(txt) + " [{}] {}".format(EXTENSION_DEGREE[:3][i], math.floor(math.log(x_rad[i])/math.log(2))), (x_rad[i], y_rad[i]))

for i, txt in enumerate(x_velu):
    ax.annotate(" " + str(txt) + " [{}] {}".format(EXTENSION_DEGREE[3:][i], math.floor(math.log(x_velu[i])/math.log(2))), (x_velu[i], y_velu[i]))

ax.legend(loc="upper left")
ax.set_xlabel("l (log)")
ax.set_ylabel("Average step time in seconds (log)")
ax.set_title("Timings for the l-primes")
ax.grid(True)
plt.show()

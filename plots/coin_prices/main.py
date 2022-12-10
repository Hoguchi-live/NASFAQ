import matplotlib.pyplot as plt
import matplotlib.dates as md

import datetime as dt
import time
import random

from common import *
from db_handle import *
from http_handle import *

if __name__ == '__main__':
    C = get_db_connection()
    cur = C.cursor()

    ## Fetch data
    days = 0
    delta = 30
    ts_high =  time.mktime(time.localtime()) * 1000 - days*24*60*60*1000
    ts_low = ts_high - delta*60*60*24*1000

    coins = ['subaru']
    userids = ['c9b0830c-8cf7-4e1d-b8f3-60bbc85160fa', '990400b5-f0e2-4b91-8aa6-5a74dc16bcc5']

    hist = fetch_history(cur, ts_low, ts_high, coins, userids)

    # Analyse data
    T0, T1, Y0, Y1 = [], [], [], []
    for tr in hist:
        if tr['userid'][:5] == userids[0][:5]:
            Y0.append(tr["quantity"])
            T0.append(tr["timestamp"]// 1000)
        else:
            Y1.append(tr["quantity"])
            T1.append(tr["timestamp"]// 1000)

    dates0 = [dt.datetime.fromtimestamp(ts) for ts in T0]
    X0 = md.date2num(dates0)
    dates1 = [dt.datetime.fromtimestamp(ts) for ts in T1]
    X1 = md.date2num(dates1)


    ax=plt.gca()
    xfmt = md.DateFormatter('%H:%M:%S')
    ax.xaxis.set_major_formatter(xfmt)

    # Plot data
    plt.plot(X1, Y1, "r+")
    plt.plot(X0, Y0, "b+")
    plt.show()

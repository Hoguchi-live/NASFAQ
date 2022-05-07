import psycopg2

from common import *

def get_db_connection():
    return psycopg2.connect(database="nasfaq", user = "steaky", host = "127.0.0.1", port = "5432")

def fetch_wma_slope_all(cursor):

    slope_dict = {}

    nb_cycles = 2
    delta = 600

    for coin in COINS:
        tmp = 0
        weight = nb_cycles
        wma = 0
        denum = 0

        query = "select PRICE, TIMESTAMP FROM last_n_spaced_prices('{}', {}, {});".format(coin, nb_cycles, delta)
        cursor.execute(query)
        rows = cursor.fetchall()

        price_top = float(rows[0][0])
        price_bot = float(rows[1][0])
        ts_top = int(rows[0][1])
        ts_bot = int(rows[1][1])

        tmp = (price_top - price_bot) / (ts_top - ts_bot)
        wma = weight * tmp
        denum = weight

        for row in rows[1::]:
            price_bot = price_top
            price_top = row[0]
            ts_bot = ts_top
            ts_top = row[1]

            weight -= 1
            tmp = (price_top - price_bot) / (ts_top - ts_bot)
            wma += weight * tmp
            denum += weight

        slope_dict[coin] = wma / denum * 1000 * 600

    return slope_dict

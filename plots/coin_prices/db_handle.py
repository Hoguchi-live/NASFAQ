import psycopg2

from common import *
import psycopg2.extras

def get_db_connection():
    return psycopg2.connect(cursor_factory=psycopg2.extras.RealDictCursor,
            database="nasfaq", user = "steaky", host = "127.0.0.1", port = "5432")

def fetch_history(cursor, ts_low, ts_high, coins = COINS, userids = None):
    if coins == None: return {}

    query = "SELECT * FROM HISTORY WHERE (TIMESTAMP > {} AND TIMESTAMP <= {}) AND (".format(ts_low, ts_high)
    # Filter by coins
    for i in range(len(coins)):
        if i == len(coins) - 1:
            query += "COIN = '{}' )".format(coins[i])
        else:
            query += "COIN = '{}' OR ".format(coins[i])

    # Filter by userids
    if userids: query += "AND ("
    for i in range(len(userids)):
        if i == len(userids) - 1:
            query += "userid = '{}' )".format(userids[i])
        else:
            query += "userid = '{}' OR ".format(userids[i])

    query += ";"
    print(query)

    cursor.execute(query)
    res = cursor.fetchall()

    return res

from common import *
from optimizer import *
from db_handle import *
from http_handle import *

if __name__ == '__main__':
    C = get_db_connection()
    cur = C.cursor()

    user_balance = fetch_balance()
    coin_qty = fetch_coin_qty_all()
    coin_prices = fetch_marketInfo_all()
    wma_coins = fetch_wma_slope_all(cur)
    #wma_coins  = {coin:20 for coin in coin_prices.keys() }

    # Cut down for test
    u = 0
    max_u = 10

    smp_prices = {}
    smp_wma = {}
    #coins = ["lamy", "towa", "luna", "marine", "subaru", "aqua", "hololive"]
    for key in coin_prices.keys():
        if u > max_u: break
        else:
            try:
                a = wma_coins[key]
                if abs(a) > 10:
                        smp_prices[key] = coin_prices[key]
                        smp_wma[key] = wma_coins[key]
                        u += 1
            except KeyError:
                pass

    print("Coins prices: ")
    print(smp_prices)
    print("Coins wma: ")
    print(smp_wma)

    M = 144
    sol = optimize_all(smp_wma, smp_prices, M)
    save_strategy_all(sol, smp_wma, smp_prices, M)

from common import *
import requests

def fetch_marketInfo_all():
    url = URL_API + "getMarketInfo?all"
    r = requests.get(url)
    return r.json()['coinInfo']['data']

def fetch_coin_qty_all():
    rop = {}

    url = URL_API + "getUserWallet?userid=" + USERID
    r = requests.get(url)

    for key, item in r.json()['wallet']['coins'].items():
        rop[key] = item['amt']

    return rop

def fetch_balance():
    url = URL_API + "getUserWallet?userid=" + USERID
    r = requests.get(url)

    return r.json()['wallet']['balance']

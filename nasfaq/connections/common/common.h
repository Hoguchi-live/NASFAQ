#ifndef _COMMON_H_
#define _COMMON_H_

enum MSG_TYPE =
	{ COIN_PRICE_UPDATE
	, HISTORY_UPDATE
	, BROKER_FEE_UPDATE
	, TODAY_PRICES_UPDATE
	}

typedef struct raw_message_t {
	enum msg_type type;
	std::string data;
} raw_message_t ;

/**
  coinPriceUpdate structure
*/
typedef struct cpu_hld_t {
	std::string coin;
	float price;
	float saleValue;
	uint inCirculation;
} cpu_hld_t

#endif


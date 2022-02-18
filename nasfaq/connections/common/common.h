#ifndef _COMMON_H_
#define _COMMON_H_

/*
   ALL INCLUDES (COMMON) SHOULD BE HERE
*/
#include <iostream>
#include <string>
#include <stdlib.h>

#include <nlohmann/json.hpp>

#include "../common/common.h"
#include "../safe_queue/safe_queue.h"


enum MSG_TYPE
	{ COIN_PRICE_UPDATE
	, HISTORY_UPDATE
	, BROKER_FEE_UPDATE
	, TODAY_PRICES_UPDATE
};

std::ostream& operator<< (std::ostream&, const MSG_TYPE&);

typedef struct raw_msg_t {
	enum MSG_TYPE type;
	std::string data;
} raw_message_t ;

/**
  Placeholder for pre-parsed outputs of the socket.
*/
template <MSG_TYPE E>
struct pparsed;

/**
  coinPriceUpdate structure
*/
template <>
struct pparsed<COIN_PRICE_UPDATE> {
	std::string coin;
	float price;
	float saleValue;
	uint inCirculation;
};

#endif


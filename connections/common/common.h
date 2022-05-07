#ifndef _COMMON_H_
#define _COMMON_H_

/*
   ALL INCLUDES (COMMON) SHOULD BE HERE
*/
#include <iostream>
#include <string>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <tuple>

#include <nlohmann/json.hpp>

//#include "../safe_queue/safe_queue.h"

/***********************************************************************************
  				WEBSOCKET MESSAGE STRUCTURES
***********************************************************************************/
/**
  Types of messages going through the websocket.
  X-macros: https://stackoverflow.com/questions/201593/is-there-a-simple-way-to-convert-c-enum-to-string
*/
enum WS_MSG {
	#define X(a) a,
	#include "ws_msg.def"
	#undef X
};

///**
//  Mapping from WS_MSG enum to its name.
//*/
//std::map<WS_MSG, std::string> WS_MSG_map {
//	#define X(a) {a, "#a"},
//	#include "ws_msg.def"
//	#undef X
//};

/**
  Structure holding websocket raw data and their type.
  Used to fill the queue uniformly
*/
typedef struct raw_msg_t {
	enum WS_MSG type;
	std::string data;
} raw_message_t ;

/**
  Placeholder for parsed outputs of the websocket.
  These structures are c++ formated and should contain the entirety of the represented data.
  Specialized below.
*/
template <WS_MSG E>
struct ws_msg_parsed;

/**
  coinPriceUpdate structure
*/
template <>
struct ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> {
	std::string coin;
	float price;
	float saleValue;
	uint inCirculation;
};

/**
  Auxiliary type for WS_EVENT_HISTORY_UPDATE.
  This holds individual transactions as handed by the websocket in a list.
*/
template <>
struct ws_msg_parsed<WS_EVENT_TRANSACTION> {
	std::string coin;
	uint type;
	std::string userid;
	int quantity;
	long timestamp;
	bool completed;
	float price;
};

/**
  historyUpdate structure holding transactions.
*/
template<>
struct ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> {
	std::vector<ws_msg_parsed<WS_EVENT_TRANSACTION>> transaction_list;
};

/***********************************************************************************
  			WEBSOCKET MESSAGE STRUCTURES FUNCTIONS
***********************************************************************************/
std::ostream& operator<< (std::ostream&, const WS_MSG&);

/***********************************************************************************
  				IN-GAME MESSAGE STRUCTURES
***********************************************************************************/
/**
  Types of in-game messages.
*/
enum IG_MSG
	{ TRANSACTION
	, IG_UNKNOWN_TYPE
};


/***********************************************************************************
  			INGAME MESSAGE STRUCTURES FUNCTIONS
***********************************************************************************/
template<WS_MSG E>
std::ostream& operator<< (std::ostream&,  ws_msg_parsed<E> const &);

/**
  Cycle representation.
*/
typedef std::map<std::string, int> cycle_t;
std::ostream& operator<< (std::ostream&, const cycle_t&);

/**
  User representation
*/
typedef struct user_t{
	std::string userid;
	std::string username;
	std::string icon;
	float networth;
} user_t;

#endif


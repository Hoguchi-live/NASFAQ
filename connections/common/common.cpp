#include "common.h"

/***********************************************************************************
  			WEBSOCKET MESSAGE STRUCTURES FUNCTIONS
***********************************************************************************/
/**
  Displays websocket message types.
*/
std::ostream& operator<< (std::ostream& stream, WS_MSG const & type) {
	std::string ret;
	switch(type) {
		#define X(a) case a: ret = #a; break;
		#include "ws_msg.def"
		#undef X
		default: ret = "Unknown type";
	}

	stream << ret;
	return stream;
}

/**
  //TODO: GOES INTO pretty_print.cpp
  Pretty prints COIN_PRICE events.
*/
template<>
std::ostream& operator<< (std::ostream& stream,  ws_msg_parsed<WS_EVENT_COIN_PRICE> const & op) {
	stream << "WS_EVENT_COIN_PRICE:"
	       << " Coin: " << op.coin
	       << " Price: " << op.price
	       << " SaleValue: " << op.saleValue
	       << " inCirculation: " << op.inCirculation;
	return stream;
}

/**
  Pretty prints WS_EVENT_TRANSACTION events.
*/
template<>
std::ostream& operator<< (std::ostream& stream,  ws_msg_parsed<WS_EVENT_TRANSACTION> const & op) {
	stream << "WS_EVENT_TRANSACTION:"
	       << " Coin: " << op.coin
	       << " Type: " << op.type
	       << " UserID: " << op.userid
	       << " Quantity: " << op.quantity
	       << " Timestamp: " << op.timestamp
	       << " Completed: " << op.completed
	       << " Price: " << op.price;
	return stream;
}

/**
  Pretty prints portfolio_coin_t.
*/
std::ostream& operator<< (std::ostream& stream, portfolio_coin_t const & op) {
	stream << "portfolio_coin_t:"
	       << " Coin: " << op.coin
	       << " Amount: " << op.amount
	       << " Timestamp: " << op.ts
	       << " MPP: " << op.mpp;
	return stream;
}

/**
  Pretty prints WS_EVENT_MF_PORTFOLIO events.
*/
template<>
std::ostream& operator<< (std::ostream& stream,  ws_msg_parsed<WS_EVENT_MF_PORTFOLIO> const & op) {
	stream << "WS_EVENT_MF_PORTFOLIO:"
	       << " Fund: " << op.fund
	       << " Event: " << op.event << std::endl
	       << "Transactions: " << std::endl;

	for(auto & element : op.transactions) {
		stream << "\t" << element << std::endl;
	}

	stream << "Portfolio: " << std::endl;

	for(auto & element : op.portfolio) {
		stream << "\t" << element << std::endl;
	}

	return stream;
}

/**
  Pretty prints the transactions held in the WS_EVENT_HISTORY vector.
*/
template<>
std::ostream& operator<< (std::ostream& stream,  ws_msg_parsed<WS_EVENT_HISTORY> const & op) {
	stream << "WS_EVENT_HISTORY:\n";
	for(auto & element : op.transaction_list) {
		stream << "\t" << element << std::endl;
	}

	return stream;
}

/***********************************************************************************
  			INGAME MESSAGE STRUCTURES FUNCTIONS
***********************************************************************************/
std::ostream& operator<< (std::ostream& stream, const cycle_t& op) {
	stream  << "CYCLE: " << std::endl;
	for(auto const& element : op) {
		stream << element.first << " : " << element.second << std::endl;
	}
	return stream;
}

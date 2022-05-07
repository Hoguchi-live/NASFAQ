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
  Pretty prints COIN_PRICE events.
*/
template<>
std::ostream& operator<< (std::ostream& stream,  ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> const & op) {
	stream << "WS_EVENT_COIN_PRICE_UPDATE:"
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
  Pretty prints the transactions held in the WS_EVENT_HISTORY_UPDATE vector.
*/
template<>
std::ostream& operator<< (std::ostream& stream,  ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> const & op) {
	stream << "WS_EVENT_HISTORY_UPDATE:\n";
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

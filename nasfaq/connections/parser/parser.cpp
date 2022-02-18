#include "parser.h"

MSG_TYPE msg_type_detect(std::string op) {
	MSG_TYPE ret;

	if (op.substr(2, 15) == "coinPriceUpdate")  {
		ret = COIN_PRICE_UPDATE;
	} else if (op.substr(2, 13) == "historyUpdate") {
		ret = HISTORY_UPDATE;
	} else if (op.substr(2, 17) == "todayPricesUpdate") {
		ret = TODAY_PRICES_UPDATE;
	} else if (op.substr(2, 15) == "brokerFeeUpdate") {
		ret = BROKER_FEE_UPDATE;
	}
	return ret;
}

template <MSG_TYPE E>
pparsed<E> parse(raw_msg_t rmsg) {};

template <>
pparsed<COIN_PRICE_UPDATE> parse<COIN_PRICE_UPDATE>(raw_msg_t rmsg) {
	std::string payload = rmsg.data;
	nlohmann::json jparsed = nlohmann::json::parse(payload.substr(21, payload.length()-21-1)); /* Check for errors and emptiness needed */

	pparsed<COIN_PRICE_UPDATE> rop;
	rop.coin = jparsed["coin"];
	rop.price = (jparsed["info"])["price"];
	rop.saleValue = (jparsed["info"])["saleValue"];
	rop.inCirculation = (jparsed["info"])["inCirculation"];

	return rop;
}


//class parser {
//public:
//	parser (SafeQueue<raw_msg_t> queue)
//		: m_queue(queue);
//	{}
//
//	void parse() {
//		std::string raw_data;
//
//		msg_type type;
//		std::string data;
//
//		while(1) {
//			raw_data = m_queue.dequeue();
//			type = msg_type_detect(raw_data);
//
//			data = m_queue.
//			sleep(0.5);
//		}
//	}
//private:
//	SafeQueue<raw_msg_t> m_queue;
//}
//

#include "common.h"

std::ostream& operator<< (std::ostream& stream, MSG_TYPE const & type) {
	std::string ret;
	switch(type) {
		case COIN_PRICE_UPDATE: ret = "COIN_PRICE_UPDATE"; break;
		case HISTORY_UPDATE: ret =  "HISTORY_UPDATE"; break;
		case BROKER_FEE_UPDATE: ret = "BROKER_FEE_UPDATE"; break;
		case TODAY_PRICES_UPDATE: ret = "TODAY_PRICES_UPDATE"; break;
		default: ret = "Unknown type: ";
	}
	stream << ret;
	return stream;
}

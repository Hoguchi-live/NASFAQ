#include "parser_aux.h"

/*******************************************************************************
	Types.
*******************************************************************************/
namespace parser::types {
/*
   Takes a string as input and returns a WS_MSG corresponding to the underlying event type.
   Returns WS_EVENT_UNKNOWN if no match is detected.
   TODO: handle malformed strings, only one ",...
*/
WS_MSG extract(std::string op) {
	WS_MSG ret;
	std::size_t pos_beg;
	std::size_t pos_end;
	std::string raw_type;

	// Find raw_type as first occurence of "RAW_TYPE" in op
	pos_beg = op.find_first_of('"');
	pos_end = op.find_first_of('"', pos_beg + 1);
	raw_type = op.substr(pos_beg+1, pos_end - pos_beg-1);


	switch(map_rawIdent[raw_type]) {

		case(0): ret = WS_EVENT_UNKNOWN; break;
		case(1): ret = WS_EVENT_COIN_PRICE; break;
		case(2): ret = WS_EVENT_HISTORY; break;
		case(3): ret = WS_EVENT_TODAY_PRICES; break;
		case(4): ret = WS_EVENT_BROKER_FEE; break;

		case(10): ret = WS_EVENT_MF_PORTFOLIO; break;
		case(11): ret = WS_EVENT_MF_BALANCE; break;
		case(12): ret = WS_EVENT_MF_RUNNINGHISTORY; break;
		case(13): ret = WS_EVENT_MF_STAT; break;
		case(14): ret = WS_EVENT_MF_MEMBERS; break;
		case(15): ret = WS_EVENT_MF_ORDER; break;
		case(16): ret = WS_EVENT_MF_MAKEPUBLIC; break;

		default: ret = WS_EVENT_UNKNOWN;
	}
	return ret;
}
} // parser::type

namespace parser::payload {
	std::string extract(std::string op) {
		std::string ret;
		std::size_t tmp;

		// Find second occurence of ". Raw payloads are of the form 42["RAW_TYPE",PAYLOAD ]
		tmp = op.find_first_of('"', 3) + 2; // Wew hopefully that doesn't break
		ret = op.substr(tmp, op.length() - tmp);

		return ret;
	}
}

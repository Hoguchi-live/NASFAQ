#ifndef _PARSER_AUX_H_
#define _PARSER_AUX_H_

#include "../common/common.h"

/*******************************************************************************
	Types
*******************************************************************************/
namespace parser::types {

/*
  Raw message identifiers and int conversion.
*/
static std::map<std::string, int> map_rawIdent = {
	{"unknown", 0},
	{"coinPriceUpdate", 1},
	{"historyUpdate", 2},
	{"todayPricespdate", 3},
	{"brokerFeeUpdate", 4},
	{"mutualFundPortfolioUpdate", 10},
	{"mutualFundBalanceUpdate", 11},
	{"mutualFundRunningHistoryUpdate", 12},
	{"mutualFundStatUpdate" , 13},
	{"mutualFundMembersUpdate" , 14},
	{"mutualFundOrderUpdate" , 15},
	{"mutualFundMakePublicUpdate" , 16}
};

/*
   Takes a string as input and returns its corresponding WS_MSG type.
*/
WS_MSG extract(std::string);

} // parser::types

namespace parser::payload {
	std::string extract(std::string);

 } // parser::payload
#endif


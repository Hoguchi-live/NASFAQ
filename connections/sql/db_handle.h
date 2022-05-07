#ifndef _DB_HANDLE_H_
#define _DB_HANDLE_H_

#include <iostream>
#include <pqxx/pqxx>
#include <fmt/core.h>
#include <algorithm>

#include "../common/common.h"


/* DB API to store and fetch data. */
namespace db {
	namespace query {
		std::string make_pull_query_last_n_spaced_prices(std::string, int, int);
	}


	/********************************************************************************
					HISTORY
	********************************************************************************/
	void push_history( pqxx::connection*, ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> );

	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> pull_last_cycle( pqxx::connection*);
	std::vector<std::string> pull_userid_list_ts( pqxx::connection *, long, long);
	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> pull_transactions_userid_ts( pqxx::connection *, std::string, long, long);
	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> pull_last_cycle_userid( pqxx::connection *, std::string);

	float pull_last_cycle_slope(pqxx::connection*, std::string);



	/********************************************************************************
					COIN PRICE
	********************************************************************************/
	void push_coin_price( pqxx::connection*, ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> );

	/********************************************************************************
					COIN PRICE
	********************************************************************************/
	std::string userid_to_username(pqxx::connection*, std::string userid);

	/********************************************************************************
					USERS
	********************************************************************************/
	void push_users( pqxx::connection*, std::vector<user_t> );
	std::string userid_to_username(pqxx::connection*, std::string userid);
}
#endif


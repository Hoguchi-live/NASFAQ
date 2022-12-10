#include "db_handle.h"

namespace db {
	void commit(pqxx::connection *C, std::string query) {
		/* Create a transactional object. */
		pqxx::work W(*C);

		/* Execute SQL query */
		W.exec( query );
		W.commit();
	}
}

namespace db::push {
	/* Add transactions contained in  history update to the database. */
	void push_history( pqxx::connection* C, ws_msg_parsed<WS_EVENT_HISTORY> op ) {
		std::string query;

		for(auto& element : op.transaction_list){
			query += query::fmt::p_query_transaction(element);
		}

		db::commit(C, query);
	}

	/* Same as push_history but checks for unique timestamp. */
	void push_uhistory( pqxx::connection* C, ws_msg_parsed<WS_EVENT_HISTORY> op ) {
		std::string query;

		for(auto& element : op.transaction_list){
			query += query::make_push_query_utransaction(element);
		}

		/* Create a transactional object. */
		pqxx::work W(*C);

		/* Execute SQL query */
		W.exec( query );
		W.commit();
	}

	/* Returns the last cycle in form of a ws_msg_parsed<WS_EVENT_HISTORY> struct. */
	ws_msg_parsed<WS_EVENT_HISTORY> pull_last_cycle(pqxx::connection* C) {
		std::string query;
		long ts_high, ts_low;
		ws_msg_parsed<WS_EVENT_TRANSACTION> tmp;
		ws_msg_parsed<WS_EVENT_HISTORY> rop;

		ts_high = time(NULL)*1000;
		ts_low  = ts_high - 600*1000;

		query = query::make_pull_query_history_ts(ts_high, ts_low);
		//std::cout << query << std::endl;

   		/* Create a non-transactional object. */
		pqxx::nontransaction N(*C);

   		/* Execute SQL query */
		pqxx::result R( N.exec( query ));

		/* Parse result. */
		for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
			tmp.coin = c[1].as<std::string>();
			tmp.type = c[2].as<int>();
			tmp.userid = c[3].as<std::string>();
			tmp.quantity = c[4].as<int>();
			tmp.timestamp = c[5].as<long>();
			tmp.completed = c[6].as<bool>();
			tmp.price = c[7].as<float>();

			rop.transaction_list.push_back(tmp);
		}

		return rop;
	}

	/* Fetches unique user ids having transactions between ts_low and ts_high. */
	std::vector<std::string> pull_userid_list_ts( pqxx::connection *C, long ts_high, long ts_low ) {
		std::string query;
		std::vector<std::string> rop;

		query = query::make_pull_query_userid_list_ts(ts_high, ts_low);

   		/* Create a non-transactional object. */
		pqxx::nontransaction N(*C);

   		/* Execute SQL query */
		pqxx::result R( N.exec( query ));

		/* Parse result. */
		for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
			rop.push_back(c[0].as<std::string>());
		}
		return rop;
	}

	/* Fetches all transactions by userid during period ts_low to ts_high. */
	ws_msg_parsed<WS_EVENT_HISTORY> pull_transactions_userid_ts( pqxx::connection *C, std::string userid, long ts_high, long ts_low) {
		std::string query;
		ws_msg_parsed<WS_EVENT_TRANSACTION> tmp;
		ws_msg_parsed<WS_EVENT_HISTORY> rop;

		query = query::make_pull_query_transactions_userid_ts(userid, ts_high, ts_low);

   		/* Create a non-transactional object. */
		pqxx::nontransaction N(*C);

   		/* Execute SQL query */
		pqxx::result R( N.exec( query ));

		/* Parse result. */
		for (pqxx::result::const_iterator c = R.begin(); c != R.end(); ++c) {
			tmp.coin = c[1].as<std::string>();
			tmp.type = c[2].as<int>();
			tmp.userid = c[3].as<std::string>();
			tmp.quantity = c[4].as<int>();
			tmp.timestamp = c[5].as<long>();
			tmp.completed = c[6].as<bool>();
			tmp.price = c[7].as<float>();

			rop.transaction_list.push_back(tmp);
		}

		return rop;
	}

	/* Fetches last cycle's transactions for userid. */
	ws_msg_parsed<WS_EVENT_HISTORY> pull_last_cycle_userid( pqxx::connection *C, std::string userid) {
		long ts_high, ts_low;
		ws_msg_parsed<WS_EVENT_HISTORY> rop;

		ts_high = time(NULL)*1000;
		ts_low  = ts_high - 60*10*1000;

		 rop = pull_transactions_userid_ts(C, userid, ts_high, ts_low);
		 return rop;
	}

	/********************************************************************************
					COIN PRICE
	********************************************************************************/
	void push_coin_price( pqxx::connection* C, ws_msg_parsed<WS_EVENT_COIN_PRICE> op) {
		std::string query;

		query = query::make_push_query_coin_price(op);

		/* Create a transactional object. */
		pqxx::work W(*C);

		/* Execute SQL query */
		W.exec( query );
		W.commit();
	}


	/********************************************************************************
					USERS
	********************************************************************************/
	// TODO: goes into common or algo
	void ReplaceAll(std::string &str, const std::string& from, const std::string& to) {
	    size_t start_pos = 0;
	    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
	        str.replace(start_pos, from.length(), to);
	        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	    }
	}
	void push_users( pqxx::connection* C, std::vector<user_t> op){
		std::string query;
		std::string tmp;

		for(auto& user : op){
			tmp = user.username;
			ReplaceAll(tmp, "'", "\\'");
			user.username = tmp;
			query += query::make_push_query_user(user);
		}

		/* Create a transactional object. */
		pqxx::work W(*C);

		/* Execute SQL query */
		W.exec( query );
		W.commit();
	}

	std::string userid_to_username(pqxx::connection* C, std::string userid){
		std::string query;
		query = query::make_pull_query_userid_to_username(userid);

   		/* Create a non-transactional object. */
		pqxx::nontransaction N(*C);

   		/* Execute SQL query */
		pqxx::result R( N.exec( query ));

		return (R.begin())[0].as<std::string>();

	}
}

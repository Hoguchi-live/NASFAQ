#include "db_handle.h"

namespace db {
	namespace query {
		/* Query templates for HISTORY. */
		//TODO: Move these definitions to .h file.
		const std::string query_template_transaction = "INSERT INTO HISTORY(COIN, TYPE, USERID, QUANTITY, TIMESTAMP, COMPLETED, PRICE) "\
							       "VALUES('{}', {}, '{}', {}, {}, {}, {});";
		const std::string query_template_history_ts = "SELECT * FROM HISTORY WHERE TIMESTAMP <= {} AND TIMESTAMP > {};";
		const std::string query_template_userid_ts = "SELECT DISTINCT USERID FROM HISTORY WHERE TIMESTAMP <= {} AND TIMESTAMP > {};";

		const std::string query_template_slope_ts_top =  "SELECT PRICE, TIMESTAMP FROM HISTORY "\
							     "WHERE COIN = '{}' AND TIMESTAMP <= {} AND TIMESTAMP > {} "\
							     "ORDER BY TIMESTAMP ASC "\
							     "LIMIT 1;";
		const std::string query_template_transactions_userid_ts = "SELECT * FROM HISTORY WHERE USERID = '{}' AND TIMESTAMP <= {} AND TIMESTAMP > {};";

		const std::string query_template_slope_ts_bot =  "SELECT PRICE, TIMESTAMP FROM HISTORY "\
							     "WHERE COIN = '{}' AND TIMESTAMP <= {} AND TIMESTAMP > {} "\
							     "ORDER BY TIMESTAMP DESC "\
							     "LIMIT 1;";

		const std::string query_template_last_n_spaced_prices = "select PRICE, TIMESTAMP FROM last_n_spaced_prices('{}', {}, {});";

		const std::string query_template_user = "INSERT INTO USERS(USERID, USERNAME, ICON, NETWORTH) "\
							"VALUES('{}', E'{}', '{}', {}) "
							"ON CONFLICT (USERID) DO UPDATE SET "\
							"NETWORTH = {};";

		const std::string query_template_userid_to_username = "select USERNAME FROM USERS WHERE USERID = '{}'";

	/********************************************************************************
					GLOBAL QUERIES
	********************************************************************************/

		std::string make_push_query_transaction( ws_msg_parsed<WS_EVENT_TRANSACTION> op ) {
			return fmt::format(query_template_transaction,
				op.coin,
				op.type,
				op.userid,
				op.quantity,
				op.timestamp,
				op.completed,
				op.price);
		}

		std::string make_pull_query_history_ts( long ts_high, long ts_low ) {
			return fmt::format(query_template_history_ts, ts_high, ts_low);
		}

		std::string make_pull_query_userid_list_ts( long ts_high, long ts_low ) {
			return fmt::format(query_template_userid_ts, ts_high, ts_low);
		}

		std::string make_pull_query_transactions_userid_ts( std::string userid, long ts_high, long ts_low ) {
			return fmt::format(query_template_transactions_userid_ts, userid, ts_high, ts_low);
		}

		std::string make_pull_query_top_price_cycle_ts( std::string coin, long ts_high, long ts_low ) {
			return fmt::format(query_template_slope_ts_top, coin, ts_high, ts_low);
		}

		std::string make_pull_query_bot_price_cycle_ts( std::string coin, long ts_high, long ts_low ) {
			return fmt::format(query_template_slope_ts_bot, coin, ts_high, ts_low);
		}


		/* Query templates for COIN PRICE. */
		const std::string query_template_coin_price = "INSERT INTO COINPRICE(COIN, PRICE, SALEVALUE, INCIRCULATION, LASTUPDATE) "\
							       "VALUES('{}', {}, {}, {}, {}) "\
							       "ON CONFLICT (COIN) DO UPDATE SET "\
							       "PRICE = {}, SALEVALUE = {}, INCIRCULATION = {}, LASTUPDATE = {};";

		std::string make_push_query_coin_price( ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> op ) {
			return fmt::format(query_template_coin_price,
				op.coin,
				op.price,
				op.saleValue,
				op.inCirculation,
				std::time(NULL)*1000,
				op.price,
				op.saleValue,
				op.inCirculation,
				std::time(NULL)*1000);
		}

		/* Query templates for math functions. */
		std::string make_pull_query_last_n_spaced_prices( std::string coin, int nb_cycles, int delta) {
			return fmt::format(query_template_last_n_spaced_prices,
					coin,
					nb_cycles,
					delta);
		}

		std::string make_push_query_user(user_t user){
			return fmt::format(query_template_user,
					user.userid,
					user.username,
					user.icon,
					user.networth,
					user.networth);
		}

		std::string make_pull_query_userid_to_username(std::string userid) {
			return fmt::format(query_template_userid_to_username,
					userid);
		}
	}

	/********************************************************************************
					HISTORY
	********************************************************************************/

	/* Add transactions contained in  history update to the database. */
	void push_history( pqxx::connection* C, ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> op ) {
		std::string query;

		for(auto& element : op.transaction_list){
			query += query::make_push_query_transaction(element);
		}

		/* Create a transactional object. */
		pqxx::work W(*C);

		/* Execute SQL query */
		W.exec( query );
		W.commit();
	}

	/* Returns the last cycle in form of a ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> struct. */
	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> pull_last_cycle(pqxx::connection* C) {
		std::string query;
		long ts_high, ts_low;
		ws_msg_parsed<WS_EVENT_TRANSACTION> tmp;
		ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> rop;

		ts_high = time(NULL)*1000;
		ts_low  = ts_high - 600*1000;

		query = query::make_pull_query_history_ts(ts_high, ts_low);

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
	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> pull_transactions_userid_ts( pqxx::connection *C, std::string userid, long ts_high, long ts_low) {
		std::string query;
		ws_msg_parsed<WS_EVENT_TRANSACTION> tmp;
		ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> rop;

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
	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> pull_last_cycle_userid( pqxx::connection *C, std::string userid) {
		long ts_high, ts_low;

		ts_high = time(NULL)*1000;
		ts_low  = ts_high - 600*1000;

		return pull_transactions_userid_ts(C, userid, ts_high, ts_low);
	}

	/********************************************************************************
					COIN PRICE
	********************************************************************************/
	void push_coin_price( pqxx::connection* C, ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> op) {
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

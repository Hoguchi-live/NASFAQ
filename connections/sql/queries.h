#ifndef _QUERIES_H_
#define _QUERIES_H_

#include "../common/common.h"

namespace db {
namespace query {
namespace tmplt {
	/* Push a user in USERS */
	const std::string p_user =	"INSERT INTO USERS(USERID, USERNAME, ICON, NETWORTH) "\
					"VALUES('{}', E'{}', '{}', {}) "
					"ON CONFLICT (USERID) DO UPDATE SET "\
					"NETWORTH = {};";

	/* Push a transaction in HISTORY */
	const std::string p_transaction = "INSERT INTO HISTORY(COIN, TYPE, USERID, QUANTITY, TIMESTAMP, COMPLETED, PRICE) "\
						       "VALUES('{}', {}, '{}', {}, {}, {}, {});";

	/* Push a transaction in HISTORY with unicity check (for db completion) */
	const std::string p_utransaction =	"INSERT INTO HISTORY(COIN, TYPE, USERID, QUANTITY, TIMESTAMP, COMPLETED, PRICE) "\
						"SELECT '{}', {}, '{}', {}, {}, {}, {}  " \
						"WHERE NOT EXISTS (SELECT TIMESTAMP FROM HISTORY WHERE TIMESTAMP = {} limit 1);";

	/* Update coin price. */
	const std::string p_coin_price = 	"INSERT INTO COINPRICE(COIN, PRICE, SALEVALUE, INCIRCULATION, LASTUPDATE) "\
					       	"VALUES('{}', {}, {}, {}, {}) "\
					       	"ON CONFLICT (COIN) DO UPDATE SET "\
					       	"PRICE = {}, SALEVALUE = {}, INCIRCULATION = {}, LASTUPDATE = {};";

	const std::string f_history_ts =	"SELECT * FROM HISTORY "\
						"WHERE TIMESTAMP <= {} AND TIMESTAMP > {};";

	const std::string f_userid_ts =		"SELECT DISTINCT USERID FROM HISTORY " \
						"WHERE TIMESTAMP <= {} AND TIMESTAMP > {};";

	const std::string f_slope_ts_top =	"SELECT PRICE, TIMESTAMP FROM HISTORY "\
						"WHERE COIN = '{}' AND TIMESTAMP <= {} AND TIMESTAMP > {} "\
						"ORDER BY TIMESTAMP ASC "\
						"LIMIT 1;";

	const std::string f_slope_ts_bot =	"SELECT PRICE, TIMESTAMP FROM HISTORY "\
						"WHERE COIN = '{}' AND TIMESTAMP <= {} AND TIMESTAMP > {} "\
						"ORDER BY TIMESTAMP DESC "\
						"LIMIT 1;";

	const std::string f_transactions_userid_ts =	"SELECT * FROM HISTORY "\
							"WHERE USERID = '{}' AND TIMESTAMP <= {} AND TIMESTAMP > {};";

	const std::string f_last_n_spaced_prices =	"select PRICE, TIMESTAMP FROM last_n_spaced_prices('{}', {}, {});";


	const std::string f_id_to_username =	"select USERNAME FROM USERS WHERE USERID = '{}'";
} // db::query::tmpt

namespace db::queries::fmt {
	std::string p_transaction( ws_msg_parsed<WS_EVENT_TRANSACTION> op );
	std::string p_utransaction( ws_msg_parsed<WS_EVENT_TRANSACTION> op );
	std::string p_coin_price( ws_msg_parsed<WS_EVENT_COIN_PRICE> op );
	std::string p_user(user_t user);

	std::string f_history_ts( long ts_high, long ts_low );
	std::string f_userid_list_ts( long ts_high, long ts_low );
	std::string f_transactions_userid_ts( std::string userid, long ts_high, long ts_low );
	std::string f_top_price_cycle_ts( std::string coin, long ts_high, long ts_low );
	std::string f_bot_price_cycle_ts( std::string coin, long ts_high, long ts_low );
	std::string f_last_n_spaced_prices( std::string coin, int nb_cycles, int delta);
	std::string f_query_userid_to_username(std::string userid);
} // db::query::fmt





#endif


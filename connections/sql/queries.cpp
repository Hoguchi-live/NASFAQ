#include "queries.h"

namespace db::queries::fmt {

std::string p_transaction( ws_msg_parsed<WS_EVENT_TRANSACTION> op ) {
	return fmt::format(p_transaction,
		op.coin,
		op.type,
		op.userid,
		op.quantity,
		op.timestamp,
		op.completed,
		op.price);
}

std::string p_utransaction( ws_msg_parsed<WS_EVENT_TRANSACTION> op ) {
	return fmt::format(p_utransaction,
		op.coin,
		op.type,
		op.userid,
		op.quantity,
		op.timestamp,
		op.completed,
		op.price,
		op.timestamp);
}

std::string f_history_ts( long ts_high, long ts_low ) {
	return fmt::format(f_history_ts, ts_high, ts_low);
}

std::string f_userid_list_ts( long ts_high, long ts_low ) {
	return fmt::format(f_userid_ts, ts_high, ts_low);
}

std::string f_transactions_userid_ts( std::string userid, long ts_high, long ts_low ) {
	return fmt::format(f_transactions_userid_ts, userid, ts_high, ts_low);
}

std::string f_top_price_cycle_ts( std::string coin, long ts_high, long ts_low ) {
	return fmt::format(f_slope_ts_top, coin, ts_high, ts_low);
}

std::string f_bot_price_cycle_ts( std::string coin, long ts_high, long ts_low ) {
	return fmt::format(f_slope_ts_bot, coin, ts_high, ts_low);
}

std::string p_coin_price( ws_msg_parsed<WS_EVENT_COIN_PRICE> op ) {
	return fmt::format(p_coin_price,
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
std::string f_last_n_spaced_prices( std::string coin, int nb_cycles, int delta) {
	return fmt::format(f_last_n_spaced_prices,
			coin,
			nb_cycles,
			delta);
}

std::string p_user(user_t user){
	return fmt::format(p_user,
			user.userid,
			user.username,
			user.icon,
			user.networth,
			user.networth);
}

std::string f_query_userid_to_username(std::string userid) {
	return fmt::format(f_userid_to_username,
			userid);
}
} // db::queries::fmt

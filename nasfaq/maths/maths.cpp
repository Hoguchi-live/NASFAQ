#include "maths.h"

namespace query {
	const std::string query_template_slope_ts_bot =  "SELECT PRICE, TIMESTAMP FROM HISTORY "\
						     "WHERE COIN = '{}' AND TIMESTAMP <= {} AND TIMESTAMP > {} "\
						     "ORDER BY TIMESTAMP DESC "\
						     "LIMIT 1;";

	const std::string query_template_last_n_spaced_prices = "select PRICE, TIMESTAMP FROM last_n_spaced_prices('{}', {}, {});";
}

namespace norm {

	float norm(cycle_t c) {
		float rop = 0;

		for (auto const & element : c) {
			rop += pow(element.second, 2);
		}
		return sqrt(rop);
	}

	/* Should balance with weights. */
	cycle_t history_to_cycle(ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> op) {
		cycle_t rop;

		for(auto const& element : op.transaction_list) {
			rop[element.coin] = element.quantity * pow((-1),  element.type);
		}

		return rop;
	}

	cycle_t diff_vector(cycle_t op1, cycle_t op2) {
		cycle_t rop;

		rop = op1;

		for(auto const& element : op2) {
			if (op1.count(element.first)) {
				rop[element.first] -= element.second;
			} else {
				rop[element.first] = -element.second;
			}
		}

		return rop;
	}

	int cycle_size(cycle_t op) {
		int rop = 0;
		for (auto const & element : op) {
			rop += std::abs(element.second);
		}

		return rop;
	}

	std::map<std::string, float> diff_all(pqxx::connection *C, cycle_t base_cycle, std::map<std::string, cycle_t> cycles, std::vector<std::string> userid_list, std::string userid, long ts_high, long ts_low, float threshold) {
		std::map<std::string, float> result;
		cycle_t tmp1, tmp2;
		float tmp_res;

		for(auto const& id : userid_list) {
			tmp2 = cycles.find(id)->second;
			if (cycle_size(tmp2) >= 10) {
				tmp_res = norm(diff_vector(tmp1, tmp2));
				if (tmp_res < threshold) {
					result[id] = tmp_res;
					std::cout << "Difference between " << userid << " and " << id << " : " << result[id] << std::endl;
				}
			}
		}
		return result;
	}

	std::map<std::string, std::map<std::string, float>> diff_map(pqxx::connection *C, std::vector<std::string> userids, long ts_high, long ts_low, float threshold){
		std::map<std::string, std::map<std::string, float>> result;
		std::map<std::string, cycle_t> cycles;
		std::map<std::string, float> tmp;

		/* Prepare cycles from db. */
		for(auto const& id : userids) {
			cycles.insert( std::pair<std::string, cycle_t>(id,  history_to_cycle(db::pull_last_cycle_userid(C, id))) );
		}

		for (auto& userid:userids) {
			tmp = norm::diff_all(C, cycles[userid], cycles, userids, userid, ts_high, ts_low, threshold);
			result.insert( std::pair<std::string, std::map<std::string, float>>( userid, tmp ));
		}

		return result;
	}

	void diff_save_json(	pqxx::connection *C, std::map<std::string, std::map<std::string, float>> op) {
		json j;
		std::string parent, child;

		for (auto& userid1_map:op) {
			parent = db::userid_to_username(C, userid1_map.first);
			for (auto& element:userid1_map.second) {
				child = db::userid_to_username(C, element.first);
				j[parent][child] = element.second;
			}
		}
		std::ofstream o("graph.json");
		o << std::setw(4) << j << std::endl;
	}
}

namespace optimizer{
	std::tuple<long, long> get_last_nth_cycle_ts(int n) {
		long ts_now, ts_top, ts_bot;

		ts_now = time(NULL);
		ts_top = (ts_now - ts_now % 600) - n * 600; // floor (top) - n cycles
		ts_bot = ts_top - (n+1) * 600;

		return std::make_tuple(ts_top * 1000, ts_bot * 1000);
	}

	///* Returns slope over last cycle. */
	//float pull_last_cycle_slope(pqxx::connection* C, std::string coin) {
	//	std::string query;

	//	float price_top, price_bot, delta_p;
	//	float ts_top, ts_bot, delta_t;
	//	float slope;

	//	pqxx::nontransaction N(*C);
	//	pqxx::result::const_iterator c;
	//	pqxx::result R;

	//	auto [ts_high, ts_low] = optimizer::get_last_nth_cycle_ts(1);
	//	//ts_high = time(NULL)*1000;
	//	//ts_low  = ts_high - 600*1000;

	//	query = db::query::make_pull_query_top_price_cycle_ts(coin, ts_high, ts_low);
	//	R = N.exec( query );
	//	c = R.begin();
	//	price_top = c[0].as<float>();
	//	ts_top = c[1].as<float>();

	//	query = db::query::make_pull_query_bot_price_cycle_ts(coin, ts_high, ts_low);
	//	R = N.exec( query );
	//	c = R.begin();
	//	price_bot = c[0].as<float>();
	//	ts_bot = c[1].as<float>();

	//	slope = (price_top - price_bot) / ( (ts_top - ts_bot) ) * 1000 * 600;
	//	std::cout << "pt: " << price_top << " pb: " << price_bot << std::endl;
	//	std::cout << "tt: " << ts_top << " tb: " << ts_bot << std::endl;


	//	return slope;
	//}

	float get_last_n_weighted_slope(pqxx::connection* C, std::string coin, int nb_cycles, int delta = 600) {
		std::string query;
		float wma, tmp_slope, price_top, price_bot;
		double ts_top, ts_bot;
		int weight, denum;

		query = db::query::make_pull_query_last_n_spaced_prices(coin, nb_cycles, delta);

   		/* Create a non-transactional object. */
		pqxx::nontransaction N(*C);

   		/* Execute SQL query */
		pqxx::result R( N.exec( query ));

		/* Parse result. */
		price_top = R[0][0].as<float>();
		price_bot = R[1][0].as<float>();
		ts_top = R[0][1].as<double>();
		ts_bot = R[1][1].as<double>();

		tmp_slope = (price_top - price_bot) / (ts_top - ts_bot);
		weight = nb_cycles;

		wma = weight * tmp_slope;
		denum = weight;

		for (pqxx::result::const_iterator c = R.begin() + 1; c != R.end(); ++c) {
			price_bot = price_top;
			ts_bot = ts_top;

			price_top = c[0].as<float>();
			ts_top = c[1].as<double>();

			weight--;
			tmp_slope = (price_top - price_bot) / (ts_top - ts_bot);
			wma += weight * tmp_slope;
			denum += weight;
		}

		return wma / denum * 1000 * 600;
	}
}

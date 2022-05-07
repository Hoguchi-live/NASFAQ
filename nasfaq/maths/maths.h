#ifndef _MATHS_H_
#define _MATHS_H_

#include <cmath>
#include <fstream>
#include <tuple>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "../connections/common/common.h"
#include "../connections/sql/db_handle.h"

namespace norm {
	float norm(cycle_t);
	cycle_t history_to_cycle(ws_msg_parsed<WS_EVENT_HISTORY_UPDATE>);
	cycle_t diff_vector(cycle_t, cycle_t);
	std::map<std::string, float> diff_all(pqxx::connection *C, cycle_t base_cycle, std::map<std::string, cycle_t> cycles, std::vector<std::string> userid_list, std::string userid, long ts_high, long ts_low, float threshold);
	std::map<std::string, std::map<std::string, float>> diff_map(pqxx::connection *C, std::vector<std::string> userid_list, long, long, float);
	void diff_save_json(		pqxx::connection *, std::map<std::string, std::map<std::string, float>>);
}

namespace optimizer{
	std::tuple<long, long> get_last_nth_cycle_ts(int);
	float get_last_n_weighted_slope(pqxx::connection*, std::string, int, int);
}

#endif


#include "../common/common.h"
#include "../sql/db_init.h"
#include "../sql/db_handle.h"
#include "../../maths/maths.h"
#include "../../maths/graph.h"

int connect_db() {

	bool done = false;
	std::string input;

	/* Populate database if needed. */
	db::populate();

	/* Open up database connection. */
	pqxx::connection C("dbname = nasfaq user = steaky  \
   	hostaddr = 127.0.0.1 port = 5432");

   	if (C.is_open()) {
		std::cout << "Opened database successfully: " << C.dbname() << std::endl;
   	} else {
		std::cout << "Can't open database" << std::endl;
   		return 1;
   	}

	/* Loop and print data through parser*/
	while(!done) {
		input = std::cin.get();

		if(input == "q" || input == "quit") {
			done = true;
		} else if (input == "t") {
			std::string coin = "miko";
			//float s = db::pull_last_cycle_slope(&C, coin);
			float ws = optimizer::get_last_n_weighted_slope(&C, coin, 2, 600);
			std::cout << "slope: " << ws << std::endl;
		} else if (input == "d") {
			long ts_low, ts_high;
			float threshold;

			std::vector<std::string> userids;
			std::map<std::string, std::map<std::string, float>> result;

			ts_high = time(NULL)*1000;
			ts_low  = ts_high - 600*1000;
			threshold = 10;

			userids = db::pull_userid_list_ts(&C, ts_high, ts_low);
			result =  norm::diff_map(&C, userids, ts_high, ts_low, threshold);

			//make_graph_from_map(result);
			norm::diff_save_json(&C, result);
		}
	}

	return 0;
}

int main(void) {
	connect_db();
	return 1;
}

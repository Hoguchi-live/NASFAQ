#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>

#include "../common/common.h"
#include "../sql/db_init.h"
#include "../sql/db_handle.h"
#include "../../maths/maths.h"
#include "../../maths/graph.h"


std::map<std::string, std::map<std::string, float>> make_map(pqxx::connection *C) {
	// Timestamps
	long ts_high = time(NULL)*1000;
	long ts_low = ts_high - 60*10*1000;
	float thresh = 10;

	/* Create graph (test zone) */
	std::map<std::string, std::map<std::string, float>> res;
	std::vector<std::string> userids = db::pull_userid_list_ts(C, ts_high, ts_low);
	std::cout << "Number of userids active during delta_ts: " << userids.size() << std::endl;

	res = norm::diff_map(C, userids, ts_high, ts_low, thresh);

	return res;
}

void make_json(pqxx::connection *C, std::map<std::string, std::map<std::string, float>> op) {
	norm::diff_save_json(C, op);
}


void make_graph(std::map<std::string, std::map<std::string, float>> op) {
	make_graph_from_map(op);
}


int main(void) {
	/* Open up database connection. */
	pqxx::connection C("dbname = nasfaq user = steaky  \
   	hostaddr = 127.0.0.1 port = 5432");

   	if (C.is_open()) {
		std::cout << "Opened database successfully: " << C.dbname() << std::endl;
   	} else {
		std::cout << "Can't open database" << std::endl;
   	}

	std::map<std::string, std::map<std::string, float>> res;

	res = make_map(&C);
	make_json(&C, res);
	//make_graph(res);
	return 1;
}

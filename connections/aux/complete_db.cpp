#include "complete_db.h"


ws_msg_parsed<WS_EVENT_TRANSACTION> raw_msg_parse(nlohmann::json op) {

	ws_msg_parsed<WS_EVENT_TRANSACTION> rop;
	rop.coin = op["coin"];
	rop.type = op["type"];
	rop.userid = op["userid"];
	rop.quantity = op["quantity"];
	rop.timestamp = op["timestamp"];
	rop.completed = op["completed"];
	rop.timestamp = op["timestamp"];
	rop.price = op["price"];

	return rop;
}

int complete_db(long ts_up, unsigned int days, bool check) {
	long tmp_ts;
	std::string BASE_URL = "https://nasfaq.biz/api/getHistory?timestamp={}";
	std::string TMP_URL;
	nlohmann::json res_json;
	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> tmp;
	nsfq_http::http_connector::connector c;

	/* Open up database connection. */
	pqxx::connection C("dbname = nasfaq user = steaky  \
   	hostaddr = 127.0.0.1 port = 5432");

   	if (C.is_open()) {
		std::cout << "Opened database successfully: " << C.dbname() << std::endl;
   	} else {
		std::cout << "Can't open database" << std::endl;
   		return 1;
   	}

	/* Update database if needed */
	tmp_ts = ts_up;
	for(int i = 0; i < days; i++) {
		std::cout << "Fetching history for ts = " << tmp_ts << std::endl;
		tmp_ts -= 60*60*24*1000;
		TMP_URL = fmt::format(BASE_URL, tmp_ts);
		c.get((const char*)TMP_URL.c_str());


		res_json = nlohmann::json::parse(c.get_buffer());

		tmp = {};
		if(!res_json.empty()) {
			for(auto const & x:res_json["history"]["transactions"]) {
				tmp.transaction_list.push_back(raw_msg_parse(x));
			}
		}

		if(check) db::push_uhistory(&C, tmp);
		else db::push_history(&C, tmp);
	}
	return 0;
}

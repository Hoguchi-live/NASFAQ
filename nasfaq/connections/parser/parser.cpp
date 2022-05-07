#include "parser.h"


namespace parser {

WS_MSG msg_type_detect(std::string op) {
	WS_MSG ret;

	if (op.substr(0, 30).find("coinPriceUpdate") != std::string::npos) {
		ret = WS_EVENT_COIN_PRICE_UPDATE;
	} else if (op.substr(0, 30).find("historyUpdate") != std::string::npos) {
		ret = WS_EVENT_HISTORY_UPDATE;
	} else if (op.substr(0, 30).find("todayPricespdate") != std::string::npos) {
		ret = WS_EVENT_TODAY_PRICES_UPDATE;
	} else if (op.substr(0, 30).find("brokerFeeUpdate") != std::string::npos) {
		ret = WS_EVENT_BROKER_FEE_UPDATE;
	} else {
		ret = WS_EVENT_UNKNOWN;
	}
	return ret;
}

template <WS_MSG E>
ws_msg_parsed<E> parse(std::string rmsg) {};

template <>
ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> raw_msg_parse<WS_EVENT_COIN_PRICE_UPDATE>(std::string rmsg) {
	nlohmann::json jparsed = nlohmann::json::parse(rmsg); /* Check for errors and emptiness needed */

	ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> rop;
	rop.coin = jparsed["coin"];
	rop.price = (jparsed["info"])["price"];
	rop.saleValue = (jparsed["info"])["saleValue"];
	rop.inCirculation = (jparsed["info"])["inCirculation"];

	return rop;
}

/****************************************************************
  Helper functions should go in common or something more general
****************************************************************/

/*
   See https://stackoverflow.com/questions/2896600/how-to-replace-all-occurrences-of-a-character-in-string
*/
static inline void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
}

/*
   Splits a string "{...}, ..., {...}" in an array ["{...}", ..., "{...}"].
   ONLY HANDLES DICTIONARIES OF DEPTH 1.
*/
std::vector<std::string> tokenize_json_array(std::string op, std::string token = "}") {
	int start = 0;
	int end = op.find("}");
	std::vector<std::string> ret;

	while( end != -1 ) {
		ret.push_back(op.substr(start, end - start + 1));
		start = end + token.size() + 1; // + 1 accounts for the ",".
        	end = op.find(token, start);
	}

	return ret;
}

template <>
ws_msg_parsed<WS_EVENT_TRANSACTION> raw_msg_parse<WS_EVENT_TRANSACTION>(std::string rmsg) {
	nlohmann::json jparsed = nlohmann::json::parse(rmsg); /* Check for errors and emptiness needed */

	ws_msg_parsed<WS_EVENT_TRANSACTION> rop;
	rop.coin = jparsed["coin"];
	rop.type = jparsed["type"];
	rop.userid = jparsed["userid"];
	rop.quantity = jparsed["quantity"];
	rop.timestamp = jparsed["timestamp"];
	rop.completed = jparsed["completed"];
	rop.timestamp = jparsed["timestamp"];
	rop.price = jparsed["price"];

	return rop;
}

template<>
ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> raw_msg_parse<WS_EVENT_HISTORY_UPDATE>(std::string rmsg) {
	std::vector<std::string> raw_vect;
	ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> rop;

	/* Replace \" by " */
	replace_all(rmsg, "\\\"", "\"");

	/* Extract array */
	raw_vect = tokenize_json_array(rmsg);

	/* Create the output array by parsing each transaction elements */
	for(auto & raw_tr : raw_vect) {
		rop.transaction_list.push_back(raw_msg_parse<WS_EVENT_TRANSACTION>(raw_tr));
	}

	return rop;
}

parser::parser(ws::connection_metadata::ptr metadata, pqxx::connection* C)
	: m_metadata(metadata)
	, m_process_queue_state(false)
	, m_connection(C)
{}

parser::~parser() {

}

void parser::process_queue() {

	WS_MSG type;
	std::string raw_data;
	std::string parsed_msg;

	m_process_queue_state = true;

	while(m_process_queue_state) {
		raw_data = m_metadata->pop_message();
		type = msg_type_detect(raw_data);

		/* POSTGRESQL STUFF GOES HERE */
		if (type == WS_EVENT_COIN_PRICE_UPDATE) {
			/* 18 = 3 + 15 */
			raw_data = raw_data.substr(18, raw_data.length()-18);
			ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> parsed_msg = raw_msg_parse<WS_EVENT_COIN_PRICE_UPDATE>(raw_data);

			std::cout << parsed_msg << std::endl;
			db::push_coin_price(m_connection, parsed_msg);

		} else if (type == WS_EVENT_HISTORY_UPDATE) {
			raw_data = raw_data.substr(18, raw_data.length()-18);
			ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> parsed_msg = raw_msg_parse<WS_EVENT_HISTORY_UPDATE>(raw_data);

			std::cout << parsed_msg << std::endl;
			db::push_history(m_connection, parsed_msg);
			//std::cout << "\x1B[31mTexting\033[0m\t\t" << std::endl;
		}
	}
}

void* parser::process_queue_helper(void* arg)
{
	parser* p = reinterpret_cast<parser*>(arg);
	p->process_queue();
	return 0;
}

void parser::process_queue_start() {
	int rc = pthread_create(&m_process_queue_thread, NULL,  process_queue_helper, this);

	if (rc) {
		std::cout << "Error:unable to create thread," << rc << std::endl;
	   exit(-1);
	}
}

void parser::process_queue_thread_join()
{
	pthread_join(m_process_queue_thread, 0);
}


void parser::process_queue_stop() {
	m_process_queue_state = false;
}

}

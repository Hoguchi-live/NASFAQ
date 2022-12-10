#include "parser.h"

namespace parser {

/*******************************************************************************
	Parser object
*******************************************************************************/
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

		type = types::extract(raw_data);

		// TODO: make a function for each of these cases and use a switch as in parser_aux

		/* POSTGRESQL STUFF GOES HERE */
		if (type == WS_EVENT_COIN_PRICE) {
			/* 18 = 3 + 15 */
			raw_data = raw_data.substr(18, raw_data.length()-18);
			ws_msg_parsed<WS_EVENT_COIN_PRICE> parsed_msg = single<WS_EVENT_COIN_PRICE>(raw_data);

			std::cout << parsed_msg << std::endl;
			db::push_coin_price(m_connection, parsed_msg);

		} else if (type == WS_EVENT_HISTORY) {
			raw_data = raw_data.substr(18, raw_data.length()-18);
			ws_msg_parsed<WS_EVENT_HISTORY> parsed_msg = single<WS_EVENT_HISTORY>(raw_data);

			std::cout << parsed_msg << std::endl;
			db::push_history(m_connection, parsed_msg);
			//std::cout << "\x1B[31mTexting\033[0m\t\t" << std::endl;
		}
		 else if (type == WS_EVENT_MF_PORTFOLIO) {
			raw_data = raw_data.substr(28, raw_data.length()-28);
			//raw_data = payload::extract(raw_data);
			ws_msg_parsed<WS_EVENT_MF_PORTFOLIO> parsed_msg = single<WS_EVENT_MF_PORTFOLIO>(raw_data);

			std::cout << parsed_msg << std::endl;
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

/*******************************************************************************
	Message parsing
*******************************************************************************/
//template <WS_MSG E>
//ws_msg_parsed<E> parse(std::string rmsg) {};

/*
   Takes as input a raw string associated to a coinPriceUpdate ws event and returns a parsed representation of it.
*/
template <>
ws_msg_parsed<WS_EVENT_COIN_PRICE> single<WS_EVENT_COIN_PRICE>(std::string rmsg) {
	nlohmann::json jparsed = nlohmann::json::parse(rmsg); /* Check for errors and emptiness needed */

	ws_msg_parsed<WS_EVENT_COIN_PRICE> rop;
	rop.coin = jparsed["coin"];
	rop.price = (jparsed["info"])["price"];
	rop.saleValue = (jparsed["info"])["saleValue"];
	rop.inCirculation = (jparsed["info"])["inCirculation"];

	return rop;
}

/*
   Takes as input a raw string associated to a transaction ws event and returns a parsed representation of it.
*/
template <>
ws_msg_parsed<WS_EVENT_TRANSACTION> single<WS_EVENT_TRANSACTION>(std::string rmsg) {
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
	// rop.fund = jparsed["fund"];

	return rop;
}

/*
   Same as above but takes a json as input.
*/
template <>
ws_msg_parsed<WS_EVENT_TRANSACTION> single_j<WS_EVENT_TRANSACTION>(nlohmann::json op) {

	ws_msg_parsed<WS_EVENT_TRANSACTION> rop;

	rop.coin = op["coin"];
	rop.type = op["type"];
	rop.userid = op["userid"];
	rop.quantity = op["quantity"];
	rop.timestamp = op["timestamp"];
	rop.completed = op["completed"];
	rop.timestamp = op["timestamp"];
	if(op["price"] == nullptr) {
		rop.price = 0;
	} else {
		rop.price = op["price"];
	}
	// rop.fund = op["fund"];

	return rop;
}

/*
   Takes as input a raw string associated to a historyUpdate ws event and returns a parsed representation of it.
*/
template<>
ws_msg_parsed<WS_EVENT_HISTORY> single<WS_EVENT_HISTORY>(std::string rmsg) {
	std::vector<std::string> raw_vect;
	ws_msg_parsed<WS_EVENT_HISTORY> rop;

	/* Replace \" by " */
	replace_all(rmsg, "\\\"", "\"");

	/* Extract array */
	raw_vect = tokenize_json_array(rmsg);

	/* Create the output array by parsing each transaction elements */
	for(auto & raw_tr : raw_vect) {
		rop.transaction_list.push_back(single<WS_EVENT_TRANSACTION>(raw_tr));
	}

	return rop;
}
///////////////////////////////// TODO: PUT THIS FUCKING TRASH SOMEWHERE ELSE /////////
/*
   Takes as input a json and returns its representation as portfolio_coin_t.
*/
portfolio_coin_t parse_portfolio_coin(std::string coin, nlohmann::json op) {
	portfolio_coin_t rop;

	rop.coin = coin;
	rop.amount = op["amount"];
	rop.ts = op["timestamp"];
	rop.mpp = op["meanPurchasePrice"];

	return rop;
}
////////////////////////////////////////////////////////////////////////////

/*
   Takes as input a raw string associated to a mutualFundPortfolioUpdate as ws event and returns a parsed representation of it.
*/
template <>
ws_msg_parsed<WS_EVENT_MF_PORTFOLIO> single<WS_EVENT_MF_PORTFOLIO>(std::string rmsg) {
	ws_msg_parsed<WS_EVENT_MF_PORTFOLIO> rop;

	nlohmann::json jparsed = nlohmann::json::parse(rmsg); /* Check for errors and emptiness needed */

	rop.fund = jparsed["fund"];
	rop.event = (jparsed["tUpdate"])["event"];

	for(auto & raw_tr : (jparsed["tUpdate"])["transactions"]) {

		rop.transactions.push_back(single_j<WS_EVENT_TRANSACTION>(raw_tr));
	}

	if((jparsed["tUpdate"])["portfolio"] !=  NULL) {
		for(auto & item : (jparsed["tUpdate"])["portfolio"].items()) {
			rop.portfolio.push_back(parse_portfolio_coin(item.key(), item.value()));
		}
	}

	return rop;
}

} //parser

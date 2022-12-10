#include "../../common/common.h"
#include "../../http/http_connector.h"
#include "../../ws/http_handshake.h"
#include "../../ws/ssl_ws.h"

int connect_ws() {

	bool done = false;
	std::string input;
	ws::websocket_endpoint endpoint;

	/* Get session id */
	std::string sid = ws::http_handshake::get_sid();

	/* Start connection */
	std::string uri = "wss://nasfaq.biz/socket/?user=314d0bda-d7f0-4636-aed7-5ea02743604b&EIO=4&transport=websocket&sid=" + sid;
	int id = endpoint.connect(uri);
	if(id != -1) {
		std::cout << ">Created nasfaq websocket connection" << std::endl;
	} else {
		return -1;
	}
	sleep(1);

	/* Display connection metadata */
	ws::connection_metadata::ptr metadata = endpoint.get_metadata(id);
	if (metadata) {
		std::cout << *metadata << std::endl;
	} else {
		std::cout << ">Unknown connection id " << id << std::endl;
	}

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

	/* Update users. */
	db::push_users(&C, users::get_users());

	/* Set up parser */
	parser::parser p(metadata, &C);

	/* Loop and print data through parser*/
	while(!done) {
		input = std::cin.get();

		if(input == "q" || input == "quit") {
			done = true;
		} else if (input == "p") {
			std::cout << "PROCESSING QUEUE " << std::endl;
			p.process_queue_start();
		} else if (input == "s") {
			std::cout << "STOPPING QUEUE PROCESSING" << std::endl;
			p.process_queue_stop();
		} else if (input == "t") {
			std::cout << "TEST" << std::endl;
			auto ret = db::pull_last_cycle(&C);
			std::cout << ret << std::endl;
		}
	}

	/* Close websocket */
	std::stringstream ss(input);

	std::string cmd;
	int close_code = websocketpp::close::status::normal;
	std::string reason;

	ss >> cmd >> id >> close_code;
	std::getline(ss, reason);

	endpoint.close(id, close_code, reason);

	return 0;
}

int main(void) {
	connect_ws();
	return 1;
}

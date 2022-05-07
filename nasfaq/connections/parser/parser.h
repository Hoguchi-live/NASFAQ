#ifndef _PARSER_H_
#define _PARSER_H_

#include <iostream>
#include <string>
#include <stdlib.h>

#include <nlohmann/json.hpp>
#include "../sql/db_handle.h"

#include "../common/common.h"
#include "../ws/ssl_ws.h"

namespace parser {

WS_MSG msg_type_detect(std::string);

template <WS_MSG E>
ws_msg_parsed<E> raw_msg_parse(std::string);

template <>
ws_msg_parsed<WS_EVENT_COIN_PRICE_UPDATE> raw_msg_parse<WS_EVENT_COIN_PRICE_UPDATE>(std::string);

template<>
ws_msg_parsed<WS_EVENT_TRANSACTION> raw_msg_parse<WS_EVENT_TRANSACTION>(std::string);

template<>
ws_msg_parsed<WS_EVENT_HISTORY_UPDATE> raw_msg_parse<WS_EVENT_HISTORY_UPDATE>(std::string);

class parser {
public:
	parser(ws::connection_metadata::ptr, pqxx::connection*);
	~parser(void);

	void process_queue();
	void process_queue_start();
	void process_queue_stop();
	void process_queue_thread_join();
private:
	ws::connection_metadata::ptr m_metadata;
	bool m_process_queue_state;
   	pthread_t m_process_queue_thread;
	pqxx::connection* m_connection;

	static void* process_queue_helper(void*);
};

}

#endif


#ifndef _PARSER_H_
#define _PARSER_H_

#include <iostream>
#include <string>
#include <stdlib.h>

#include <nlohmann/json.hpp>
#include "../sql/db_handle.h"

#include "../common/common.h"
#include "../common/formatting.h"
#include "../ws/ssl_ws.h"

#include "./parser_aux.h"
//TODO: Remove ws here and use a safequeue

namespace parser {

/*******************************************************************************
	Parser object
*******************************************************************************/
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

/*******************************************************************************
	Message parsing
*******************************************************************************/
template <WS_MSG E>
ws_msg_parsed<E> single(std::string);

template <WS_MSG E>
ws_msg_parsed<E> single_j(nlohmann::json);

template <>
ws_msg_parsed<WS_EVENT_COIN_PRICE> single<WS_EVENT_COIN_PRICE>(std::string);

template<>
ws_msg_parsed<WS_EVENT_TRANSACTION> single<WS_EVENT_TRANSACTION>(std::string);

template <>
ws_msg_parsed<WS_EVENT_TRANSACTION> single_j<WS_EVENT_TRANSACTION>(nlohmann::json );

template<>
ws_msg_parsed<WS_EVENT_HISTORY> single<WS_EVENT_HISTORY>(std::string);

template <>
ws_msg_parsed<WS_EVENT_MF_PORTFOLIO> single<WS_EVENT_MF_PORTFOLIO>(std::string);

} // parser
#endif


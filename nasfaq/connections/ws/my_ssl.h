#ifndef _MY_SSL_H_
#define _MY_SSL_H_

#include <iostream>
#include <string>
#include <stdlib.h>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include <nlohmann/json.hpp>

#include "../parser/parser.h"
#include "../common/common.h"
#include "../safe_queue/safe_queue.h"

int connect_ws(std::string);

#endif


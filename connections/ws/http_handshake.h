#ifndef _HTTP_HANDSHAKE_H_
#define _HTTP_HANDSHAKE_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "../http/http_connector.h"

namespace ws {
	namespace http_handshake {
		std::string get_sid();
	}
}

#endif


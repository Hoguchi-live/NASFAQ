#ifndef _USERS_H_
#define _USERS_H_

#include <iostream>
#include <stdio.h>
#include <string.h>

//#include <curl/curl.h>
//#include <nlohmann/json.hpp>

#include "../http/http_connector.h"
#include "../common/common.h"

namespace users {
	std::vector<user_t> get_users();
}

#endif


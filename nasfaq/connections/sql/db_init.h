#ifndef _DB_INIT_H_
#define _DB_INIT_H_

#include <iostream>
#include <pqxx/pqxx>
#include <fmt/core.h>
#include <ctime>

#include "../http/users.h"

namespace db {
	int create_table_history(pqxx::connection&);
	int create_table_coin_price(pqxx::connection&);
	int create_table_users(pqxx::connection&);
	int populate();
}

#endif


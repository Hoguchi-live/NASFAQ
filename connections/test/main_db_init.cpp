#include "../common/common.h"
#include "../sql/db_init.h"

int connect_db() {

	bool done = false;
	std::string input;

	/* Populate database if needed. */
	db::create::database::all();

	/* Open up database connection. */
	pqxx::connection C("dbname = nasfaq user = steaky  \
   	hostaddr = 127.0.0.1 port = 5432");

   	if (C.is_open()) {
		std::cout << "Opened database successfully: " << C.dbname() << std::endl;
   	} else {
		std::cout << "Can't open database" << std::endl;
   		return 1;
   	}

	return 0;
}

int main(void) {
	connect_db();
	return 1;
}

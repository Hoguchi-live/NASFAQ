#include "db_init.h"

namespace db {

int create_table_history(pqxx::connection& C)  {
	std::string query;

	try {
		/* Create a transactional object. */
		pqxx::work W(C);

		/* Create sql query for history table creation */
		query = "CREATE TABLE IF NOT EXISTS HISTORY(" \
			"ID SERIAL PRIMARY KEY NOT NULL," \
			"COIN CHAR(32) NOT NULL,"\
			"TYPE INT,"\
			"USERID CHAR(128) NOT NULL,"\
			"QUANTITY INT,"\
			"TIMESTAMP BIGINT NOT NULL,"\
			"COMPLETED BOOLEAN,"\
			"PRICE REAL);";

		/* Execute SQL query */
      		W.exec( query );
      		W.commit();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

int create_table_coin_price(pqxx::connection& C) {
	std::string query;

	try {
		/* Create a transactional object. */
		pqxx::work W(C);

		/* Create sql query for history table creation */
		query = "CREATE TABLE IF NOT EXISTS COINPRICE(" \
			"COIN CHAR(32) PRIMARY KEY,"\
			"PRICE REAL," \
			"SALEVALUE REAL," \
			"INCIRCULATION INT, "\
			"LASTUPDATE BIGINT);";

		/* Execute SQL query */
      		W.exec( query );
      		W.commit();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

int create_last_n_anchor_prices_function(pqxx::connection& C) {
	std::string query;

	try {
		/* Create a transactional object. */
		pqxx::work W(C);

		/* Create sql query for spaced prices function, delta is in seconds*/
		query = "CREATE OR REPLACE FUNCTION last_n_spaced_prices(var_coin CHAR(32), var_n INT, var_delta INT) RETURNS SETOF HISTORY AS $$"  \
		"DECLARE  "\
		"    row      HISTORY%ROWTYPE;  "\
		"    cur_date BIGINT = 9223372036854775806;  "\
		"    nb_dates INT = 0;  "\
		"    nb_dates_max INT = var_n; "\
		"BEGIN  "\
		"    FOR row IN  "\
		"        SELECT *  "\
		"        FROM HISTORY  "\
		"        WHERE HISTORY.COIN = var_coin  "\
		"        ORDER BY TIMESTAMP DESC  "\
		"    LOOP  "\
		"    	IF nb_dates = nb_dates_max  "\
		"        THEN  "\
		"        	EXIT;  "\
		"        END IF;  "\
		"          "\
		"        IF row.TIMESTAMP <= cur_date - var_delta*1000 OR cur_date IS NULL  "\
		"        THEN  "\
		"            cur_date := row.TIMESTAMP;  "\
		"            nb_dates := nb_dates + 1;  "\
		"            RETURN NEXT row;  "\
		"        END IF;  "\
		"    END LOOP;  "\
		"END;  "\
		"$$ LANGUAGE plpgsql;  ";

		/* Execute SQL query */
      		W.exec( query );
      		W.commit();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;

}

int create_table_users(pqxx::connection& C) {
	std::string query;

	try {
		/* Create a transactional object. */
		pqxx::work W(C);

		/* Create sql query for history table creation */
		query = "CREATE TABLE IF NOT EXISTS USERS(" \
			"USERID CHAR(128) PRIMARY KEY,"\
			"USERNAME TEXT NOT NULL,"\
			"ICON CHAR(32) NOT NULL,"\
			"NETWORTH REAL);";

		/* Execute SQL query */
      		W.exec( query );
      		W.commit();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;

}


int populate() {
	try {
		pqxx::connection C("dbname = nasfaq user = steaky  \
   		hostaddr = 127.0.0.1 port = 5432");

   		if (C.is_open()) {
			std::cout << "Opened database successfully: " << C.dbname() << std::endl;
   		} else {
			std::cout << "Can't open database" << std::endl;
   			return 1;
   		}

		/* Create tables. */
		create_table_history(C);
		create_table_coin_price(C);
		create_table_users(C);

		/* Create functions. */
		create_last_n_anchor_prices_function(C);

	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
}

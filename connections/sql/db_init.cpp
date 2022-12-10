#include "db_init.h"

/*******************************************************************************
	Wrapper for single querying
*******************************************************************************/

int single_query(pqxx::connection& C, std::string query) {
	try {
		pqxx::work W(C);
      		W.exec( query );
      		W.commit();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}

namespace db {
namespace create {

/*******************************************************************************
	Table creation queries
*******************************************************************************/
namespace table {

int history(pqxx::connection& C)  {
	// TODO: make timestamp the primary key
	// 	 make sizes macros
	std::string query = "CREATE TABLE IF NOT EXISTS HISTORY(" \
		"ID SERIAL PRIMARY KEY NOT NULL," \
		"COIN CHAR(32) NOT NULL,"\
		"TYPE INT,"\
		"USERID CHAR(128) NOT NULL,"\
		"QUANTITY INT,"\
		"TIMESTAMP BIGINT NOT NULL,"\
		"COMPLETED BOOLEAN,"\
		"PRICE REAL);";
	return single_query(C, query);
}

int coin_price(pqxx::connection& C) {
	std::string query = "CREATE TABLE IF NOT EXISTS COINPRICE(" \
		"COIN CHAR(32) PRIMARY KEY,"\
		"PRICE REAL," \
		"SALEVALUE REAL," \
		"INCIRCULATION INT, "\
		"LASTUPDATE BIGINT);";
	return single_query(C, query);
}

int users(pqxx::connection& C) {
	std::string query = "CREATE TABLE IF NOT EXISTS USERS(" \
		"USERID CHAR(128) PRIMARY KEY,"\
		"USERNAME TEXT NOT NULL,"\
		"ICON CHAR(32) NOT NULL,"\
		"NETWORTH REAL);";
	return single_query(C, query);

}
} // db::create::table

/*******************************************************************************
	Function creation queries
*******************************************************************************/
namespace function {

int last_n_anchor_prices_function(pqxx::connection& C) {
	/* Create sql query for spaced prices function, delta is in seconds*/
	std::string query = "CREATE OR REPLACE FUNCTION last_n_spaced_prices(var_coin CHAR(32), var_n INT, var_delta INT) RETURNS SETOF HISTORY AS $$"  \
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
	return single_query(C, query);
}
} // db::create::function

/*******************************************************************************
	Database creation query
*******************************************************************************/
namespace database{

int all(void) {
	try {
		// TODO: GLOBAL INFO
		pqxx::connection C("dbname = nasfaq user = steaky hostaddr = 127.0.0.1 port = 5432");

   		if (C.is_open()) {
			std::cout << "Opened database successfully: " << C.dbname() << std::endl;
   		} else {
			std::cout << "Can't open database" << std::endl;
   			return 1;
   		}

		table::history(C);
		table::coin_price(C);
		table::users(C);

		function::last_n_anchor_prices_function(C);

	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
} // db::create::database
} // db::create
}

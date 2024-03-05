#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "psafe/psafe.h"

#define SUCCESS 0
#define FAIL 1

static int default_sqlite_callback(
	__attribute_maybe_unused__ void *data,
	const int col_count,
	char **values,
	char **col_names
) {
	for (int i = 0; i < col_count; ++i) {
		char *val;
		if (values[i] != NULL) {
			val = values[i];
		} else {
			val = "NULL";
		}
		if (psafe_println("%s = %s", col_names[i], val)) {
			return FAIL;
		}
	}
	if (psafe_print("\n")) {
		return FAIL;
	}
	return SUCCESS;
}

static int print_table(sqlite3 *db, const char *table) {
	const char *selstr = "SELECT * from ";
	const int max_query_len = 100;
	const int query_len = strlen(table);

	if (query_len > max_query_len) {
		psafe_eprintln(
			"print_table: Max query length exceeded (max: %d, actual: %d)",
			query_len, max_query_len);
		return FAIL;
	}

	const size_t query_size = strlen(selstr) + max_query_len + 1;
	char query[query_size];
	strcpy(query, selstr);

	char* error_msg;

	strncat(query, table, query_size);

	if (psafe_println("%s contents:\n- - -", table)) {
		return FAIL;
	}

	if (sqlite3_exec(db, query, default_sqlite_callback, NULL, &error_msg)) {
		psafe_eprintln("print_table: Select was unsuccessful: %s", error_msg);
		return FAIL;
	}
	return SUCCESS;
}

static int db_exec(sqlite3 *db, const char *query) {
	char* error_msg;
	if (sqlite3_exec(db, query, default_sqlite_callback, NULL, &error_msg)) {
		psafe_eprintln("db_exec: Unsuccessful database operation: %s\n", error_msg);
		return FAIL;
	}
	return SUCCESS;
}

static int print_all_tables(sqlite3 *db) {
	if (psafe_println("\n# All tables #\n--------------\n")) {
		return FAIL;
	}
	if (print_table(db, "PRODUCTS")) { return FAIL; }
	if (print_table(db, "SUPPLIERS")) { return FAIL; }
	if (print_table(db, "PRODUCTSUPPLIER")) { return FAIL; }
	if (print_table(db, "ORDERS")) { return FAIL; }
	return SUCCESS;
}

int main(void) {
	sqlite3 *db;
	const int rc = sqlite3_open("data.db", &db);
	if (rc != SQLITE_OK) {
		psafe_eprintln("main: Failed to open database: error code %d", rc);
		exit(EXIT_FAILURE);
	}

	char* query = "CREATE TABLE PRODUCTS("
			      "ProductID   KEY     NOT NULL,"
	              "ProductCode CHAR(4) NOT NULL,"
	              "Quantity    INT     NOT NULL,"
	              "Price       INT     NOT NULL);";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	query = "INSERT INTO PRODUCTS (ProductID,ProductCode,Quantity,Price)"
	        "VALUES (1, 'abcd', 3, 100);"

	        "INSERT INTO PRODUCTS (ProductID,ProductCode,Quantity,Price)"
	        "VALUES (2, 'heee', 7, 40);";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	if (psafe_println("Running operation with select query on PRODUCTS:")) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}
	query = "SELECT * from PRODUCTS";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	if (psafe_println("Running operation with select query on PRODUCTS:")) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}
	query = "UPDATE PRODUCTS set Quantity = 199 where ProductID=2;"

	        "SELECT * from PRODUCTS";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	if (psafe_println("Running operation with select query on PRODUCTS:")) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}
	query = "DELETE from PRODUCTS where ProductID=1;"

	        "SELECT * from PRODUCTS";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	query = "CREATE TABLE SUPPLIERS("
	        "SupplierID KEY  NOT NULL,"
	        "Name       TEXT NOT NULL,"
	        "Phone      TEXT NOT NULL);";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	query = "INSERT INTO SUPPLIERS (SupplierID,Name,Phone)"
	        "VALUES (456, 'ET', '73828372');";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	if (print_table(db, "SUPPLIERS")) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	query = "CREATE TABLE PRODUCTSUPPLIER("
	        "ProductID  INT NOT NULL,"
	        "SupplierID INT NOT NULL);";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	query = "INSERT INTO PRODUCTSUPPLIER (ProductID,SupplierID)"
	        "VALUES (2, 456);";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	if (print_table(db, "PRODUCTSUPPLIER")) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	query = "CREATE TABLE ORDERS("
	        "OrderID   KEY NOT NULL,"
	        "ProductID INT NOT NULL,"
	        "Quantity  INT NOT NULL);";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	query = "INSERT INTO ORDERS (OrderID,ProductID,Quantity)"
	        "VALUES (777, 2, 899);"
	        "SELECT * from ORDERS";

	if (db_exec(db, query)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	if (print_all_tables(db)) {
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

	if (sqlite3_close(db)) {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

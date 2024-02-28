#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   printf("\n--- CALLBACK ---\n");
   
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

void db_select(sqlite3 *db, char *table) {
	char* errorMsg;

	// Creating new SQL statement
	char sql[200] = "SELECT * from ";

	strcat(sql, table);

	// Executing SQL statement
	if(sqlite3_exec(db, sql, callback, NULL, &errorMsg)) {
		printf("Select was unsuccessful: %s\n", errorMsg);
	}
}

void db_exec(sqlite3 *db, char *sql) {
	char* errorMsg;
	if(sqlite3_exec(db, sql, callback, NULL, &errorMsg)) {
		printf("Unsuccessful operation: %s\n", errorMsg);
	}
}

void pretty_print_all(sqlite3 *db) {
	printf("\n- - -Pretty printing all- - -\n");
	db_select(db, "PRODUCTS");
	db_select(db, "SUPPLIERS");
	db_select(db, "PRODUCTSUPPLIER");
	db_select(db, "ORDERS");
}

int main(void) {
	//data base that will be used
	sqlite3 *db;
	char *sql;
	char* errorMsg;

	//opening data base and checking for success or failure
	if(sqlite3_open("data.db", &db)) {
		printf("Cannot open database\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Data base was opened successfully\n");
	}

	//Creating the sql statement
	sql = "CREATE TABLE PRODUCTS(" \
		"ProductID		KEY		NOT NULL," \
		"ProductCode	CHAR(4)	NOT NULL," \
		"Quantity		INT		NOT NULL," \
		"Price			INT);";

	if(sqlite3_exec(db, sql, callback, NULL, &errorMsg)) {
		printf("Exec failed: %s\n", errorMsg);
		exit(EXIT_FAILURE);
	} else {
		printf("Created products table successfully\n");
	}
	
	//Creating new sql statement
	sql = "INSERT INTO PRODUCTS (ProductID,ProductCode,Quantity,Price) " \
	      "VALUES (1, 'abcd', 3, 100);" \
		  "INSERT INTO PRODUCTS (ProductID,ProductCode,Quantity,Price)"
		  "VALUES (2, 'heee', 7, 40);";

	if(sqlite3_exec(db, sql, callback, NULL, &errorMsg)) {
		printf("Insert was unsuccessful: %s\n", errorMsg);
	} else {
		printf("Product inserted successfully\n");
	}

	//Creating new SQL statement
	sql = "SELECT * from PRODUCTS";

	//Executing SQL statement
	if(sqlite3_exec(db, sql, callback, NULL, &errorMsg)) {
		printf("Select was unsuccessful: %s\n", errorMsg);
	} else {
		printf("Products selected successfully\n\n");
	}
	//Creating merged SQL statement
	sql = "UPDATE PRODUCTS set Quantity = 199 where ProductID=2; " \
	"SELECT * from PRODUCTS";

	if(sqlite3_exec(db, sql, callback, NULL, &errorMsg)) {
		printf("Update was unsuccessful: %s\n", errorMsg);
	} else {
		printf("Products updated successfully\n\n");
	}

	//create merged SQL statement
	sql = "DELETE from PRODUCTS where ProductID=1;" \
		"SELECT * from PRODUCTS";
		
	if(sqlite3_exec(db, sql, callback, NULL, &errorMsg)) {
		printf("Unsuccessful operation: %s\n", errorMsg);
	}

	sql = "CREATE TABLE SUPPLIERS(" \
		"SupplierID		KEY		NOT NULL," \
		"Name			TEXT	NOT NULL," \
		"Phone			TEXT	NOT NULL);";
	
	db_exec(db, sql);
	sql = "INSERT INTO SUPPLIERS (SupplierID,Name,Phone)" \
			"VALUES (456, 'ET', '73828372');";
	db_exec(db, sql);
	db_select(db, "SUPPLIERS");

	sql = "CREATE TABLE PRODUCTSUPPLIER(" \
		"ProductID		INT		NOT NULL," \
		"SupplierID		INT		NOT NULL);";
	
	db_exec(db, sql);

	sql = "INSERT INTO PRODUCTSUPPLIER (ProductID,SupplierID)" \
			"VALUES (2, 456);";
	db_exec(db, sql);
	db_select(db, "PRODUCTSUPPLIER");

	//creating SQL statement
	sql = "CREATE TABLE ORDERS(" \
		"OrderID			KEY		NOT NULL," \
		"ProductID			INT		NOT NULL," \
		"Quantity			INT		NOT NULL);";

	db_exec(db, sql);

	sql = "INSERT INTO ORDERS (OrderID,ProductID,Quantity)" \
			"VALUES (777, 2, 899);" \
			"SELECT * from ORDERS";

	db_exec(db, sql);
	sqlite3_close(db);
}

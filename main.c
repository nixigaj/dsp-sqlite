#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

int main(void) {
	sqlite3 *db;

	if(sqlite3_open("data.db", &db)) {
		printf("Cannot open database\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Data base was opened successfully\n");
	}

	sqlite3_close(db);

}

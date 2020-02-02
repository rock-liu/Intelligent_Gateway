#ifndef SQLITE
#define SQLITE

#include <stdio.h>
#include <string.h>
#include "/home/linux/sqlite3/install/include/sqlite3.h"

extern int sqlite(char *buf);
extern int sqlite_select(char *temp_buf);
#endif

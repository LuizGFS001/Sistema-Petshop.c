#ifndef DATABASE_H
#define DATABASE_H

#include "../sqlite/sqlite3.h"

extern sqlite3 *db;

int abrirDB();
void fecharDB();

#endif
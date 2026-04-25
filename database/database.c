#include <stdio.h>
#include "../include/cliente.h"
#include "database.h"
#include "../sqlite/sqlite3.h"

sqlite3 *db;

int abrirDB(){
    int rc = sqlite3_open("Petshop.db", &db);

    if(rc){
        printf("Erro ao abrir o banco: %s\n", sqlite3_errmsg(db));
        return 1; // se a função não abrir o banco de dados a vai retornar o printf como verdadeiro
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0); // essa execução garante que o sqlite não ignora as fks
    printf("Banco conectado com sucesso!\n");
    return 0;
}

void fecharDB(){
sqlite3_close(db);
}

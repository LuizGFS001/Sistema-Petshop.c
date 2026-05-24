#ifndef DATABASE_H
#define DATABASE_H

#include "../sqlite/sqlite3.h"

extern sqlite3 *db;

int abrirDB();
void fecharDB();
void criarTabelaServicos();
void criarTriggerEstoque();
void popularServicosEProdutos();
void obterDataAtualBR(char *buffer); 
void criarTabelaUsoBeneficios();

#endif
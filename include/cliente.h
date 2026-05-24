#ifndef CLIENTE_H
#define CLIENTE_H
#include <stdio.h>
#include <sqlite3.h>

struct cliente{
    int id;
    char nome[100]; //essa linha indica ao criar no arquivo csv que "nome" só pode ter um máximo de 50 carácteres.
    char cpf[15]; // símilar a linha superior, cpf recebe 15 digitos por motivos de usarmos dados ficticios na inserção de dados decidimos dessa forma para não exibir o dado de ninguem por engano de acordo com a LGPD
    char email[70];
    char endereco[100];
    char telefone[15];
};

int callback(void *data, int argcount, char **argvetor, char **vtColName);

void criartabelaclientes();
int cadastrarCliente();
void listarClientes();
int buscarCliente();
void alterarCliente();
void excluirCliente();
void saveClientDB(struct cliente c);
int clienteCadastrado(); 
int desejaCadastrar();
void ClienteGenerico();

#endif
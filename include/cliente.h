#ifndef CLIENTE_H //Comando de compilação condicional para verificar se a macro "CLIENTE_H" já não foi definida anteriormente
#define CLIENTE_H //Define a macro CLIENTE_H, já que no momento da escrita não tinha sido criada, ou seja a compilação checa antes se foi criado e se não foi cria a nova.

struct cliente{
    int id;
    char nome[50]; //essa linha indica ao criar no arquivo csv que "nome" só pode ter um máximo de 50 carácteres.
    char cpf[7]; // símilar a linha superior, cpf recebe apenas 7 digitos por motivos de usarmos dados ficticios na inserção de dados decidimos dessa forma para não exibir o dado de ninguem por engano de acordo com a LGPD
    char email[50];
    char endereco[100];
    char telefone[7];
};

int callback(void *data, int argcount, char **argvetor, char **vtColName);

void criartabelaclientes();
void cadastrarCliente();
void listarClientes();
void buscarCliente();
void alterarCliente();
void excluirCliente();
void saveClientDB(struct cliente c);

#endif
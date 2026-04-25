#include <stdio.h>
#include "../include/cliente.h"
#include "../database/database.h"
#include "../sqlite/sqlite3.h" //biblioteca do sqlite, de onde vem as funções que usaremos ao decorrer do codigo


// criar tabela através do sql

// função

void criartabelaclientes(){

    char *sql = 
    "CREATE TABLE IF NOT EXISTS clientes ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "nome TEXT,"
    "cpf TEXT,"
    "email TEXT,"
    "endereco TEXT,"
    "telefone TEXT);";
    //comando do sql para a criação da tabela com os parâmetros indicados por exemplo, id interger armazena apenas valores inteiros, é uma Primary key indicando que é única e autoincrement significa que novos valores irão ser sequencial (ex: 1, 2, 3, 4...)

    char *erro = 0; // variavel de erro caso ocorra algum erro durante a criação da tabela 
    int rc = sqlite3_exec(db, sql, 0, 0, &erro); //a função sqlite3_exec é a função pricipal da biblioteca do sqlite3 onde recebe como parâmetros (db = banco, sql = comando, 0 = callback(sem callback no nosso caso), dados, e o endereço da variavel de erro &erro)
    //A variavel rc significa returncode
    if(rc != SQLITE_OK){ // a variavel rc que armazena a execução do sqlite, e nessa linha vamos conferir se deu tudo certo onde se rc que recebe a execução der diferente de sqlike_ok ele nos mostrará que não funciou retornando a variavel erro que armazena o erro
        printf("Erro ao criar a tabela: %s\n", erro);
        sqlite3_free(erro);
    }
}

//salvar clientes

void saveClientDB(struct cliente c){

    char sql[800]; //definição da quantidade de caracteres para montagem das querys no sql

    sprintf(sql, //formatação da string com o comando para inserir na tabela criada acima com os parâmetros indicados abaixo correspondentes aos parâmetros na criação da tabela acima.
    "INSERT INTO clientes (nome, cpf, email, endereco, telefone)"
    " VALUES('%s','%s','%s','%s','%s');",
    c.nome,
    c.cpf,
    c.email,
    c.endereco,
    c.telefone
    );

    char *erro = 0; //variavel de erro para apontar se houve sucesso ou erro durante o salvamento no banco de dados

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao inserir cliente: %s\n", erro);
        sqlite3_free(erro);
    }
    else{
        printf("Cliente cadastro feito com sucesso!\n");
    }
}

//cadastro do cliente

void cadastrarCliente(){

    struct cliente novo; 
    
    
    printf("\nNome: ");
    scanf(" %[^\n]", novo.nome); // o espaço antes do % ignora e quebra linhas anteriores, a expressão "%[^\n] é uma função da biblioteca string.h que lê tud odentro de [] o "^" é uma negação que não permite o uso do enter. Então %[^\n] lê tudo até o usuario apertar enter.

    printf("CPF: ");
    scanf(" %[^\n]", novo.cpf);

    printf("Email: ");
    scanf(" %[^\n]", novo.email);

    printf("Endereço: ");
    scanf(" %[^\n]", novo.endereco);

    printf("Telefone: ");   
    scanf(" %[^\n]", novo.telefone);

    saveClientDB(novo);
}

//callback para o select
//a função será chamada automaticamente uma vez para cada linha retornada pela consulta
int callback(void *data, int argcount, char **argvetor, char **vtColName){ //o argcount conta a quantidade de colunas, argvetor é um vetor contendo os valores das colunas da linha atual do laço e vtColname é o vetor com nome das colunas
    
    for(int i = 0; i < argcount; i++){
        printf("%s: %s\n", //o printf vai imprimir o nome e o valor da coluna
            vtColName[i],
            argvetor[i] ? argvetor[i] : "NULL"); // <<<< operação ternaria onde se o valor dentro da tabela for nulo(não tiver nada) vai retornar a str "NULL"
    }
    printf("--------------------------\n");
    return 0;
}

//listagem de clientes

void listarClientes(){

    char *sql = "SELECT * FROM clientes"; //Query do sql para puxar toda a tabela clientes 

    char *erro = 0;

    printf("\n========== CLIENTES ==========\n");

    int rc = sqlite3_exec(db, sql, callback, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao buscar clientes: %s\n", erro);
        sqlite3_free(erro);
    }
}

//função de busca de clientes no bd
void buscarCliente(){

    char esco[12];
    int opcao;
    char valor[100];
    char sql[300];

    printf("\nBuscar cliente por:\n");
    printf("1 ID\n");
    printf("2 Nome\n");
    printf("3 Telefone\n");
    printf("4 CPF\n");

    printf("Escolha: ");
    scanf("%d",&opcao);

    while(getchar() != '\n'); // limpeza de buffer

    if(opcao == 1)       printf("Digite o ID: ");
    else if(opcao == 2)  printf("Digite o Nome: ");
    else if(opcao == 3)  printf("Digite o Telefone: ");
    else if(opcao == 4)  printf("Digite o CPF: ");
    else{ printf("Opcao invalida!\n"); return; }

    scanf("%[^\n]", valor);

    if(opcao == 1){
        sprintf(sql,
        "SELECT * FROM clientes WHERE id = %s ;",
        valor);
    }

    else if(opcao == 2){
        sprintf(sql,
        "SELECT * FROM clientes WHERE nome LIKE '%%%s%%';",
        valor);
    }

    else if(opcao == 3){
        sprintf(sql,
        "SELECT * FROM clientes WHERE telefone LIKE '%%%s%%';",
        valor);
    }
    else if(opcao == 4){
        sprintf(sql,
        "SELECT * FROM clientes WHERE cpf='%s';",
        valor);
    }

    char *errMsg = 0;

    sqlite3_exec(db, sql, callback, 0, &errMsg);
}


//alterar dados do cliente no bd

void alterarCliente(){
    int id;  //declaração de variaveis
    int opcao;
    char valor[100];

    buscarCliente(); // aqui chamamos a função de busca do cliente para retornar o id do cliente para a facilidar a identificação da id do cliente para alterar o dado

    printf("Digite o ID do cliente que deseja alterar: ");
    scanf("%d",&id);

    printf("\n=== ALTERAR CLIENTE ===\n");

    printf("1 Alterar nome\n");
    printf("2 Alterar CPF\n");
    printf("3 Alterar email\n");
    printf("4 Alterar endereco\n");
    printf("5 Alterar telefone\n");

    printf("Escolha: ");
    scanf("%d",&opcao);

    printf("Digite o novo valor: ");
    scanf(" %[^\n]", valor);

    char sql[300]; //definição da quantidade de caracteres para montagem das querys no sql

    switch(opcao){

        case 1: sprintf(sql,"UPDATE clientes SET nome='%s' WHERE id=%d;",valor,id); break;
        case 2: sprintf(sql,"UPDATE clientes SET cpf='%s' WHERE id=%d;",valor,id); break;
        case 3: sprintf(sql,"UPDATE clientes SET email='%s' WHERE id=%d;",valor,id); break;
        case 4: sprintf(sql,"UPDATE clientes SET endereco='%s' WHERE id=%d;",valor,id); break;
        case 5: sprintf(sql,"UPDATE clientes SET telefone='%s' WHERE id=%d;",valor,id); break;
        default:
            printf("Opcao invalida\n");
            return;
    }

    char *errMsg = 0; //verificação de erro

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if(rc != SQLITE_OK){
        printf("Erro SQL: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    else{
        printf("Cliente atualizado com sucesso!\n");
    }
}

//função para excluir cliente com cascade para demais relações.
void excluirCliente(){

    int id;
    char confirm;
    char sql[200];

    buscarCliente();

    printf("\nDigite o ID do cliente que deseja excluir: ");
    scanf("%d",&id);

    printf("Tem certeza que deseja excluir este cliente? (s/n): "); //confirmação de exclusão
    scanf(" %c",&confirm);

    if(confirm != 's' && confirm != 'S'){
        printf("Operacao cancelada.\n");
        return;
    }

    sprintf(sql,"DELETE FROM clientes WHERE id=%d;",id);

    char *erro = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao excluir cliente: %s\n",erro);
        sqlite3_free(erro);
    }
    else{
        printf("Cliente excluido com sucesso!\n");
    }

}
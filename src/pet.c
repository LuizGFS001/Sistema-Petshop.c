#include <stdio.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"

#include "../database/database.h"
#include "../include/pet.h"
#include "../include/cliente.h"


//cria a tabela de pets caso não exista 

void criarTabelaPets(){

    char *sql =
    "CREATE TABLE IF NOT EXISTS pets ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT," // um novo id será gerado para cada vez que um novo pet por salvo
    "nome TEXT,"
    "especie TEXT,"
    "raca TEXT,"
    "idade INTEGER,"
    "cliente_id INTEGER,"
    "FOREIGN KEY(cliente_id) REFERENCES clientes(id) ON DELETE CASCADE" // referencia a o id do cadastro do cliente
    ");";

    char *erro = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){

        printf("Erro ao criar tabela pets: %s\n", erro);
        sqlite3_free(erro);
    }
}


//salvar pet no banco de dados

void savePetDB(struct pet p){

    char sql[400];

    sprintf(sql,
    "INSERT INTO pets (nome, especie, raca, idade, cliente_id)"
    " VALUES('%s','%s','%s',%d,%d);",
    p.nome,
    p.especie,
    p.raca,
    p.idade,
    p.cliente_id
    );

    char *erro = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){

        printf("Erro ao cadastrar pet: %s\n", erro);
        sqlite3_free(erro);
    }

    else{
        printf("Pet cadastrado com sucesso!\n");
    }

}


//cadastro do pet

void cadastrarPet(){

    struct pet novo;

    printf("\n=== CADASTRO DE PET ===\n");

    printf("Nome do pet: ");
    scanf(" %[^\n]", novo.nome);

    printf("Especie: ");
    scanf(" %[^\n]", novo.especie);

    printf("Raca: ");
    scanf(" %[^\n]", novo.raca);

    printf("Idade: ");
    scanf("%d", &novo.idade);

//função para relação do id do cliente para registro do pet
    printf("\nBuscando tutor...\n");
    buscarCliente();   // função do cliente.c

    printf("\nDigite o ID do tutor deste pet: ");
    scanf("%d", &novo.cliente_id);

    savePetDB(novo);
}

//listagem de pets no banco de dados relacionados por id

void listarPets(){

    char *sql =
    "SELECT pets.id, pets.nome AS pet_nome, pets.especie, pets.raca, pets.idade, "
    "clientes.nome AS tutor "
    "FROM pets "
    "JOIN clientes ON pets.cliente_id = clientes.id;";

    char *erro = 0;

    int rc = sqlite3_exec(db, sql, callback, 0, &erro);

    if(rc != SQLITE_OK){

        printf("Erro ao listar pets: %s\n", erro);
        sqlite3_free(erro);
    }
}
    
    



//exclusão de registro de pets

void excluirPet(){

    int id;
    char confirm;
    char sql[200];

    listarPets();

    printf("\nDigite o ID do pet que deseja excluir: ");
    scanf("%d", &id);

    printf("Confirmar exclusao? (s/n): ");
    scanf(" %c", &confirm);

    if(confirm != 's' && confirm != 'S'){

        printf("Operacao cancelada.\n");
        return;

    }

    sprintf(sql,"DELETE FROM pets WHERE id=%d;",id);

    char *erro = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){

        printf("Erro ao excluir pet: %s\n", erro);
        sqlite3_free(erro);

    }
    else{

        printf("Pet excluido com sucesso!\n");

    }

}

//função básicamente identica a de busca de cliente com o diferencial que a query busca por id do pet, nome do pet e id do tutor
void buscarPet(){

    int opcao;
    char valor[100];
    char sql[400];

    printf("\nBuscar pet por:\n");
    printf("1 ID do pet\n");
    printf("2 Nome do pet\n");
    printf("3 ID do cliente (tutor)\n");
    scanf("%d",&opcao);

    while(getchar() != '\n');//limpeza de buffer


    if(opcao == 1){
        printf("Digite o ID do pet: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pets.id, pets.nome AS pet_nome, pets.especie, pets.raca, pets.idade, "
        "clientes.nome AS tutor "
        "FROM pets "
        "JOIN clientes ON pets.cliente_id = clientes.id "
        "WHERE pets.id = %s;",
        valor);
    }

    else if(opcao == 2){
        printf("Digite o ID do pet: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pets.id, pets.nome AS pet_nome, pets.especie, pets.raca, pets.idade, "
        "clientes.nome AS tutor "
        "FROM pets "
        "JOIN clientes ON pets.cliente_id = clientes.id "
        "WHERE pets.nome LIKE '%%%s%%';",
        valor);
    }

    else if(opcao == 3){
        printf("\nBuscando cliente...\n");
        buscarCliente(); //chamamos a função de busca de cliente para auxiliar o usuario a achar o id do cliente para localizar seus pets no banco de dados

        while(getchar() != '\n');

        printf("\n=== Buscar cliente ===\n");

        printf("\nDigite o ID do cliente: ");
        scanf(" %[^\n]", valor);
    

    
        sprintf(sql,
        "SELECT pets.id, pets.nome AS pet_nome, pets.especie, pets.raca, pets.idade, "
        "clientes.nome AS tutor "
        "FROM pets "
        "JOIN clientes ON pets.cliente_id = clientes.id "
        "WHERE pets.cliente_id = %s;",
        valor);
    }


    else{
        printf("Opcao invalida!\n");
        return;
    }

    char *errMsg = 0;

    sqlite3_exec(db, sql, callback, 0, &errMsg);

    if(errMsg){
        printf("Erro na busca: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

}

void alterarPet(){
    int id, opcao;
    char valor[100];
    char sql[300];

    listarPets();

    printf("Digite o ID do pet que deseja alterar: ");
    scanf("%d", &id);

    printf("\n1 Alterar nome\n");
    printf("2 Alterar especie\n");
    printf("3 Alterar raca\n");
    printf("4 Alterar idade\n");
    printf("Escolha: ");
    scanf("%d", &opcao);

    printf("Digite o novo valor: ");
    scanf(" %[^\n]", valor);

    switch(opcao){
        case 1: sprintf(sql,"UPDATE pets SET nome='%s' WHERE id=%d;",valor,id); break;
        case 2: sprintf(sql,"UPDATE pets SET especie='%s' WHERE id=%d;",valor,id); break;
        case 3: sprintf(sql,"UPDATE pets SET raca='%s' WHERE id=%d;",valor,id); break;
        case 4: sprintf(sql,"UPDATE pets SET idade=%s WHERE id=%d;",valor,id); break;
        default: printf("Opcao invalida\n"); return;
    }

    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if(rc != SQLITE_OK){
        printf("Erro SQL: %s\n", errMsg);
        sqlite3_free(errMsg);
    } else {
        printf("Pet atualizado com sucesso!\n");
    }
}
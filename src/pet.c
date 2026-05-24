#include <stdio.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"

#include "../database/database.h"
#include "../include/pet.h"
#include "../include/cliente.h"


//cria a tabela de pets caso não exista 

void criarTabelaPets(){//função para criar a tabela de pets no banco de dados, caso ela ainda não exista. A função utiliza um comando SQL para definir a estrutura da tabela, incluindo os campos id, nome, especie, raca, idade e cliente_id. O campo id é definido como chave primária e autoincremento para garantir que cada pet tenha um identificador único. O campo cliente_id é definido como chave estrangeira para garantir a integridade referencial com a tabela de clientes. Caso haja algum erro durante a criação da tabela, a função exibe uma mensagem de erro.

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

void savePetDB(struct pet p){//função para salvar um pet no banco de dados, recebendo uma estrutura de pet como parâmetro. A função constrói uma consulta SQL de inserção com os dados do pet e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que o pet foi cadastrado com sucesso.

    char sql[800];

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

static int callbackCount(void *data, int argc, char **argv, char **col){//callback para contar o número de registros retornados por uma consulta SQL, armazenando o resultado em uma variável passada por referência. A função verifica se há resultados e, se houver, converte o valor do contador para um inteiro e armazena na variável. A função retorna 0 para indicar que a execução foi bem-sucedida.
    if(argc > 0 && argv[0]){
        *(int*)data = atoi(argv[0]);
    }
    return 0;
}

void cadastrarPet(){//função para cadastrar um novo pet, solicitando ao usuário as informações do pet (nome, espécie, raça, idade e ID do cliente) e salvando os dados no banco de dados. A função inclui uma verificação para garantir que o ID do cliente fornecido exista no banco de dados antes de salvar o pet. Caso haja algum erro durante a execução da consulta SQL, a função exibe uma mensagem de erro; caso contrário, confirma que o pet foi cadastrado com sucesso.

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
    while(getchar() != '\n'); // limpa o buffer após ler int

//função para relação do id do cliente para registro do pet
    printf("\nBuscando tutor...\n");
    buscarCliente();   // função do cliente.c
    while(getchar() != '\n'); // limpa buffer após buscarCliente

    while(1){
        printf("\nDigite o ID do tutor deste pet (0 para cancelar): ");
        scanf("%d", &novo.cliente_id);

        if(novo.cliente_id == 0){
            printf("Operacao cancelada.\n");
            return;
        }

        char sql_verifica[200];
        char *erro_v = 0;
        int existe = 0;
        sprintf(sql_verifica, "SELECT COUNT(*) FROM clientes WHERE id=%d;", novo.cliente_id);
        sqlite3_exec(db, sql_verifica, callbackCount, &existe, &erro_v);

        if(existe){
            savePetDB(novo);
            return;
        } else {
            printf("Cliente ID %d nao encontrado! Tente outro ID ou 0 para cancelar.\n", novo.cliente_id);
        }
    }
    printf("\nDigite o ID do tutor deste pet: ");
    scanf("%d", &novo.cliente_id);

    savePetDB(novo);
}

void listarPets(){//função para listar os pets cadastrados no banco de dados, executando uma consulta SQL para selecionar todos os registros da tabela de pets e exibindo os resultados. A função utiliza um callback para processar os resultados da consulta e exibir as informações de cada pet, incluindo o nome do pet, a espécie, a raça, a idade e o nome do tutor (cliente). Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.

    char *sql =
    "SELECT pets.id, pets.nome AS pet_nome, pets.especie, pets.raca, pets.idade, "
    "clientes.nome AS tutor "
    "FROM pets "
    "JOIN clientes ON pets.cliente_id = clientes.id;";

    char *erro = 0;

    printf("\n========== PETS ==========\n");
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);

    if(rc != SQLITE_OK){

        printf("Erro ao listar pets: %s\n", erro);
        sqlite3_free(erro);
    }
}
    
void excluirPet(){//função para excluir um pet do banco de dados, solicitando ao usuário o ID do pet que deseja excluir. A função inclui uma confirmação de exclusão para evitar exclusões acidentais. Se o usuário confirmar a exclusão, a função constrói uma consulta SQL de exclusão e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que o pet foi excluído com sucesso.

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

void buscarPet(){//função para buscar pets no banco de dados com base em diferentes critérios, como ID do pet, nome do pet ou ID do cliente (tutor). A função solicita ao usuário o critério de busca e o valor correspondente, constrói uma consulta SQL com base na escolha do usuário e executa a consulta usando sqlite3_exec. A função utiliza um callback para processar os resultados da consulta e exibir as informações dos pets encontrados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.

    int opcao;
    char valor[100];
    char sql[400];

    printf("\nBuscar pet por:\n");
    printf("1 ID do pet\n");
    printf("2 Nome do pet\n");
    printf("3 ID do cliente (tutor)\n");
    printf("0 Cancelar\n");
    scanf("%d",&opcao);
    if(opcao == 0){ printf("Operacao cancelada.\n"); return; }

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

void alterarPet(){//função para alterar as informações de um pet no banco de dados, solicitando ao usuário o ID do pet que deseja alterar e o campo que deseja modificar (nome, espécie, raça ou idade). A função constrói uma consulta SQL de atualização com base na escolha do usuário e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que o pet foi atualizado com sucesso.
    int id, opcao;
    char valor[100];
    char sql[300];

    listarPets();

    printf("Digite o ID do pet que deseja alterar (0 = cancelar): ");
    scanf("%d", &id);
    if(id == 0){ printf("Operacao cancelada.\n"); return; }

    printf("\n1 Alterar nome\n");
    printf("2 Alterar especie\n");
    printf("3 Alterar raca\n");
    printf("4 Alterar idade\n");
    printf("0 Cancelar\n");
    printf("Escolha: ");
    scanf("%d", &opcao);
    if(opcao == 0){ printf("Operacao cancelada.\n"); return; }

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

void PetGenerico(int cliente_id){
    char sql[300];
    sprintf(sql,
    "INSERT OR IGNORE INTO pets (id, nome, especie, raca, idade, cliente_id) "
    "VALUES (999, 'Pet Avulso', 'Nao informado', 'Nao informado', 0, %d);",
    cliente_id);
    char *erro = 0;
    sqlite3_exec(db, sql, 0, 0, &erro);
    if(erro){ sqlite3_free(erro); }
}

void cadastrarPetAtendimento(int cliente_id){
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
    while(getchar() != '\n');
    novo.cliente_id = cliente_id;
    savePetDB(novo);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cliente.h"
#include "../include/assinatura.h"
#include "../database/database.h"
#include "../sqlite/sqlite3.h"

typedef struct {
    int cliente_id;
    int pet_id;
    char plano[50];
    float valor;
    char data_inicio[50];
    char data_renovacao[50];
    char status[20];
    int pontos;
} assinatura;



void criarTabelaAssinatura() {//função para criar a tabela de assinaturas no banco de dados, caso ela ainda não exista. A função utiliza um comando SQL para definir a estrutura da tabela, incluindo os campos id, cliente_id, pet_id, plano, valor, data_inicio, data_renovacao, status e pontos. Os campos cliente_id e pet_id são definidos como chaves estrangeiras que referenciam as tabelas de clientes e pets, respectivamente. A função também inclui tratamento de erros para informar caso haja algum problema durante a criação da tabela.
    char *sql =

    "CREATE TABLE IF NOT EXISTS assinatura ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "cliente_id INTEGER,"
    "pet_id INTEGER,"
    "plano TEXT,"
    "valor REAL,"
    "data_inicio TEXT,"
    "data_renovacao TEXT,"
    "status TEXT,"
    "pontos INTEGER DEFAULT 0,"
    "FOREIGN KEY(cliente_id) REFERENCES clientes(id),"
    "FOREIGN KEY(pet_id) REFERENCES pets(id)"
    ");";

    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if (rc != SQLITE_OK) {
        printf("Erro tabela assinatura: %s\n", erro);
        sqlite3_free(erro);
    }
}

static int callbackAssinatura(//callback para exibir os dados das assinaturas, formatando a saída de forma legível. A função percorre os resultados da consulta SQL e imprime cada campo da assinatura, incluindo o ID, nome do cliente, nome do pet, plano, valor, data de início, data de renovação, status e pontos acumulados. A função retorna 0 para indicar que a execução foi bem-sucedida.
    void *data,
    int argc,
    char **argv,
    char **col
) {
    for (int i = 0; i < argc; i++) {
        printf(
            "%s : %s\n",
            col[i],
            argv[i] ? argv[i] : "NULL"
        );
    }
    printf("\n");
    return 0;
}

void cadastrarAssinatura() {//função para cadastrar uma nova assinatura, solicitando ao usuário as informações necessárias e salvando os dados no banco de dados. A função coleta o ID do cliente, o ID do pet, o plano escolhido, o valor da assinatura, as datas de início e renovação, e o status da assinatura. Em seguida, a função constrói uma consulta SQL de inserção com os dados da assinatura e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que a assinatura foi criada com sucesso.

    struct assinatura a;
    int op;

    printf("\n=== NOVA ASSINATURA ===\n");

    printf("ID cliente: ");
    scanf("%d", &a.cliente_id);

    printf("ID pet: ");
    scanf("%d", &a.pet_id);

    printf("\nPLANOS\n");
    printf("1 - PATINHAS\n");
    printf("2 - AMIGO PET\n");
    printf("3 - VIDA ANIMAL\n");
    printf("Escolha: ");
    scanf("%d", &op);

    switch(op) {

        case 1:
            strcpy(a.plano, "PATINHAS");
            a.valor = 39.90;
        break;

        case 2:
            strcpy(a.plano, "AMIGO PET");
            a.valor = 79.90;
        break;

        case 3:
            strcpy(a.plano, "VIDA ANIMAL");
            a.valor = 149.90;
        break;

        default:
            printf("Plano invalido\n");
            return;
    }

    printf("Data inicio: ");
    scanf(" %[^\n]", a.data_inicio);
    printf("Data renovacao: ");
    scanf(" %[^\n]", a.data_renovacao);
    strcpy(a.status, "ATIVA");
    a.pontos = 0;

    char sql[1000];
    sprintf(
        sql,
        "INSERT INTO assinatura "
        "(cliente_id, pet_id, plano, valor, data_inicio, data_renovacao, status, pontos) "
        "VALUES "
        "(%d,%d,'%s',%.2f,'%s','%s','%s',%d);",
        a.cliente_id,
        a.pet_id,
        a.plano,
        a.valor,
        a.data_inicio,
        a.data_renovacao,
        a.status,
        a.pontos
    );

    char *erro = 0;

    sqlite3_exec(
        db,
        sql,
        0,
        0,
        &erro
    );

    if (erro) {

        printf("Erro: %s\n", erro);

        sqlite3_free(erro);

    } else {

        printf("Assinatura criada com sucesso\n");
    }
}

void listarAssinaturas() {//função para listar todas as assinaturas cadastradas, exibindo os detalhes de cada assinatura. A função executa uma consulta SQL para selecionar todas as assinaturas da tabela e utiliza o callback callbackAssinatura para formatar e exibir os resultados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.

    char *sql =
    "SELECT * FROM assinatura;";
    char *erro = 0;

    sqlite3_exec(
        db,
        sql,
        callbackAssinatura,
        0,
        &erro
    );

    if (erro) {
        printf("Erro: %s\n", erro);
        sqlite3_free(erro);
    }
}

void buscarAssinatura() {//função para buscar assinaturas no banco de dados, permitindo ao usuário inserir o ID do cliente para filtrar as assinaturas. A função constrói uma consulta SQL com base no ID do cliente fornecido e executa a consulta usando sqlite3_exec, utilizando o callback callbackAssinatura para processar os resultados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.

    int cliente;

    printf("Cliente ID: ");
    scanf("%d", &cliente);

    char sql[300];

    sprintf(

        sql,
        "SELECT * FROM assinatura "
        "WHERE cliente_id=%d;",
        cliente
    );

    sqlite3_exec(
        db,
        sql,
        callbackAssinatura,
        0,
        0
    );
}

void renovarAssinatura() {//função para renovar uma assinatura, solicitando ao usuário o ID da assinatura e a nova data de renovação. A função constrói uma consulta SQL de atualização para modificar a data de renovação da assinatura correspondente ao ID fornecido e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que a assinatura foi renovada com sucesso.

    int id;
    char data[20];

    listarAssinaturas();
    printf("ID assinatura: ");
    scanf("%d", &id);
    printf("Nova renovacao: ");
    scanf(" %[^\n]", data);

    char sql[300];

    sprintf(

        sql,
        "UPDATE assinatura "
        "SET data_renovacao='%s' "
        "WHERE id=%d;",
        data,
        id
    );

    sqlite3_exec(
        db,
        sql,
        0,
        0,
        0
    );
    printf("Renovada\n");
}

void cancelarAssinatura() {//função para cancelar uma assinatura, solicitando ao usuário o ID da assinatura a ser cancelada. A função constrói uma consulta SQL de atualização para modificar o status da assinatura correspondente ao ID fornecido e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que a assinatura foi cancelada com sucesso.

    int id;

    listarAssinaturas();
    printf("ID assinatura: ");
    scanf("%d", &id);

    char sql[300];

    sprintf(

        sql,
        "UPDATE assinatura "
        "SET status='CANCELADA' "
        "WHERE id=%d;",
        id
    );

    sqlite3_exec(
        db,
        sql,
        0,
        0,
        0
    );
    printf("Cancelada\n");
}

void adicionarPontos(//função para adicionar pontos a uma assinatura, recebendo o ID do cliente e a quantidade de pontos a ser adicionada. A função constrói uma consulta SQL de atualização para incrementar os pontos da assinatura correspondente ao ID do cliente fornecido e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, os pontos são adicionados com sucesso.
    int cliente_id,
    int pontos
) {
    char sql[300];
    sprintf(

        sql,
        "UPDATE assinatura "
        "SET pontos = pontos + %d "
        "WHERE cliente_id=%d;",
        pontos,
        cliente_id
    );

    sqlite3_exec(
        db,
        sql,
        0,
        0,
        0
    );
}

void verificarAssinatura() {//função para verificar o status de uma assinatura, solicitando ao usuário o ID do cliente. A função constrói uma consulta SQL para selecionar a assinatura correspondente ao ID do cliente fornecido e executa a consulta usando sqlite3_exec, utilizando o callback callbackAssinatura para processar os resultados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, exibe os detalhes da assinatura do cliente.
    int cliente_id;
    char sql[300];
    char *erro = 0;

    printf("\n=== VERIFICAR BENEFÍCIOS DO CLIENTE ===\n");
    buscarCliente();

    printf("\nDigite o ID do cliente para ver a Assinatura: ");
    scanf("%d", &cliente_id);

    sprintf(
        sql,
        "SELECT plano, valor, status, pontos FROM assinatura WHERE cliente_id=%d;",
        cliente_id
    );

    printf("\n--- Detalhes da Assinatura ---\n");

    sqlite3_exec(
        db,
        sql,
        callbackAssinatura,
        0,
        &erro
    );

    if (erro) {
        printf("Erro ao verificar assinatura: %s\n", erro);
        sqlite3_free(erro);
    }
}


void relatorioAssinaturas() {//função para gerar um relatório de assinaturas, agrupando as assinaturas por plano e contando o total de assinaturas para cada plano. A função constrói uma consulta SQL que utiliza a cláusula GROUP BY para agrupar as assinaturas pelo campo plano e a função de agregação COUNT para contar o número de assinaturas em cada grupo. A consulta é executada usando sqlite3_exec, utilizando o callback callbackAssinatura para processar os resultados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, exibe o relatório com o total de assinaturas por plano.

    char *sql =
    "SELECT plano, COUNT(*) AS total "
    "FROM assinatura "
    "GROUP BY plano;";

    sqlite3_exec(
        db,
        sql,
        callbackAssinatura,
        0,
        0
    );
}
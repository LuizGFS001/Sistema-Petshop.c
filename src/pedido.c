#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"
#include "database.h"
#include "pedido.h"
#include "cliente.h"
#include "pet.h"
#include "servico.h"
#include "itens_pedido.h"

void criarTabelaPedidos(){
    char *sql =
    "CREATE TABLE IF NOT EXISTS pedidos ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "cliente_id INTEGER,"
    "data TEXT,"
    "total REAL,"
    "status TEXT,"
    "FOREIGN KEY(cliente_id) REFERENCES clientes(id) ON DELETE CASCADE);";
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar tabela pedidos: %s\n", erro); sqlite3_free(erro); }
}

int OrdemServico(int id_cliente, char *data_atual) {
    struct pedido novo;

    printf("\n=== NOVO PEDIDO ===\n");
    
    novo.cliente_id = id_cliente;
    strcpy(novo.data, data_atual);
    novo.total = 0.0;
    strcpy(novo.status, "aberto");

    char sql[400];
    sprintf(sql,
            "INSERT INTO pedidos (cliente_id, data, total, status)"
            " VALUES(%d, '%s', %.2f, '%s');",
            novo.cliente_id, novo.data, novo.total, novo.status);

    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK) { 
        printf("Erro ao criar pedido: %s\n", erro); 
        sqlite3_free(erro); 
        return 0; 
    }

    int pedido_id = (int)sqlite3_last_insert_rowid(db);
    printf("Pedido #%d criado!\n", pedido_id);

    char continuar = 's';
    while(continuar == 's' || continuar == 'S'){
        adicionarItemPedido(pedido_id, id_cliente);
        
        printf("\nAdicionar outro item? (s/n): ");
        scanf(" %c", &continuar);
    }

    printf("\n=== ITENS DO PEDIDO #%d ===\n", pedido_id);
    listarItensPedido(pedido_id);

    char sql_total[200];
    sprintf(sql_total, "SELECT total FROM pedidos WHERE id=%d;", pedido_id);
    sqlite3_exec(db, sql_total, callback, 0, 0);
    return pedido_id;
}

void listarPedidos(){//função para listar os pedidos cadastrados no banco de dados, executando uma consulta SQL para selecionar todos os registros da tabela de pedidos e exibindo os resultados. A função utiliza um callback para processar os resultados da consulta e exibir as informações de cada pedido, incluindo o nome do cliente associado ao pedido. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.
    char *sql =
    "SELECT pedidos.id, clientes.nome AS cliente, pedidos.data, pedidos.total, pedidos.status "
    "FROM pedidos "
    "JOIN clientes ON pedidos.cliente_id = clientes.id;";
    char *erro = 0;
    printf("\n========== PEDIDOS ==========\n");
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar pedidos: %s\n", erro); sqlite3_free(erro); }
}

void buscarPedido(){//função para buscar pedidos no banco de dados com base em diferentes critérios, como ID do pedido, ID do cliente ou status do pedido. A função solicita ao usuário o critério de busca e o valor correspondente, constrói uma consulta SQL com base na escolha do usuário e executa a consulta usando sqlite3_exec. A função utiliza um callback para processar os resultados da consulta e exibir as informações dos pedidos encontrados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.
    int opcao;
    char valor[100];
    char sql[400];

    printf("\nBuscar pedido por:\n");
    printf("1 ID\n");
    printf("2 Cliente ID\n");
    printf("3 Status\n");
    printf("Escolha: ");
    scanf("%d", &opcao);
    while(getchar() != '\n');

    if(opcao == 1){
        printf("Digite o ID do pedido: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT * FROM pedidos WHERE id=%s;", valor);
    }
    else if(opcao == 2){
        printf("Digite o ID do cliente: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT * FROM pedidos WHERE cliente_id=%s;", valor);
    }
    else if(opcao == 3){
        printf("Digite o status (aberto/fechado): ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT * FROM pedidos WHERE status='%s';", valor);
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

void alterarPedido(){//função para alterar informações de um pedido existente no banco de dados. A função solicita ao usuário o ID do pedido que deseja alterar e as novas informações, constrói uma consulta SQL para atualizar o registro e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.
    int id, opcao;
    char valor[100];
    char sql[300];
    listarPedidos();
    printf("Digite o ID do pedido que deseja alterar: ");
    scanf("%d", &id);
    printf("\n1 Alterar status\n");
    printf("2 Alterar data\n");
    printf("Escolha: ");
    scanf("%d", &opcao);
    printf("Digite o novo valor: ");
    scanf(" %[^\n]", valor);
    switch(opcao){
        case 1: sprintf(sql, "UPDATE pedidos SET status='%s' WHERE id=%d;", valor, id); break;
        case 2: sprintf(sql, "UPDATE pedidos SET data='%s' WHERE id=%d;", valor, id); break;
        default: printf("Opcao invalida\n"); return;
    }
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if(rc != SQLITE_OK){ printf("Erro SQL: %s\n", errMsg); sqlite3_free(errMsg); }
    else{ printf("Pedido atualizado com sucesso!\n"); }
}

void excluirPedido(){//função para excluir um pedido do banco de dados com base em seu ID. A função solicita ao usuário o ID do pedido que deseja excluir, confirma a operação e executa uma consulta SQL para remover o registro. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.
    int id;
    char confirm;
    char sql[200];
    listarPedidos();
    printf("\nDigite o ID do pedido que deseja excluir: ");
    scanf("%d", &id);
    printf("Confirmar exclusao? (s/n): ");
    scanf(" %c", &confirm);
    if(confirm != 's' && confirm != 'S'){ printf("Operacao cancelada.\n"); return; }
    sprintf(sql, "DELETE FROM pedidos WHERE id=%d;", id);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao excluir pedido: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Pedido excluido com sucesso!\n"); }
}
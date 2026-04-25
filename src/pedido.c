#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"
#include "../database/database.h"
#include "../include/pedido.h"
#include "../include/cliente.h"
#include "../include/pet.h"
#include "../include/servico.h"
#include "../include/itens_pedido.h"

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

void cadastrarPedido(){
    struct pedido novo;
    printf("\n=== NOVO PEDIDO ===\n");

    printf("\nBuscando cliente...\n");
    buscarCliente();
    printf("\nDigite o ID do cliente: ");
    scanf("%d", &novo.cliente_id);

    // data atual no formato YYYY-MM-DD para salvar na tabela
    printf("Data (YYYY-MM-DD): ");
    scanf(" %[^\n]", novo.data);

    novo.total = 0.0;
    strcpy(novo.status, "aberto");

    char sql[400];
    sprintf(sql,
    "INSERT INTO pedidos (cliente_id, data, total, status)"
    " VALUES(%d, '%s', %.2f, '%s');",
    novo.cliente_id, novo.data, novo.total, novo.status);

    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar pedido: %s\n", erro); sqlite3_free(erro); return; }

    // pega o id do pedido recem criado
    int pedido_id = (int)sqlite3_last_insert_rowid(db);
    printf("Pedido #%d criado!\n", pedido_id);

    // adiciona itens ao pedido
    char continuar = 's';
    while(continuar == 's' || continuar == 'S'){
        adicionarItemPedido(pedido_id);
        printf("\nAdicionar outro item? (s/n): ");
        scanf(" %c", &continuar);
    }

    printf("\n=== ITENS DO PEDIDO #%d ===\n", pedido_id);
    listarItensPedido(pedido_id);

    // exibe total atualizado
    char sql_total[200];
    sprintf(sql_total, "SELECT total FROM pedidos WHERE id=%d;", pedido_id);
    sqlite3_exec(db, sql_total, callback, 0, 0);
}

void listarPedidos(){
    char *sql =
    "SELECT pedidos.id, clientes.nome AS cliente, pedidos.data, pedidos.total, pedidos.status "
    "FROM pedidos "
    "JOIN clientes ON pedidos.cliente_id = clientes.id;";
    char *erro = 0;
    printf("\n========== PEDIDOS ==========\n");
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar pedidos: %s\n", erro); sqlite3_free(erro); }
}

void buscarPedido(){
    int opcao;
    char valor[100];
    char sql[400];
    printf("\nBuscar pedido por:\n");
    printf("1 ID do pedido\n");
    printf("2 ID do cliente\n");
    printf("3 Status\n");
    scanf("%d", &opcao);
    while(getchar() != '\n');
    if(opcao == 1){
        printf("Digite o ID do pedido: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pedidos.id, clientes.nome AS cliente, pedidos.data, pedidos.total, pedidos.status "
        "FROM pedidos JOIN clientes ON pedidos.cliente_id = clientes.id "
        "WHERE pedidos.id = %s;", valor);
    }
    else if(opcao == 2){
        printf("\nBuscando cliente...\n");
        buscarCliente();
        while(getchar() != '\n');
        printf("\nDigite o ID do cliente: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pedidos.id, clientes.nome AS cliente, pedidos.data, pedidos.total, pedidos.status "
        "FROM pedidos JOIN clientes ON pedidos.cliente_id = clientes.id "
        "WHERE pedidos.cliente_id = %s;", valor);
    }
    else if(opcao == 3){
        printf("Digite o status (aberto/fechado): ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pedidos.id, clientes.nome AS cliente, pedidos.data, pedidos.total, pedidos.status "
        "FROM pedidos JOIN clientes ON pedidos.cliente_id = clientes.id "
        "WHERE pedidos.status = '%s';", valor);
    }
    else{ printf("Opcao invalida!\n"); return; }
    char *errMsg = 0;
    sqlite3_exec(db, sql, callback, 0, &errMsg);
    if(errMsg){ printf("Erro na busca: %s\n", errMsg); sqlite3_free(errMsg); }
}

void alterarPedido(){
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

void excluirPedido(){
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
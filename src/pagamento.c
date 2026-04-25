#include <stdio.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"
#include "../database/database.h"
#include "../include/pagamento.h"
#include "../include/cliente.h"
#include "../include/pedido.h"

void criarTabelaPagamentos(){
    char *sql =
    "CREATE TABLE IF NOT EXISTS pagamentos ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "pedido_id INTEGER,"
    "valor REAL,"
    "forma TEXT,"
    "data TEXT,"
    "FOREIGN KEY(pedido_id) REFERENCES pedidos(id) ON DELETE CASCADE);";
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar tabela pagamentos: %s\n", erro); sqlite3_free(erro); }
}

void savePagamentoDB(struct pagamento p){
    char sql[400];
    sprintf(sql,
    "INSERT INTO pagamentos (pedido_id, valor, forma, data)"
    " VALUES(%d, %.2f, '%s', '%s');",
    p.pedido_id, p.valor, p.forma, p.data);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao registrar pagamento: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Pagamento registrado com sucesso!\n"); }
}

void cadastrarPagamento(){
    struct pagamento novo;

    printf("\n=== REGISTRAR PAGAMENTO ===\n");

    printf("\nPedidos em aberto:\n");
    char *sql_abertos = 
    "SELECT pedidos.id, clientes.nome AS cliente, pedidos.total "
    "FROM pedidos "
    "JOIN clientes ON pedidos.cliente_id = clientes.id "
    "WHERE pedidos.status = 'aberto';";
    sqlite3_exec(db, sql_abertos, callback, 0, 0);

    printf("\nDigite o ID do pedido: ");
    scanf("%d", &novo.pedido_id);

    // exibe os itens do pedido selecionado
    printf("\n=== ITENS DO PEDIDO ===\n");
    listarItensPedido(novo.pedido_id);

    // busca o total do pedido automaticamente
    char sql_total[200];
    sprintf(sql_total, "SELECT total FROM pedidos WHERE id=%d;", novo.pedido_id);
    printf("Total do pedido: ");
    sqlite3_exec(db, sql_total, callback, 0, 0);

    printf("\nValor do pagamento: ");
    scanf("%f", &novo.valor);

    printf("Forma de pagamento (dinheiro/cartao/pix): ");
    scanf(" %[^\n]", novo.forma);

    printf("Data (YYYY-MM-DD): ");
    scanf(" %[^\n]", novo.data);

    savePagamentoDB(novo);

    // atualiza status do pedido para fechado
    char sql_status[200];
    sprintf(sql_status, "UPDATE pedidos SET status='fechado' WHERE id=%d;", novo.pedido_id);
    sqlite3_exec(db, sql_status, 0, 0, 0);
    printf("Pedido #%d fechado!\n", novo.pedido_id);
}

void listarPagamentos(){
    char *sql =
    "SELECT pagamentos.id, clientes.nome AS cliente, pagamentos.valor, "
    "pagamentos.forma, pagamentos.data "
    "FROM pagamentos "
    "JOIN pedidos ON pagamentos.pedido_id = pedidos.id "
    "JOIN clientes ON pedidos.cliente_id = clientes.id;";
    char *erro = 0;
    printf("\n========== PAGAMENTOS ==========\n");
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar pagamentos: %s\n", erro); sqlite3_free(erro); }
}

void buscarPagamento(){
    int opcao;
    char valor[100];
    char sql[400];

    printf("\nBuscar pagamento por:\n");
    printf("1 ID do pagamento\n");
    printf("2 ID do pedido\n");
    printf("3 Forma de pagamento\n");
    scanf("%d", &opcao);
    while(getchar() != '\n');

    if(opcao == 1){
        printf("Digite o ID do pagamento: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pagamentos.id, clientes.nome AS cliente, pagamentos.valor, "
        "pagamentos.forma, pagamentos.data "
        "FROM pagamentos "
        "JOIN pedidos ON pagamentos.pedido_id = pedidos.id "
        "JOIN clientes ON pedidos.cliente_id = clientes.id "
        "WHERE pagamentos.id = %s;", valor);
    }
    else if(opcao == 2){
        printf("Digite o ID do pedido: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pagamentos.id, clientes.nome AS cliente, pagamentos.valor, "
        "pagamentos.forma, pagamentos.data "
        "FROM pagamentos "
        "JOIN pedidos ON pagamentos.pedido_id = pedidos.id "
        "JOIN clientes ON pedidos.cliente_id = clientes.id "
        "WHERE pagamentos.pedido_id = %s;", valor);
    }
    else if(opcao == 3){
        printf("Forma (dinheiro/cartao/pix): ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT pagamentos.id, clientes.nome AS cliente, pagamentos.valor, "
        "pagamentos.forma, pagamentos.data "
        "FROM pagamentos "
        "JOIN pedidos ON pagamentos.pedido_id = pedidos.id "
        "JOIN clientes ON pedidos.cliente_id = clientes.id "
        "WHERE pagamentos.forma = '%s';", valor);
    }
    else{ printf("Opcao invalida!\n"); return; }

    char *errMsg = 0;
    sqlite3_exec(db, sql, callback, 0, &errMsg);
    if(errMsg){ printf("Erro na busca: %s\n", errMsg); sqlite3_free(errMsg); }
}

void excluirPagamento(){
    int id;
    char confirm;
    char sql[200];

    listarPagamentos();

    printf("\nDigite o ID do pagamento que deseja excluir: ");
    scanf("%d", &id);

    printf("Confirmar exclusao? (s/n): ");
    scanf(" %c", &confirm);
    if(confirm != 's' && confirm != 'S'){ printf("Operacao cancelada.\n"); return; }

    // reabre o pedido ao excluir o pagamento
    char sql_pedido[200];
    sprintf(sql_pedido,
    "UPDATE pedidos SET status='aberto' WHERE id=("
    "SELECT pedido_id FROM pagamentos WHERE id=%d);", id);
    sqlite3_exec(db, sql_pedido, 0, 0, 0);

    sprintf(sql, "DELETE FROM pagamentos WHERE id=%d;", id);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao excluir pagamento: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Pagamento excluido e pedido reaberto!\n"); }
}
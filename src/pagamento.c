#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../sqlite/sqlite3.h"
#include "database.h"
#include "../include/assinatura.h"
#include "../include/pagamento.h"
#include "../include/cliente.h"
#include "../include/pedido.h"
#include "../include/itens_pedido.h"

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
    char sql[800];
    sprintf(sql,
    "INSERT INTO pagamentos (pedido_id, valor, forma, data)"
    " VALUES(%d, %.2f, '%s', '%s');",
    p.pedido_id, p.valor, p.forma, p.data);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao registrar pagamento: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Pagamento registrado com sucesso!\n"); }
}

static int callbackIDPagamento(void *data, int argc, char **argv, char **col){
    if(argc > 0 && argv[0]){
        *(int*)data = atoi(argv[0]);
    }
    return 0;
}

static int callbackTotal(void *data, int argc, char **argv, char **col){
    if(argc > 0 && argv[0]){
        *(float*)data = atof(argv[0]);
    }
    return 0;
}

static int cbPlano(void *data, int argc, char **argv, char **col){
    if(argc > 0 && argv[0]) strcpy((char*)data, argv[0]);
    return 0;
}


void emitirResumo(int pedido_id){
    printf("\n=========== RESUMO DO PEDIDO ===========\n");
    printf("Pedido #%d\n", pedido_id);

    listarItensPedido(pedido_id);

    char sql_total[200];
    sprintf(sql_total, "SELECT total FROM pedidos WHERE id=%d;", pedido_id);

    printf("\nTotal do pedido:\n");
    sqlite3_exec(db, sql_total, callback, 0, 0);

    printf("========================================\n");
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

int formaPagamento(){
    int opcao;

    printf("\n=== FORMA DE PAGAMENTO ===\n");
    printf("1 - Dinheiro\n");
    printf("2 - Cartao\n");
    printf("3 - Pix\n");
    printf("Escolha: ");
    scanf("%d", &opcao);

    while(opcao < 1 || opcao > 3){
        printf("Opcao invalida. Tente novamente: ");
        scanf("%d", &opcao);
    }

    return opcao;
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

void cadastrarPagamento(int pedido_id){
    struct pagamento novo;

    printf("\n=== PROCESSANDO PAGAMENTO ===\n");

    novo.pedido_id = pedido_id;
    printf("Pedido atual: #%d\n", pedido_id);

    float total = 0;
    char sql_total[200];
    sprintf(sql_total, "SELECT total FROM pedidos WHERE id=%d;", pedido_id);
    sqlite3_exec(db, sql_total, callbackTotal, &total, 0);

    printf("Total calculado automaticamente: R$ %.2f\n", total);

    // busca cliente do pedido
    int cli_id = 0;
    char sql_cli[200];
    sprintf(sql_cli, "SELECT cliente_id FROM pedidos WHERE id=%d;", pedido_id);
    sqlite3_exec(db, sql_cli, callbackIDPagamento, &cli_id, 0);

    // aplica desconto do plano se cliente tiver assinatura ativa
    if(cli_id > 0 && cli_id != 999){
        char plano[50] = "";
        char sql_plano[300];
        sprintf(sql_plano,
            "SELECT plano FROM assinatura "
            "WHERE cliente_id=%d AND status='ATIVA';",
            cli_id);
        sqlite3_exec(db, sql_plano, cbPlano, plano, 0);

        float desconto = 0.0;
        if(strcmp(plano, "PATINHAS") == 0)        desconto = 0.05;
        else if(strcmp(plano, "AMIGO PET") == 0)  desconto = 0.10;
        else if(strcmp(plano, "VIDA ANIMAL") == 0) desconto = 0.15;

        if(desconto > 0){
            float valor_desc = total * desconto;
            printf("Plano %s: %.0f%% de desconto!\n", plano, desconto * 100);
            printf("Desconto: R$ %.2f\n", valor_desc);
            total = total - valor_desc;
            printf("Total com desconto: R$ %.2f\n", total);
        }
    }

    novo.valor = total;

    int op = formaPagamento();
    if(op == 1) strcpy(novo.forma, "dinheiro");
    else if(op == 2) strcpy(novo.forma, "cartao");
    else strcpy(novo.forma, "pix");

    obterDataAtualBR(novo.data);
    printf("Data do sistema aplicada: %s\n", novo.data);

    savePagamentoDB(novo);

    // acumula pontos na assinatura do cliente
    if(cli_id > 0 && cli_id != 999){
        int pontos = (int)novo.valor;
        adicionarPontos(cli_id, pontos);
        printf("+ %d pontos adicionados a assinatura do cliente!\n", pontos);
    }

    char sql_status[200];
    sprintf(sql_status, "UPDATE pedidos SET status='fechado' WHERE id=%d;", pedido_id);
    sqlite3_exec(db, sql_status, 0, 0, 0);

    printf("Pedido #%d finalizado com sucesso!\n", pedido_id);
}
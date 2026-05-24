#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"
#include "../database/database.h"
#include "../include/agendamento.h"
#include "../include/cliente.h"
#include "../include/pagamento.h"

void criarTabelaAgendamentos() {
    char *sql =
    "CREATE TABLE IF NOT EXISTS agendamentos ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "cliente_id INTEGER,"
    "pet_id INTEGER,"
    "servico_id INTEGER,"
    "data TEXT,"
    "horario TEXT,"
    "status TEXT DEFAULT 'AGENDADO',"
    "observacao TEXT,"
    "FOREIGN KEY(cliente_id) REFERENCES clientes(id),"
    "FOREIGN KEY(pet_id) REFERENCES pets(id),"
    "FOREIGN KEY(servico_id) REFERENCES servicos(id));";
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar tabela agendamentos: %s\n", erro); sqlite3_free(erro); }
}

// exibe agendamentos formatados com nome do cliente, pet e servico
static int callbackAgendamento(void *data, int argc, char **argv, char **col) {
    for(int i = 0; i < argc; i++){
        printf("%s: %s\n", col[i], argv[i] ? argv[i] : "NULL");
    }
    printf("--------------------------\n");
    return 0;
}

void criarAgendamento() {
    printf("\n=== NOVO AGENDAMENTO ===\n");

    printf("\nBuscando cliente...\n");
    int cliente_id = buscarCliente();
    if(cliente_id <= 0 || cliente_id == 999){
        printf("Agendamento requer cliente cadastrado.\n");
        return;
    }

    char sql_pets[300];
    sprintf(sql_pets,
        "SELECT pets.id, pets.nome, pets.especie FROM pets WHERE cliente_id = %d;",
        cliente_id);
    printf("\nPets do cliente:\n");
    sqlite3_exec(db, sql_pets, callback, 0, 0);

    int pet_id;
    printf("ID do pet (0 = sem pet especifico): ");
    scanf("%d", &pet_id);
    if(pet_id == 0) pet_id = 999;

    printf("\n--- SERVICOS DISPONIVEIS ---\n");
    char *sql_serv = "SELECT id, nome, preco FROM servicos WHERE estoque = -1 ORDER BY nome;";
    sqlite3_exec(db, sql_serv, callback, 0, 0);
    printf("----------------------------\n");

    int servico_id;
    printf("ID do servico: ");
    scanf("%d", &servico_id);

    // valida que e um servico (estoque=-1), nao um produto
    char sql_val[200];
    sprintf(sql_val, "SELECT COUNT(*) FROM servicos WHERE id=%d AND estoque=-1;", servico_id);
    sqlite3_stmt *stmt;
    int existe = 0;
    if(sqlite3_prepare_v2(db, sql_val, -1, &stmt, 0) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW) existe = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if(!existe){ printf("ID invalido ou item selecionado e um produto, nao um servico.\n"); return; }

    char data[20], horario[10], observacao[200];
    printf("Data (DD-MM-YYYY): ");
    scanf(" %[^\n]", data);
    printf("Horario (HH:MM): ");
    scanf(" %[^\n]", horario);

    printf("Observacao (ENTER para pular): ");
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF);
    observacao[0] = '\0';
    if(fgets(observacao, sizeof(observacao), stdin)){
        observacao[strcspn(observacao, "\n")] = 0;
    }
    if(strlen(observacao) == 0) strcpy(observacao, "-");

    char sql[700];
    sprintf(sql,
        "INSERT INTO agendamentos (cliente_id, pet_id, servico_id, data, horario, status, observacao) "
        "VALUES (%d, %d, %d, '%s', '%s', 'AGENDADO', '%s');",
        cliente_id, pet_id, servico_id, data, horario, observacao);

    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar agendamento: %s\n", erro); sqlite3_free(erro); }
    else{
        int id = (int)sqlite3_last_insert_rowid(db);
        printf("\nAgendamento #%d criado com sucesso!\n", id);
        printf("Data: %s as %s\n", data, horario);
    }
}

void listarAgendamentos() {
    char *sql =
    "SELECT a.id, c.nome AS cliente, p.nome AS pet, s.nome AS servico, "
    "a.data, a.horario, a.status, a.observacao "
    "FROM agendamentos a "
    "JOIN clientes c ON a.cliente_id = c.id "
    "LEFT JOIN pets p ON a.pet_id = p.id "
    "JOIN servicos s ON a.servico_id = s.id "
    "ORDER BY a.data, a.horario;";
    char *erro = 0;
    printf("\n========== AGENDAMENTOS ==========\n");
    int rc = sqlite3_exec(db, sql, callbackAgendamento, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar agendamentos: %s\n", erro); sqlite3_free(erro); }
}

void agendamentosDoDia() {
    char data_hoje[11];
    obterDataAtualBR(data_hoje);

    char sql[500];
    sprintf(sql,
    "SELECT a.id, c.nome AS cliente, p.nome AS pet, s.nome AS servico, "
    "a.horario, a.status, a.observacao "
    "FROM agendamentos a "
    "JOIN clientes c ON a.cliente_id = c.id "
    "LEFT JOIN pets p ON a.pet_id = p.id "
    "JOIN servicos s ON a.servico_id = s.id "
    "WHERE a.data = '%s' "
    "ORDER BY a.horario;", data_hoje);

    char *erro = 0;
    printf("\n========== AGENDA DE HOJE (%s) ==========\n", data_hoje);
    int rc = sqlite3_exec(db, sql, callbackAgendamento, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro: %s\n", erro); sqlite3_free(erro); }
}

void buscarAgendamento() {
    int opcao;
    char valor[100];
    char sql[500];

    printf("\nBuscar agendamento por:\n");
    printf("1 ID\n");
    printf("2 Cliente ID\n");
    printf("3 Data (DD-MM-YYYY)\n");
    printf("4 Status (AGENDADO/CONFIRMADO/CONCLUIDO/CANCELADO)\n");
    printf("0 Cancelar\n");
    printf("Escolha: ");
    scanf("%d", &opcao);
    if(opcao == 0){ printf("Operacao cancelada.\n"); return; }
    while(getchar() != '\n');

    if(opcao == 1){
        printf("ID do agendamento: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT a.id, c.nome AS cliente, p.nome AS pet, s.nome AS servico, "
        "a.data, a.horario, a.status, a.observacao "
        "FROM agendamentos a "
        "JOIN clientes c ON a.cliente_id = c.id "
        "LEFT JOIN pets p ON a.pet_id = p.id "
        "JOIN servicos s ON a.servico_id = s.id "
        "WHERE a.id = %s;", valor);
    }
    else if(opcao == 2){
        printf("ID do cliente: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT a.id, c.nome AS cliente, p.nome AS pet, s.nome AS servico, "
        "a.data, a.horario, a.status, a.observacao "
        "FROM agendamentos a "
        "JOIN clientes c ON a.cliente_id = c.id "
        "LEFT JOIN pets p ON a.pet_id = p.id "
        "JOIN servicos s ON a.servico_id = s.id "
        "WHERE a.cliente_id = %s ORDER BY a.data, a.horario;", valor);
    }
    else if(opcao == 3){
        printf("Data (DD-MM-YYYY): ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT a.id, c.nome AS cliente, p.nome AS pet, s.nome AS servico, "
        "a.data, a.horario, a.status, a.observacao "
        "FROM agendamentos a "
        "JOIN clientes c ON a.cliente_id = c.id "
        "LEFT JOIN pets p ON a.pet_id = p.id "
        "JOIN servicos s ON a.servico_id = s.id "
        "WHERE a.data = '%s' ORDER BY a.horario;", valor);
    }
    else if(opcao == 4){
        printf("Status: ");
        scanf(" %[^\n]", valor);
        sprintf(sql,
        "SELECT a.id, c.nome AS cliente, p.nome AS pet, s.nome AS servico, "
        "a.data, a.horario, a.status, a.observacao "
        "FROM agendamentos a "
        "JOIN clientes c ON a.cliente_id = c.id "
        "LEFT JOIN pets p ON a.pet_id = p.id "
        "JOIN servicos s ON a.servico_id = s.id "
        "WHERE UPPER(a.status) = UPPER('%s') ORDER BY a.data, a.horario;", valor);
    }
    else{ printf("Opcao invalida!\n"); return; }

    char *errMsg = 0;
    sqlite3_exec(db, sql, callbackAgendamento, 0, &errMsg);
    if(errMsg){ printf("Erro na busca: %s\n", errMsg); sqlite3_free(errMsg); }
}

// atualiza status de um agendamento (confirmado ou cancelado)
static void alterarStatusAgendamento(const char *novo_status) {
    listarAgendamentos();
    int id;
    printf("ID do agendamento (0 = cancelar): ");
    scanf("%d", &id);
    if(id == 0){ printf("Operacao cancelada.\n"); return; }

    char sql[200];
    sprintf(sql, "UPDATE agendamentos SET status='%s' WHERE id=%d;", novo_status, id);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Agendamento #%d -> %s\n", id, novo_status); }
}

void confirmarAgendamento() {
    printf("\n=== CONFIRMAR AGENDAMENTO ===\n");
    alterarStatusAgendamento("CONFIRMADO");
}

void cancelarAgendamento() {
    printf("\n=== CANCELAR AGENDAMENTO ===\n");
    alterarStatusAgendamento("CANCELADO");
}

void concluirAgendamento() {
    printf("\n=== CONCLUIR AGENDAMENTO (gera pedido) ===\n");

    // lista apenas agendamentos pendentes/confirmados
    char *sql_lista =
    "SELECT a.id, c.nome AS cliente, p.nome AS pet, s.nome AS servico, "
    "a.data, a.horario, a.status "
    "FROM agendamentos a "
    "JOIN clientes c ON a.cliente_id = c.id "
    "LEFT JOIN pets p ON a.pet_id = p.id "
    "JOIN servicos s ON a.servico_id = s.id "
    "WHERE a.status IN ('AGENDADO','CONFIRMADO') "
    "ORDER BY a.data, a.horario;";
    printf("\n--- Agendamentos pendentes ---\n");
    sqlite3_exec(db, sql_lista, callbackAgendamento, 0, 0);

    int agenda_id;
    printf("ID do agendamento a concluir (0 = cancelar): ");
    scanf("%d", &agenda_id);
    if(agenda_id == 0){ printf("Operacao cancelada.\n"); return; }

    // busca dados do agendamento
    char sql_get[300];
    sprintf(sql_get,
        "SELECT cliente_id, pet_id, servico_id FROM agendamentos WHERE id=%d;",
        agenda_id);

    sqlite3_stmt *stmt;
    int cliente_id = 0, pet_id = 0, servico_id = 0;

    if(sqlite3_prepare_v2(db, sql_get, -1, &stmt, 0) == SQLITE_OK){
        if(sqlite3_step(stmt) == SQLITE_ROW){
            cliente_id  = sqlite3_column_int(stmt, 0);
            pet_id      = sqlite3_column_int(stmt, 1);
            servico_id  = sqlite3_column_int(stmt, 2);
        }
    }
    sqlite3_finalize(stmt);

    if(cliente_id == 0){ printf("Agendamento nao encontrado.\n"); return; }

    char data_atual[11];
    obterDataAtualBR(data_atual);

    // cria pedido
    char sql_pedido[300];
    sprintf(sql_pedido,
        "INSERT INTO pedidos (cliente_id, data, total, status) "
        "VALUES (%d, '%s', 0.00, 'aberto');",
        cliente_id, data_atual);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql_pedido, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar pedido: %s\n", erro); sqlite3_free(erro); return; }

    int pedido_id = (int)sqlite3_last_insert_rowid(db);
    printf("\nPedido #%d criado!\n", pedido_id);

    // adiciona servico ao pedido pelo preco do catalogo
    char sql_item[400];
    sprintf(sql_item,
        "INSERT INTO itens_pedido (pedido_id, servico_id, pet_id, quantidade, preco_unitario) "
        "VALUES (%d, %d, %d, 1, (SELECT preco FROM servicos WHERE id=%d));",
        pedido_id, servico_id, pet_id, servico_id);
    rc = sqlite3_exec(db, sql_item, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao adicionar item: %s\n", erro); sqlite3_free(erro); return; }

    // atualiza total do pedido
    char sql_total[300];
    sprintf(sql_total,
        "UPDATE pedidos SET total = ("
        "SELECT SUM(quantidade * preco_unitario) FROM itens_pedido WHERE pedido_id=%d"
        ") WHERE id=%d;", pedido_id, pedido_id);
    sqlite3_exec(db, sql_total, 0, 0, 0);

    // marca agendamento como concluido
    char sql_status[200];
    sprintf(sql_status, "UPDATE agendamentos SET status='CONCLUIDO' WHERE id=%d;", agenda_id);
    sqlite3_exec(db, sql_status, 0, 0, 0);

    printf("Agendamento #%d marcado como CONCLUIDO.\n", agenda_id);

    emitirResumo(pedido_id);
    cadastrarPagamento(pedido_id);
}

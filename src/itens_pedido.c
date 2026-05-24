#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>                          
#include "../sqlite/sqlite3.h"
#include "../database/database.h"
#include "../include/itens_pedido.h"
#include "../include/cliente.h"
#include "../include/pet.h"
#include "../include/servico.h"
#include "../include/assinatura.h"         

void criarTabelaItensPedido(){//função para criar a tabela itens_pedido no banco de dados, caso ela ainda não exista. A tabela itens_pedido é utilizada para armazenar os detalhes dos serviços incluídos em cada pedido, como o ID do pedido, o ID do serviço, o ID do pet (se aplicável), a quantidade e o preço unitário. A função executa uma consulta SQL para criar a tabela com as colunas apropriadas e define as chaves estrangeiras para garantir a integridade referencial com as tabelas de pedidos, serviços e pets. Caso haja algum erro durante a execução da consulta SQL, a função exibirá uma mensagem de erro apropriada.
    char *sql =
    "CREATE TABLE IF NOT EXISTS itens_pedido ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "pedido_id INTEGER,"
    "servico_id INTEGER,"
    "pet_id INTEGER,"
    "quantidade INTEGER,"
    "preco_unitario REAL,"
    "FOREIGN KEY(pedido_id) REFERENCES pedidos(id) ON DELETE CASCADE,"
    "FOREIGN KEY(servico_id) REFERENCES servicos(id),"
    "FOREIGN KEY(pet_id) REFERENCES pets(id));";
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar tabela itens_pedido: %s\n", erro); sqlite3_free(erro); }
}


static int cbUso(void *data, int argc, char **argv, char **col){//callback para contar usos de benefícios do plano
    if(argc > 0 && argv[0]) *(int*)data = atoi(argv[0]);
    return 0;
}

static int cbPlanoItem(void *data, int argc, char **argv, char **col){//callback para obter nome do plano
    if(argc > 0 && argv[0]) strcpy((char*)data, argv[0]);
    return 0;
}

static int cbAsinId(void *data, int argc, char **argv, char **col){//callback para obter id da assinatura
    if(argc > 0 && argv[0]) *(int*)data = atoi(argv[0]);
    return 0;
}

void adicionarItemPedido(int pedido_id, int id_cliente) {
    struct item_pedido item;
    item.pedido_id = pedido_id;

    printf("\n=== ADICIONAR ITEM AO PEDIDO ===\n");

    if (id_cliente == 999) {
        item.pet_id = 999;
        printf("\nCliente Avulso: utilizando Pet Avulso.\n");
    } else {
        printf("\nPets do cliente:\n");
        char sql_pets[300];
        sprintf(sql_pets,
                "SELECT pets.id, pets.nome, pets.especie, pets.raca "
                "FROM pets "
                "WHERE pets.cliente_id = %d;", id_cliente);
        char *erro_pets = 0;
        sqlite3_exec(db, sql_pets, callback, 0, &erro_pets);
        if(erro_pets){ printf("Erro ao buscar pets: %s\n", erro_pets); sqlite3_free(erro_pets); }

        printf("ID do pet: ");
        scanf("%d", &item.pet_id);
    }

    printf("\nDigite parte do nome do servico para buscar (ou ENTER para listar todos): ");
    char busca_servico[50] = "";
    int _ch;
    while((_ch = getchar()) != '\n' && _ch != EOF); // descarta \n residual do scanf anterior
    if(fgets(busca_servico, sizeof(busca_servico), stdin)){
        busca_servico[strcspn(busca_servico, "\n")] = 0;
    }

    char sql_busca_serv[300];
    if (strlen(busca_servico) == 0) {
        sprintf(sql_busca_serv, "SELECT id, nome, preco FROM servicos ORDER BY nome;");
    } else {
        sprintf(sql_busca_serv,
                "SELECT id, nome, preco FROM servicos WHERE nome LIKE '%%%s%%' ORDER BY nome;", busca_servico);
    }

    printf("\n--- SERVICOS ENCONTRADOS ---\n");
    char *erro_busca_serv = 0;
    sqlite3_exec(db, sql_busca_serv, callback, 0, &erro_busca_serv);
    if(erro_busca_serv){
        printf("Erro na busca: %s\n", erro_busca_serv);
        sqlite3_free(erro_busca_serv);
    }
    printf("----------------------------\n");

    int id_valido = 0;
    while (!id_valido) {
        printf("ID do servico (ou 0 para cancelar a operacao): ");
        scanf("%d", &item.servico_id);

        if (item.servico_id == 0) {
            printf("\nOperacao cancelada pelo usuario.\n");
            return;
        }

        char sql_checa_id[150];
        sprintf(sql_checa_id, "SELECT COUNT(*) FROM servicos WHERE id = %d;", item.servico_id);

        sqlite3_stmt *stmt_id;
        int existe = 0;

        if (sqlite3_prepare_v2(db, sql_checa_id, -1, &stmt_id, 0) == SQLITE_OK) {
            if (sqlite3_step(stmt_id) == SQLITE_ROW) {
                existe = sqlite3_column_int(stmt_id, 0);
            }
        }
        sqlite3_finalize(stmt_id);

        if (existe > 0) {
            id_valido = 1;
        } else {
            printf("\nErro: ID invalido ou nao encontrado! Tente novamente.\n\n");
        }
    }

    printf("Quantidade: ");
    scanf("%d", &item.quantidade);

    char sql_valida[200];
    sprintf(sql_valida, "SELECT estoque FROM servicos WHERE id = %d;", item.servico_id);

    sqlite3_stmt *stmt;
    int estoque_atual = -1;

    if (sqlite3_prepare_v2(db, sql_valida, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            estoque_atual = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);

    if (estoque_atual != -1 && estoque_atual < item.quantidade) {
        printf("\nErro: Estoque insuficiente! Quantidade disponivel: %d\n", estoque_atual);
        return;
    }

    // verifica se servico é incluso no plano
    float preco_cobrar = -1.0;

    if(id_cliente != 999){
        char plano[50] = "";
        int asin_id = 0;

        char sql_plano[300];
        sprintf(sql_plano,
            "SELECT plano FROM assinatura "
            "WHERE cliente_id=%d AND status='ATIVA';", id_cliente);
        sqlite3_exec(db, sql_plano, cbPlanoItem, plano, 0);

        char sql_asin_id[300];
        sprintf(sql_asin_id,
            "SELECT id FROM assinatura "
            "WHERE cliente_id=%d AND status='ATIVA';", id_cliente);
        sqlite3_exec(db, sql_asin_id, cbAsinId, &asin_id, 0);

        int inclusos_patinhas[] = {1, 0};
        int limites_patinhas[]  = {1, 0};

        int inclusos_amigo[]    = {1, 3, 0};
        int limites_amigo[]     = {2, 1, 0};

        int inclusos_vida[]     = {1, 2, 3, 0};
        int limites_vida[]      = {99, 2, 2, 0};

        int *inclusos = NULL;
        int *limites  = NULL;

        if(strcmp(plano, "PATINHAS") == 0){
            inclusos = inclusos_patinhas;
            limites  = limites_patinhas;
        } else if(strcmp(plano, "AMIGO PET") == 0){
            inclusos = inclusos_amigo;
            limites  = limites_amigo;
        } else if(strcmp(plano, "VIDA ANIMAL") == 0){
            inclusos = inclusos_vida;
            limites  = limites_vida;
        }

        if(inclusos != NULL && asin_id > 0){
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            int mes = tm.tm_mon + 1;
            int ano = tm.tm_year + 1900;

            for(int i = 0; inclusos[i] != 0; i++){
                if(inclusos[i] == item.servico_id){
                    int usos = 0;
                    char sql_uso[300];
                    sprintf(sql_uso,
                        "SELECT COUNT(*) FROM uso_beneficios "
                        "WHERE assinatura_id=%d AND servico_id=%d "
                        "AND mes=%d AND ano=%d;",
                        asin_id, item.servico_id, mes, ano);
                    sqlite3_exec(db, sql_uso, cbUso, &usos, 0);

                    if(usos < limites[i]){
                        preco_cobrar = 0.0;
                        printf("\nServico incluso no plano %s! Cobrado: R$ 0,00\n", plano);

                        char sql_reg[300];
                        sprintf(sql_reg,
                            "INSERT INTO uso_beneficios "
                            "(assinatura_id, servico_id, mes, ano) "
                            "VALUES(%d, %d, %d, %d);",
                            asin_id, item.servico_id, mes, ano);
                        sqlite3_exec(db, sql_reg, 0, 0, 0);
                    } else {
                        printf("\nLimite mensal do plano atingido para este servico.\n");
                    }
                    break;
                }
            }
        }
    }

    char sql[500];
    if(preco_cobrar >= 0){
        sprintf(sql,
            "INSERT INTO itens_pedido (pedido_id, servico_id, pet_id, quantidade, preco_unitario)"
            " VALUES(%d, %d, %d, %d, %.2f);",
            item.pedido_id, item.servico_id, item.pet_id,
            item.quantidade, preco_cobrar);
    } else {
        sprintf(sql,
            "INSERT INTO itens_pedido (pedido_id, servico_id, pet_id, quantidade, preco_unitario)"
            " VALUES(%d, %d, %d, %d, (SELECT preco FROM servicos WHERE id=%d));",
            item.pedido_id, item.servico_id, item.pet_id,
            item.quantidade, item.servico_id);
    }

    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){
        printf("Erro ao adicionar item: %s\n", erro);
        sqlite3_free(erro);
    } else {
        char sql_total[300];
        sprintf(sql_total,
            "UPDATE pedidos SET total = ("
            "SELECT SUM(quantidade * preco_unitario) FROM itens_pedido WHERE pedido_id=%d"
            ") WHERE id=%d;", pedido_id, pedido_id);
        sqlite3_exec(db, sql_total, 0, 0, 0);
        printf("Item adicionado com sucesso!\n");
    }
}

void listarItensPedido(int pedido_id){//função para listar os itens de um pedido específico. A função recebe o ID do pedido como parâmetro e executa uma consulta SQL para obter os detalhes dos itens incluídos no pedido, como o nome do serviço, o nome do pet (se aplicável), a quantidade e o preço unitário. Os resultados são exibidos em um formato legível para o usuário. Caso haja algum erro durante a execução da consulta SQL, a função exibirá uma mensagem de erro apropriada.
    char sql[500];
    sprintf(sql,
    "SELECT itens_pedido.id, servicos.nome AS servico, COALESCE(pets.nome, 'Nenhum') AS pet, "
    "itens_pedido.quantidade, itens_pedido.preco_unitario "
    "FROM itens_pedido "
    "JOIN servicos ON itens_pedido.servico_id = servicos.id "
    "LEFT JOIN pets ON itens_pedido.pet_id = pets.id "
    "WHERE itens_pedido.pedido_id = %d;", pedido_id);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar itens: %s\n", erro); sqlite3_free(erro); }
}

void excluirItemPedido(int pedido_id){//função para excluir um item específico de um pedido. A função recebe o ID do pedido como parâmetro e exibe a lista de itens incluídos no pedido, permitindo ao usuário escolher o item que deseja excluir. O usuário é solicitado a confirmar a exclusão antes que a função execute uma consulta SQL para remover o item selecionado da tabela itens_pedido. Após a exclusão, a função atualiza o total do pedido com base nos itens restantes. Caso haja algum erro durante a execução da consulta SQL, a função exibirá uma mensagem de erro apropriada.
    int id;
    char confirm;
    char sql[200];

    listarItensPedido(pedido_id);

    printf("\nDigite o ID do item que deseja excluir: ");
    scanf("%d", &id);

    printf("Confirmar exclusao? (s/n): ");
    scanf(" %c", &confirm);
    if(confirm != 's' && confirm != 'S'){ printf("Operacao cancelada.\n"); return; }

    sprintf(sql, "DELETE FROM itens_pedido WHERE id=%d AND pedido_id=%d;", id, pedido_id);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){
        printf("Erro ao excluir item: %s\n", erro);
        sqlite3_free(erro);
    } else {
        char sql_total[300];
        sprintf(sql_total,
        "UPDATE pedidos SET total = ("
        "SELECT COALESCE(SUM(quantidade * preco_unitario), 0) FROM itens_pedido WHERE pedido_id=%d"
        ") WHERE id=%d;", pedido_id, pedido_id);
        sqlite3_exec(db, sql_total, 0, 0, 0);
        printf("Item excluido com sucesso!\n");
    }
}
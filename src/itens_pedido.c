#include <stdio.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"
#include "../database/database.h"
#include "../include/itens_pedido.h"
#include "../include/cliente.h"
#include "../include/pet.h"
#include "../include/servico.h"

void criarTabelaItensPedido(){
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

void adicionarItemPedido(int pedido_id){
    struct item_pedido item;
    item.pedido_id = pedido_id;

    printf("\n=== ADICIONAR ITEM AO PEDIDO ===\n");

    // busca o cliente_id do pedido automaticamente
    int cliente_id = 0;
    char sql_cliente[200];
    sprintf(sql_cliente, "SELECT cliente_id FROM pedidos WHERE id=%d;", pedido_id);
    // callback temporario para pegar o cliente_id
    sqlite3_exec(db, sql_cliente, callback, 0, 0);

    // mostra os pets do cliente
    printf("\nPets do cliente:\n");
    char sql_pets[300];
    sprintf(sql_pets,
    "SELECT pets.id, pets.nome, pets.especie, pets.raca "
    "FROM pets "
    "JOIN pedidos ON pets.cliente_id = pedidos.cliente_id "
    "WHERE pedidos.id = %d;", pedido_id);
    char *erro_pets = 0;
    sqlite3_exec(db, sql_pets, callback, 0, &erro_pets);
    if(erro_pets){ printf("Erro ao buscar pets: %s\n", erro_pets); sqlite3_free(erro_pets); }

    printf("ID do pet: ");
    scanf("%d", &item.pet_id);

    // lista serviços disponíveis
    printf("\nServicos disponíveis:\n");
    listarServicos();
    printf("ID do servico: ");
    scanf("%d", &item.servico_id);

    printf("Quantidade: ");
    scanf("%d", &item.quantidade);

    // insere o item com o preco buscado automaticamente da tabela servicos
    char sql[400];
    sprintf(sql,
    "INSERT INTO itens_pedido (pedido_id, servico_id, pet_id, quantidade, preco_unitario)"
    " VALUES(%d, %d, %d, %d, (SELECT preco FROM servicos WHERE id=%d));",
    item.pedido_id, item.servico_id, item.pet_id, item.quantidade, item.servico_id);

    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){
        printf("Erro ao adicionar item: %s\n", erro);
        sqlite3_free(erro);
    }
    else{
        // atualiza o total do pedido automaticamente
        char sql_total[300];
        sprintf(sql_total,
        "UPDATE pedidos SET total = ("
        "SELECT SUM(quantidade * preco_unitario) FROM itens_pedido WHERE pedido_id=%d"
        ") WHERE id=%d;", pedido_id, pedido_id);
        sqlite3_exec(db, sql_total, 0, 0, 0);
        printf("Item adicionado com sucesso!\n");
    }
}

void listarItensPedido(int pedido_id){
    char sql[400];
    sprintf(sql,
    "SELECT itens_pedido.id, servicos.nome AS servico, pets.nome AS pet, "
    "itens_pedido.quantidade, itens_pedido.preco_unitario "
    "FROM itens_pedido "
    "JOIN servicos ON itens_pedido.servico_id = servicos.id "
    "JOIN pets ON itens_pedido.pet_id = pets.id "
    "WHERE itens_pedido.pedido_id = %d;", pedido_id);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar itens: %s\n", erro); sqlite3_free(erro); }
}

void excluirItemPedido(int pedido_id){
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
    }
    else{
        // atualiza o total do pedido após exclusão
        char sql_total[300];
        sprintf(sql_total,
        "UPDATE pedidos SET total = ("
        "SELECT COALESCE(SUM(quantidade * preco_unitario), 0) FROM itens_pedido WHERE pedido_id=%d"
        ") WHERE id=%d;", pedido_id, pedido_id);
        sqlite3_exec(db, sql_total, 0, 0, 0);
        printf("Item excluido com sucesso!\n");
    }
}
#include <stdio.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"
#include "../database/database.h"
#include "../include/servico.h"
#include "../include/cliente.h"

void criarTabelaServicos(){
    char *sql =
    "CREATE TABLE IF NOT EXISTS servicos ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "nome TEXT,"
    "descricao TEXT,"
    "preco REAL);"; 
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar tabela servicos: %s\n", erro); sqlite3_free(erro); }
}

void saveServicoDB(struct servico s){
    char sql[800];
    sprintf(sql,
    "INSERT INTO servicos (nome, descricao, preco)"
    " VALUES('%s','%s',%.2f);",
    s.nome, s.descricao, s.preco);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao cadastrar servico: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Servico cadastrado com sucesso!\n"); }
}

void cadastrarServico(){
    char continuar = 's';
    while(continuar == 's' || continuar == 'S'){
        struct servico novo;
        printf("\n=== CADASTRO DE SERVICO ===\n");
        printf("Nome: ");
        scanf(" %[^\n]", novo.nome);
        printf("Descricao: ");
        scanf(" %[^\n]", novo.descricao);
        printf("Preco: ");
        scanf("%f", &novo.preco);
        saveServicoDB(novo);
        printf("\nCadastrar outro servico? (s/n): ");
        scanf(" %c", &continuar);
    }
}

void listarServicos(){
    char *sql = "SELECT * FROM servicos;";
    char *erro = 0;
    printf("\n========== SERVICOS ==========\n");
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar servicos: %s\n", erro); sqlite3_free(erro); }
}

void buscarServico(){
    int opcao;
    char valor[100];
    char sql[300];
    printf("\nBuscar servico por:\n");
    printf("1 ID\n");
    printf("2 Nome\n");
    scanf("%d", &opcao);
    while(getchar() != '\n');
    if(opcao == 1){
        printf("Digite o ID: ");
        scanf(" %[^\n]", valor);
        sprintf(sql, "SELECT * FROM servicos WHERE id = %s;", valor);
    }
    else if(opcao == 2){
        printf("Digite o nome: ");
        scanf(" %[^\n]", valor);
        sprintf(sql, "SELECT * FROM servicos WHERE nome LIKE '%%%s%%';", valor);
    }
    else{ printf("Opcao invalida!\n"); return; }
    char *errMsg = 0;
    sqlite3_exec(db, sql, callback, 0, &errMsg);
    if(errMsg){ printf("Erro na busca: %s\n", errMsg); sqlite3_free(errMsg); }
}

void alterarServico(){
    int id, opcao;
    char valor[100];
    char sql[300];
    listarServicos();
    printf("Digite o ID do servico que deseja alterar: ");
    scanf("%d", &id);
    printf("\n1 Alterar nome\n");
    printf("2 Alterar descricao\n");
    printf("3 Alterar preco\n");
    printf("Escolha: ");
    scanf("%d", &opcao);
    printf("Digite o novo valor: ");
    scanf(" %[^\n]", valor);
    switch(opcao){
        case 1: sprintf(sql, "UPDATE servicos SET nome='%s' WHERE id=%d;", valor, id); break;
        case 2: sprintf(sql, "UPDATE servicos SET descricao='%s' WHERE id=%d;", valor, id); break;
        case 3: sprintf(sql, "UPDATE servicos SET preco=%s WHERE id=%d;", valor, id); break;
        default: printf("Opcao invalida\n"); return;
    }
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if(rc != SQLITE_OK){ printf("Erro SQL: %s\n", errMsg); sqlite3_free(errMsg); }
    else{ printf("Servico atualizado com sucesso!\n"); }
}

void excluirServico(){
    int id;
    char confirm;
    char sql[200];
    listarServicos();
    printf("\nDigite o ID do servico que deseja excluir: ");
    scanf("%d", &id);
    printf("Confirmar exclusao? (s/n): ");
    scanf(" %c", &confirm);
    if(confirm != 's' && confirm != 'S'){ printf("Operacao cancelada.\n"); return; }
    sprintf(sql, "DELETE FROM servicos WHERE id=%d;", id);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao excluir servico: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Servico excluido com sucesso!\n"); }
}
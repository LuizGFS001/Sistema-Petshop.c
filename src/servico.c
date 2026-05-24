#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../sqlite/sqlite3.h"
#include "../database/database.h"
#include "../include/servico.h"
#include "../include/cliente.h"

void criarTabelaServicos(){//função para criar a tabela de serviços no banco de dados, caso ela ainda não exista. A função utiliza um comando SQL para definir a estrutura da tabela, incluindo os campos id, nome, descricao e preco. O campo id é definido como chave primária e autoincremento para garantir que cada serviço tenha um identificador único. A função também inclui tratamento de erros para informar caso haja algum problema durante a criação da tabela.
    char *sql =
    "CREATE TABLE IF NOT EXISTS servicos ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "nome TEXT,"
    "descricao TEXT,"
    "preco REAL,"
    "tipo TEXT DEFAULT 'SERVICO',"
    "estoque INTEGER DEFAULT -1,"
    "estoque_minimo INTEGER DEFAULT 0);";
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar tabela servicos: %s\n", erro); sqlite3_free(erro); }
}

void saveServicoDB(struct servico s){//função para salvar um serviço no banco de dados, recebendo uma estrutura de serviço como parâmetro. A função constrói uma consulta SQL de inserção com os dados do serviço e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que o serviço foi cadastrado com sucesso.
    char sql[800];
    sprintf(sql,
    "INSERT INTO servicos "
    "(nome,descricao,preco,tipo,estoque,estoque_minimo)"
    "VALUES"
    "('%s','%s',%.2f,'%s',%d,%d);",
    s.nome,
    s.descricao,
    s.preco,
    s.tipo,
    s.estoque,
    s.estoqueMinimo);
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao cadastrar servico: %s\n", erro); sqlite3_free(erro); }
    else{ printf("Servico cadastrado com sucesso!\n"); }
}

static int callbackID(void *data, int argc, char **argv, char **col){//callback para obter o ID do último pedido cadastrado, armazenando o valor em uma variável passada por referência. A função verifica se há resultados e, se houver, converte o valor do ID para um inteiro e armazena na variável. A função retorna 0 para indicar que a execução foi bem-sucedida.
    if(argc > 0 && argv[0]){
        *(int*)data = atoi(argv[0]);
    }
    return 0;
}

void cadastrarServico(){//função para cadastrar um novo serviço, solicitando ao usuário as informações do serviço (nome, descrição e preço) e salvando os dados no banco de dados. A função inclui um loop para permitir o cadastro de múltiplos serviços, perguntando ao usuário se deseja cadastrar outro serviço após cada cadastro.
    char continuar='s';

while(continuar=='s'||continuar=='S'){
struct servico novo;
printf("\nNOME: ");
scanf(" %[^\n]",novo.nome);
printf("DESCRICAO: ");
scanf(" %[^\n]",novo.descricao);
printf("PRECO: ");
scanf("%f",&novo.preco);
printf("\nTIPO\n");
printf("1 SERVICO\n");
printf("2 PRODUTO\n");

int op;
scanf("%d",&op);
if(op==1){
strcpy(novo.tipo,"SERVICO");
novo.estoque=0;
novo.estoqueMinimo=0;
}else{

strcpy(novo.tipo,"PRODUTO");
printf("Estoque: ");
scanf("%d",&novo.estoque);
printf("Estoque minimo: ");
scanf("%d",&novo.estoqueMinimo);

}
saveServicoDB(novo);
printf("\nCadastrar outro? (s/n): ");
scanf(" %c",&continuar);

}

}

void listarServicos(){//função para listar os serviços cadastrados no banco de dados, executando uma consulta SQL para selecionar todos os registros da tabela de serviços e exibindo os resultados. A função utiliza um callback para processar os resultados da consulta e exibir as informações de cada serviço. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.
    char *sql = "SELECT * FROM servicos;";
    char *erro = 0;
    printf("\n========== SERVICOS ==========\n");
    int rc = sqlite3_exec(db, sql, callback, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao listar servicos: %s\n", erro); sqlite3_free(erro); }
}

void buscarServico(){//função para buscar serviços no banco de dados, permitindo ao usuário escolher o critério de busca (ID ou nome) e inserindo o valor correspondente. A função constrói uma consulta SQL com base na escolha do usuário e executa a consulta usando sqlite3_exec, utilizando um callback para processar os resultados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.
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

void alterarServico(){//função para alterar os dados de um serviço existente no banco de dados. A função solicita ao usuário o ID do serviço que deseja alterar e o campo específico que deseja modificar (nome, descrição ou preço). Em seguida, o usuário insere o novo valor para o campo selecionado. A função constrói uma consulta SQL de atualização com base na escolha do usuário e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que a atualização foi bem-sucedida.
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

void excluirServico(){//função para excluir um serviço do banco de dados, solicitando ao usuário o ID do serviço que deseja excluir. A função inclui uma confirmação de exclusão para evitar exclusões acidentais. Se o usuário confirmar a exclusão, a função constrói uma consulta SQL de exclusão e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que o serviço foi excluído com sucesso.
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

void executarServicos(int pedido_id){//função para executar os serviços selecionados no pedido, processando cada serviço e calculando o total do atendimento. A função pode incluir chamadas para funções específicas de cada serviço, bem como a aplicação de descontos ou promoções, se aplicável.

    char sql_update[200];

    sprintf(sql_update,
    "UPDATE pedidos SET status='em andamento' WHERE id=%d;",
    pedido_id);

    char *erro = 0;

    int rc = sqlite3_exec(db, sql_update, 0, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao atualizar status: %s\n", erro);
        sqlite3_free(erro);
    } else {
        printf("\nPedido #%d agora está EM ANDAMENTO.\n", pedido_id);
    }
}

void atualizarEstoque(int id, int quantidade){

    char sql[300];

    sprintf(sql,

    "UPDATE servicos "

    "SET estoque = estoque - %d "

    "WHERE id=%d;",

    quantidade,

    id);

    char *erro = 0;

    sqlite3_exec(db, sql, 0, 0, &erro);

    if(erro){

        printf("Erro estoque: %s\n",erro);

        sqlite3_free(erro);
    }
}
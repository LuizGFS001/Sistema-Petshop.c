#ifndef SERVICO_H
#define SERVICO_H

struct servico{
    int id;
    char nome[100];
    char descricao[150];
    float preco;
    char tipo[20];
    int estoque;
    int estoqueMinimo;
};

void criarTabelaServicos();

void saveServicoDB(struct servico s);

void cadastrarServico();

void listarServicos();

void buscarServico();

void alterarServico();

void excluirServico();

void executarServicos(int pedido_id);

void atualizarEstoque(int id, int quantidade);

#endif
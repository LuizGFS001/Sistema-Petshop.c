#ifndef SERVICO_H
#define SERVICO_H

struct servico{
    int id;
    char nome[50];
    char descricao[100];
    float preco;
};

void criarTabelaServicos();
void saveServicoDB(struct servico s);
void cadastrarServico();
void listarServicos();
void buscarServico();
void alterarServico();
void excluirServico();

#endif
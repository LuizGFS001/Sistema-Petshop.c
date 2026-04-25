#ifndef PAGAMENTO_H
#define PAGAMENTO_H

struct pagamento{
    int id;
    int pedido_id;
    float valor;
    char forma[30];
    char data[20];
};

void criarTabelaPagamentos();
void savePagamentoDB(struct pagamento p);
void cadastrarPagamento();
void listarPagamentos();
void buscarPagamento();
void excluirPagamento();

#endif
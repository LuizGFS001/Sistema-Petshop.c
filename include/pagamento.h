#ifndef PAGAMENTO_H
#define PAGAMENTO_H

struct pagamento{
    int id;
    int pedido_id;
    float valor;
    char forma[30];
    char data[20];
};

int callback(void *data, int argc, char **argv, char **col);

void criarTabelaPagamentos();
void savePagamentoDB(struct pagamento p);
void cadastrarPagamento(int pedido_id);
void listarPagamentos();
void buscarPagamento();
void excluirPagamento();
void emitirResumo(int pedido_id);
int formaPagamento();
void cadastrarPagamentoAuto();
void cobrarAssinaturaStandalone(int cliente_id, float valor);
#endif
#ifndef PEDIDO_H
#define PEDIDO_H

struct pedido{
    int id;
    int cliente_id;
    char data[20];
    float total;
    char status[20];
};

void criarTabelaPedidos();
void criarTabelaItensPedido();
void cadastrarPedido();
void listarPedidos();
void buscarPedido();
void alterarPedido();
void excluirPedido();
int OrdemServico(int id_cliente, char *data_atual);
#endif
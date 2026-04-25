#ifndef ITENS_PEDIDO_H
#define ITENS_PEDIDO_H

struct item_pedido{
    int id;
    int pedido_id;
    int servico_id;
    int pet_id;
    int quantidade;
    float preco_unitario;
};

void criarTabelaItensPedido();
void adicionarItemPedido(int pedido_id);
void listarItensPedido(int pedido_id);
void excluirItemPedido(int pedido_id);

#endif
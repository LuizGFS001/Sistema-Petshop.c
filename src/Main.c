#include <stdio.h>
#include "../database/database.h"
#include "../include/menu.h"
#include "../include/cliente.h"
#include "../include/servico.h"
#include "../include/pedido.h"
#include "../include/itens_pedido.h"
#include "../include/pagamento.h"
#include "../include/pet.h"

int main(){
    if(abrirDB() != 0){
        printf("Erro ao conectar com banco\n");
        return 1;
    }
    criartabelaclientes();
    criarTabelaPets();
    criarTabelaServicos();
    criarTabelaPedidos();
    criarTabelaItensPedido();
    criarTabelaPagamentos();
    menuPrincipal();
    fecharDB();
    return 0;
}
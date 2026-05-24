#include <stdio.h>
#include <windows.h>
#include "database.h"
#include "menu.h"
#include "cliente.h"
#include "servico.h"
#include "pedido.h"
#include "itens_pedido.h"
#include "pagamento.h"
#include "pet.h"
#include "assinatura.h"
#include "agendamento.h"

int main(){// Inicia o programa

    SetConsoleOutputCP(65001);

    if(abrirDB() != 0){
        printf("Erro ao conectar com banco\n");
        return 1;
    }

    criartabelaclientes();
    ClienteGenerico();      // insere cliente avulso id=999
    criarTabelaPets();
    PetGenerico(999);       // insere pet avulso id=999 ligado ao cliente avulso
    criarTabelaServicos();
    criarTabelaPedidos();
    criarTabelaItensPedido();
    criarTabelaPagamentos();
    criarTabelaAssinatura();
    criarTabelaAgendamentos();
    menuPrincipal();
    fecharDB();
    return 0;
}
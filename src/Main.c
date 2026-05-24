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

int main(){// Inicia o programa

    UINT CPAGE_UTF8 = 65001;//definição da página de código UTF-8 para garantir a correta exibição de caracteres acentuados e especiais no console, especialmente em sistemas Windows. A função SetConsoleOutputCP é usada para definir a página de código do console para UTF-8, permitindo que os caracteres sejam exibidos corretamente durante a execução do programa.
    UINT CPAGE_DEFAULT = GetConsoleOutputCP();
    SetConsoleOutputCP(CPAGE_UTF8);

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
    menuPrincipal();
    fecharDB();
    return 0;
}
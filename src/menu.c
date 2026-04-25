#include <stdio.h>

#include "../include/menu.h"
#include "../include/cliente.h"
#include "../include/pedido.h"
#include "../include/pagamento.h"


void menuPrincipal(){

    int opcao;

    do{

        printf("\n=== PETSHOP SYSTEM ===\n");

        printf("1 Gerenciar Clientes\n");
        printf("2 Gerenciar Pets\n");
        printf("3 Gerenciar Servicos\n");
        printf("4 Gerenciar Pedidos\n");
        printf("5 Gerenciar Pagamentos\n");
        printf("0 Sair\n");

        printf("Escolha: ");
        scanf("%d",&opcao);

        switch(opcao){

            case 1: menuClientes(); break;
            case 2: menuPets(); break;
            case 3: menuServico(); break;
            case 4: menuPedidos(); break;
            case 5: menuPagamentos(); break;
            case 0: printf("Encerrando sistema...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);

}


void menuClientes(){

    int opcao;

    do{

        printf("\n=== MENU CLIENTES ===\n");

        printf("1 Cadastrar Cliente\n");
        printf("2 Listar Clientes\n");
        printf("3 Alterar Cliente\n");
        printf("4 Buscar Cliente\n");
        printf("5 Excluir Cliente\n");
        printf("0 Voltar\n");

        printf("Escolha: ");
        scanf("%d",&opcao);

        switch(opcao){

            case 1: cadastrarCliente(); break;
            case 2: listarClientes(); break;
            case 3: alterarCliente(); break;
            case 4: buscarCliente(); break;
            case 5: excluirCliente(); break;
            case 0: printf("Voltando ao menu principal...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);

}

void menuPets(){

    int opcao;

    do{

        printf("\n=== MENU PETS ===\n");

        printf("1 Cadastrar Pet\n");
        printf("2 Listar Pets\n");
        printf("3 Alterar Pet\n");
        printf("4 Buscar Pet\n");
        printf("5 Excluir Pet\n");
        printf("0 Voltar\n");

        printf("Escolha: ");
        scanf("%d",&opcao);

        switch(opcao){

            case 1: cadastrarPet(); break;
            case 2: listarPets(); break;
            case 3: alterarPet(); break;
            case 4: buscarPet(); break;
            case 5: excluirPet(); break;
            case 0: printf("Voltando ao menu principal...\n"); break;
            default:
                printf("Opcao invalida!\n");
        }

    }while(opcao != 0);

}

void menuServico(){
    int opcao;
    do{
        printf("\n=== MENU SERVICOS ===\n");
        printf("1 Cadastrar Servico\n");
        printf("2 Listar Servicos\n");
        printf("3 Alterar Servico\n");
        printf("4 Buscar Servico\n");
        printf("5 Excluir Servico\n");
        printf("0 Voltar\n");
        printf("Escolha: ");
        scanf("%d",&opcao);


        switch(opcao){
            case 1: cadastrarServico(); break;
            case 2: listarServicos(); break;
            case 3: alterarServico(); break;
            case 4: buscarServico(); break;
            case 5: excluirServico(); break;
            case 0: printf("Voltando ao menu principal...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);
}

void menuPedidos(){
    int opcao;
    do{
        printf("\n=== MENU PEDIDOS ===\n");
        printf("1 Novo Pedido\n");
        printf("2 Listar Pedidos\n");
        printf("3 Buscar Pedido\n");
        printf("4 Alterar Pedido\n");
        printf("5 Excluir Pedido\n");
        printf("0 Voltar\n");
        printf("Escolha: ");
        scanf("%d",&opcao);

        
        switch(opcao){
            case 1: cadastrarPedido(); break;
            case 2: listarPedidos(); break;
            case 3: buscarPedido(); break;
            case 4: alterarPedido(); break;
            case 5: excluirPedido(); break;
            case 0: printf("Voltando ao menu principal...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);
}

void menuPagamentos(){
    int opcao;
    do{
        printf("\n=== MENU PAGAMENTOS ===\n");
        printf("1 Registrar Pagamento\n");
        printf("2 Listar Pagamentos\n");
        printf("3 Buscar Pagamento\n");
        printf("4 Excluir Pagamento\n");
        printf("0 Voltar\n");
        printf("Escolha: ");
        scanf("%d",&opcao);
        switch(opcao){
            case 1: cadastrarPagamento(); break;
            case 2: listarPagamentos(); break;
            case 3: buscarPagamento(); break;
            case 4: excluirPagamento(); break;
            case 0: printf("Voltando ao menu principal...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);
}
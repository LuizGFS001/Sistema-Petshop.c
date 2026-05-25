#include <stdio.h>

#include "menu.h"
#include "Atendimento.h"
#include "cliente.h"
#include "pedido.h"
#include "pagamento.h"
#include "pet.h"
#include "servico.h"
#include "itens_pedido.h"
#include "assinatura.h"
#include "agendamento.h"


void menuPrincipal(){//função para exibir o menu principal do sistema, permitindo ao usuário escolher entre diferentes opções, como iniciar um novo atendimento, acessar consultas, cadastros ou o menu financeiro. A função utiliza um loop para exibir o menu repetidamente até que o usuário escolha a opção de sair. Para cada opção selecionada, a função chama a função correspondente para realizar a ação desejada. Caso haja algum erro durante a execução das consultas SQL, as funções chamadas exibirão mensagens de erro apropriadas.

    int opcao;

    do{

        printf("\n=== SISTEMA UNIPETS ===\n");

        printf("1 Novo Atendimento\n");
        printf("2 Consultas\n");
        printf("3 Cadastro\n");
        printf("4 Financeiro\n");
        printf("5 Assinaturas\n");
        printf("6 Agendamentos\n");
        printf("0 Sair\n");

        printf("Escolha: ");
        scanf("%d",&opcao);

        switch(opcao){

            case 1: NovoAtendimento(); break;
            case 2: menuConsultas(); break;
            case 3: menuCadastro(); break;
            case 4: menuFinanceiro(); break;
            case 5: menuAssinaturas(); break;
            case 6: menuAgendamentos(); break;
            case 0: printf("Encerrando sistema...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);

}

void menuConsultas(){//função para exibir um menu de consultas, permitindo ao usuário escolher entre diferentes tipos de consultas, como buscar clientes, pets, serviços, pedidos ou pagamentos. A função utiliza um loop para exibir o menu repetidamente até que o usuário escolha a opção de voltar. Para cada opção selecionada, a função chama a função correspondente para realizar a consulta e exibir os resultados. Caso haja algum erro durante a execução das consultas SQL, as funções chamadas exibirão mensagens de erro apropriadas.

    int opcao;

    do{
        printf("\n=== CONSULTAS ===\n");

        printf("1 Buscar Clientes\n");
        printf("2 Buscar Pets\n");
        printf("3 Buscar Servicos\n");
        printf("4 Listar Pedidos\n");
        printf("5 Buscar Pedidos\n");
        printf("6 Buscar Pagamentos\n");
        printf("0 Voltar\n");

        scanf("%d", &opcao);

        switch(opcao){
            case 1: buscarCliente(); break;
            case 2: buscarPet(); break;
            case 3: buscarServico(); break;
            case 4: listarPedidos(); break;
            case 5: buscarPedido(); break;
            case 6: buscarPagamento(); break;
        }

    }while(opcao != 0);
}

void menuCadastro(){//função para exibir um menu de cadastro, permitindo ao usuário escolher entre diferentes tipos de cadastros, como cadastrar clientes, pets, serviços ou pedidos. A função utiliza um loop para exibir o menu repetidamente até que o usuário escolha a opção de voltar. Para cada opção selecionada, a função chama a função correspondente para realizar o cadastro e salvar os dados no banco de dados. Caso haja algum erro durante a execução das consultas SQL, as funções chamadas exibirão mensagens de erro apropriadas.

    int opcao;

    do{
        printf("\n=== CADASTRO ===\n");

        printf("1 Clientes\n");
        printf("2 Pets\n");
        printf("3 Servicos\n");
        printf("0 Voltar\n");

        scanf("%d", &opcao);

        switch(opcao){
            case 1: cadastrarCliente(); break;
            case 2: cadastrarPet(); break;
            case 3: cadastrarServico(); break;
        }

    }while(opcao != 0);
}



void menuFinanceiro(){//função para exibir um menu financeiro, permitindo ao usuário escolher entre diferentes opções relacionadas aos pagamentos, como listar os pagamentos cadastrados, buscar pagamentos por critérios específicos ou excluir um pagamento do banco de dados. A função utiliza um loop para exibir o menu repetidamente até que the usuário escolha a opção de voltar. Para cada opção selecionada, a função chama a função correspondente para realizar a ação desejada. Caso haja algum erro durante a execução das consultas SQL, as funções chamadas exibirão mensagens de erro apropriadas.

    int opcao;

    do{
        printf("\n=== FINANCEIRO ===\n");

        printf("1 Listar Pagamentos\n");
        printf("2 Buscar Pagamento\n");
        printf("3 Excluir Pagamento\n");
        printf("0 Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao){
            case 1: listarPagamentos(); break;
            case 2: buscarPagamento(); break;
            case 3: excluirPagamento(); break;
            case 0: printf("Voltando ao menu principal...\n"); break;
            default: printf("Opcao invalida!\n");
        }

    }while(opcao != 0);
}

void menuAssinaturas(){
    int opcao;
    do{
        printf("\n=== ASSINATURAS ===\n");
        printf("1 Nova Assinatura\n");
        printf("2 Listar Assinaturas\n");
        printf("3 Buscar Assinatura\n");
        printf("4 Renovar Assinatura\n");
        printf("5 Cancelar Assinatura\n");
        printf("6 Verificar Beneficios\n");
        printf("7 Relatorio\n");
        printf("0 Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        switch(opcao){
            case 1: {
                int cli_id = 0;
                float val = cadastrarAssinatura(&cli_id);
                if(val > 0 && cli_id > 0) cobrarAssinaturaStandalone(cli_id, val);
                break;
            }
            case 2: listarAssinaturas(); break;
            case 3: buscarAssinatura(); break;
            case 4: renovarAssinatura(); break;
            case 5: cancelarAssinatura(); break;
            case 6: verificarAssinatura(); break;
            case 7: relatorioAssinaturas(); break;
            case 0: printf("Voltando...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);
}

void menuAgendamentos(){
    int opcao;
    do{
        printf("\n=== AGENDAMENTOS ===\n");
        printf("1 Novo Agendamento\n");
        printf("2 Agenda do Dia\n");
        printf("3 Listar Agendamentos\n");
        printf("4 Buscar Agendamento\n");
        printf("5 Confirmar Agendamento\n");
        printf("6 Concluir Agendamento\n");
        printf("7 Cancelar Agendamento\n");
        printf("0 Voltar\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        switch(opcao){
            case 1: criarAgendamento(); break;
            case 2: agendamentosDoDia(); break;
            case 3: listarAgendamentos(); break;
            case 4: buscarAgendamento(); break;
            case 5: confirmarAgendamento(); break;
            case 6: concluirAgendamento(); break;
            case 7: cancelarAgendamento(); break;
            case 0: printf("Voltando...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    }while(opcao != 0);
}
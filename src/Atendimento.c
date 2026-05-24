#include <stdio.h>
#include <sqlite3.h>
#include "Atendimento.h"
#include "pedido.h"
#include "pagamento.h"
#include "pet.h"
#include "servico.h"
#include "itens_pedido.h"
#include "assinatura.h"
#include "../include/cliente.h"
#include "database.h"

void NovoAtendimento() {
    int id_cliente_atual = 999;
    char data_atual[11];

    obterDataAtualBR(data_atual);

    // Recepção
    if (!clienteCadastrado()) {
        if (desejaCadastrar() == 1) {
            id_cliente_atual = cadastrarCliente();

            if(id_cliente_atual > 0){
                printf("\nDeseja cadastrar um pet para este cliente? (1 = sim, 0 = nao): ");
                int op_pet;
                scanf("%d", &op_pet);
                if(op_pet == 1){
                    cadastrarPetAtendimento(id_cliente_atual);
                } else {
                    PetGenerico(id_cliente_atual);
                    printf("Continuando sem pet cadastrado.\n");
                }
            }
        } else {
            continuarSemCadastro();
            id_cliente_atual = 999;
        }
    } else {
        printf("\nBuscando cliente...\n");
        id_cliente_atual = buscarCliente();

        if (id_cliente_atual <= 0) {
            printf("Busca cancelada ou ID invalido. Usando Cliente Avulso.\n");
            id_cliente_atual = 999;
        }
    }

    int pedido_id = OrdemServico(id_cliente_atual, data_atual);
    if (pedido_id <= 0) {
        printf("Nao foi possivel prosseguir com o atendimento.\n");
        return;
    }

    if (id_cliente_atual != 999) {
        printf("\n=== VERIFICAR BENEFICIOS DO CLIENTE ===\n");
        verificarAssinatura();
    } else {
        printf("\nCliente Avulso: Sem assinatura ou beneficios ativos.\n");
    }

    executarServicos(pedido_id);
    emitirResumo(pedido_id);
    cadastrarPagamento(pedido_id);
}
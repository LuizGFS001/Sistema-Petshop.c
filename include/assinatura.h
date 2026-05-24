#ifndef ASSINATURA_H
#define ASSINATURA_H

struct assinatura {

    int id;
    int cliente_id;
    int pet_id;
    char plano[50];
    float valor;
    char data_inicio[20];
    char data_renovacao[20];
    char status[20];
    int pontos;
};

void criarTabelaAssinatura();
void cadastrarAssinatura();
void listarAssinaturas();
void buscarAssinatura();
void renovarAssinatura();
void cancelarAssinatura();
void adicionarPontos(int cliente_id, int pontos);
void verificarAssinatura();
void relatorioAssinaturas();

#endif
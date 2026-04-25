#ifndef PET_H
#define PET_H

struct pet{
    int id;
    char nome[60];
    char especie[40];
    char raca[40];
    int idade;
    int cliente_id;
};

void criarTabelaPets();
void cadastrarPet();
void listarPets();
void buscarPet();
void alterarPet();
void excluirPet();

#endif
#ifndef PET_H
#define PET_H

struct pet{
    int id;
    char nome[50];
    char especie[30];
    char raca[30];
    int idade;
    int cliente_id;

};

void criarTabelaPets();
void cadastrarPet();
void listarPets();
void buscarPet();
void excluirPet();

#endif
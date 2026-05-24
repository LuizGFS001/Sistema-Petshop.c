#include <stdio.h>
#include "cliente.h"
#include "database.h"
#include "../sqlite/sqlite3.h" //biblioteca do sqlite, de onde vem as funções que usaremos ao decorrer do codigo


void criartabelaclientes(){//função para criar a tabela clientes no banco de dados, caso ela ainda não exista. A função utiliza um comando SQL para definir a estrutura da tabela, incluindo os campos id, nome, cpf, email, endereco e telefone. O campo id é definido como chave primária e autoincremento para garantir que cada cliente tenha um identificador único. A função também inclui tratamento de erros para informar caso haja algum problema durante a criação da tabela.

    char *sql = 
    "CREATE TABLE IF NOT EXISTS clientes ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "nome TEXT,"
    "cpf TEXT,"
    "email TEXT,"
    "endereco TEXT,"
    "telefone TEXT);";
    //comando do sql para a criação da tabela com os parâmetros indicados por exemplo, id interger armazena apenas valores inteiros, é uma Primary key indicando que é única e autoincrement significa que novos valores irão ser sequencial (ex: 1, 2, 3, 4...)

    char *erro = 0; // variavel de erro caso ocorra algum erro durante a criação da tabela 
    int rc = sqlite3_exec(db, sql, 0, 0, &erro); //a função sqlite3_exec é a função pricipal da biblioteca do sqlite3 onde recebe como parâmetros (db = banco, sql = comando, 0 = callback(sem callback no nosso caso), dados, e o endereço da variavel de erro &erro)
    //A variavel rc significa returncode
    if(rc != SQLITE_OK){ // a variavel rc que armazena  execução do sqlite, e nessa linha vamos conferir se deu tudo certo onde se rc que recebe a execução der diferente de sqlike_ok ele nos mostrará que não funciou retornando a variavel erro que armazena o erro
        printf("Erro ao criar a tabela: %s\n", erro);
        sqlite3_free(erro);
    }
}

void saveClientDB(struct cliente c){//salvar clientes

    char sql[800]; //definição da quantidade de caracteres para montagem das querys no sql

    sprintf(sql, //formatação da string com o comando para inserir na tabela criada acima com os parâmetros indicados abaixo correspondentes aos parâmetros na criação da tabela acima.
    "INSERT INTO clientes (nome, cpf, email, endereco, telefone)"
    " VALUES('%s','%s','%s','%s','%s');",
    c.nome,
    c.cpf,
    c.email,
    c.endereco,
    c.telefone
    );

    char *erro = 0; //variavel de erro para apontar se houve sucesso ou erro durante o salvamento no banco de dados

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao inserir cliente: %s\n", erro);
        sqlite3_free(erro);
    }
    else{
        printf("Cliente cadastro feito com sucesso!\n");
    }
}

int callback(void *data, int argcount, char **argvetor, char **vtColName){ //callback para processar os resultados das consultas SQL, imprimindo as informações de cada cliente no console. A função recebe como parâmetros um ponteiro para dados adicionais (data), o número de colunas retornadas (argcount), um vetor de strings contendo os valores das colunas (argvetor) e um vetor de strings contendo os nomes das colunas (vtColName). A função itera sobre as colunas e imprime o nome e o valor de cada coluna, formatando a saída para facilitar a leitura. A função retorna 0 para indicar que a execução foi bem-sucedida.
    
    for(int i = 0; i < argcount; i++){
        printf("%s: %s\n", //o printf vai imprimir o nome e o valor da coluna
            vtColName[i],
            argvetor[i] ? argvetor[i] : "NULL"); // <<<< operação ternaria onde se o valor dentro da tabela for nulo(não tiver nada) vai retornar a str "NULL"
    }
    printf("--------------------------\n");
    return 0;
}

int clienteCadastrado() {//função para verificar se o cliente já está cadastrado no banco de dados, utilizando uma consulta SQL para contar o número de registros na tabela de clientes. A função executa a consulta usando sqlite3_exec e um callback para processar o resultado, retornando 1 se houver pelo menos um cliente cadastrado e 0 caso contrário. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro e retorna 0.
    int resposta;
    printf("Cliente cadastrado? (1 = sim, 0 = nao): ");
    scanf("%d", &resposta);
    return resposta;
}

void ClienteGenerico(){//Cria um Cliente genérico para facilitar o processo de atendimento sem a necessidade de cadastrar um cliente real, evitando assim o uso de dados pessoais e respeitando a LGPD. O cliente genérico tem um ID fixo e informações padrão que indicam que é um cliente avulso, sem dados reais associados.

    char *sql =
    "INSERT OR IGNORE INTO clientes (id, nome, cpf, email, endereco, telefone) "
    "VALUES (999, 'Cliente Avulso', '000.000.000-00', 'avulso@unipets.com', 'Nao cadastrado', '000000000');";

    char *erro = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao criar cliente generico: %s\n", erro);
        sqlite3_free(erro);
    }
}

int cadastrarCliente(){//cadastro do cliente

    struct cliente novo; 
    
    
    printf("\nNome: ");
    scanf(" %[^\n]", novo.nome); // o espaço antes do % ignora e quebra linhas anteriores, a expressão "%[^\n] é uma função da biblioteca string.h que lê tud odentro de [] o "^" é uma negação que não permite o uso do enter. Então %[^\n] lê tudo até o usuario apertar enter.

    printf("CPF: ");
    scanf(" %[^\n]", novo.cpf);

    printf("Email: ");
    scanf(" %[^\n]", novo.email);

    printf("Endereço: ");
    scanf(" %[^\n]", novo.endereco);

    printf("Telefone: ");   
    scanf(" %[^\n]", novo.telefone);

    saveClientDB(novo);

    int id_gerado = (int)sqlite3_last_insert_rowid(db);
    return id_gerado;

}

int desejaCadastrar() {//funcão para perguntar ao cliente se deseja se cadastrar, retornando 1 para sim e 0 para não. Essa função é utilizada no processo de atendimento para permitir que o cliente escolha se deseja fornecer seus dados para cadastro ou continuar como cliente avulso, garantindo flexibilidade no atendimento e respeitando a privacidade do cliente.
    int op;
    printf("Deseja se cadastrar? (1 = sim, 0 = nao): ");
    scanf("%d", &op);
    return op;
}

void continuarSemCadastro() {
    char *sql = 
    "INSERT OR IGNORE INTO clientes (id, nome, cpf, email, endereco, telefone) "
    "VALUES (999, 'Cliente Avulso', '000.000.000-00', 'avulso@unipets.com', 'Nao cadastrado', '000000000');";
    
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    
    if (rc != SQLITE_OK) {
        printf("Erro ao inicializar cliente avulso: %s\n", erro);
        sqlite3_free(erro);
    } else {
        printf("Continuando atendimento sem cadastro...\n");
    }
}

void listarClientes(){//função para listar os clientes cadastrados no banco de dados, utilizando uma consulta SQL para selecionar todos os registros da tabela de clientes. A função utiliza a função sqlite3_exec para executar a consulta e um callback para processar os resultados, imprimindo as informações de cada cliente no console. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.

    char *sql = "SELECT * FROM clientes"; //Query do sql para puxar toda a tabela clientes 

    char *erro = 0;

    printf("\n========== CLIENTES ==========\n");

    int rc = sqlite3_exec(db, sql, callback, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao buscar clientes: %s\n", erro);
        sqlite3_free(erro);
    }
}

int buscarCliente(){//função para buscar clientes no banco de dados, permitindo ao usuário escolher o critério de busca (ID, nome, telefone ou CPF) e inserindo o valor correspondente. A função constrói uma consulta SQL com base na escolha do usuário e executa a consulta usando sqlite3_exec, utilizando um callback para processar os resultados. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro.

    int opcao;
    char valor[100];
    char sql[300];
    int id_selecionado = 0;

    printf("\nBuscar cliente por:\n");
    printf("1 ID\n");
    printf("2 Nome\n");
    printf("3 Telefone\n");
    printf("4 CPF\n");

    printf("Escolha: ");
    scanf("%d",&opcao);

    while(getchar() != '\n'); // limpeza de buffer

    if(opcao == 1)       printf("Digite o ID: ");
    else if(opcao == 2)  printf("Digite o Nome: ");
    else if(opcao == 3)  printf("Digite o Telefone: ");
    else if(opcao == 4)  printf("Digite o CPF: ");
    else { printf("Opcao invalida!\n"); return 999; }

    scanf("%[^\n]", valor);

    if(opcao == 1){
        sprintf(sql,
        "SELECT * FROM clientes WHERE id = %s ;",
        valor);
    }

    else if(opcao == 2){
        sprintf(sql,
        "SELECT * FROM clientes WHERE nome LIKE '%%%s%%';",
        valor);
    }

    else if(opcao == 3){
        sprintf(sql,
        "SELECT * FROM clientes WHERE telefone LIKE '%%%s%%';",
        valor);
    }
    else if(opcao == 4){
        sprintf(sql,
        "SELECT * FROM clientes WHERE cpf='%s';",
        valor);
    }

    char *errMsg = 0;
    sqlite3_exec(db, sql, callback, 0, &errMsg);

    printf("Confirme o ID do cliente desejado: ");
    scanf("%d", &id_selecionado);
    while(getchar() != '\n');

    return id_selecionado;

}

void alterarCliente(){//função para alterar os dados de um cliente existente no banco de dados. A função solicita ao usuário o ID do cliente que deseja alterar e o campo específico que deseja modificar (nome, CPF, email, endereço ou telefone). Em seguida, o usuário insere o novo valor para o campo selecionado. A função constrói uma consulta SQL de atualização com base na escolha do usuário e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que a atualização foi bem-sucedida.
    int id;  //declaração de variaveis
    int opcao;
    char valor[100];

    buscarCliente(); // aqui chamamos a função de busca do cliente para retornar o id do cliente para a facilidar a identificação da id do cliente para alterar o dado

    printf("Digite o ID do cliente que deseja alterar: ");
    scanf("%d",&id);

    printf("\n=== ALTERAR CLIENTE ===\n");

    printf("1 Alterar nome\n");
    printf("2 Alterar CPF\n");
    printf("3 Alterar email\n");
    printf("4 Alterar endereco\n");
    printf("5 Alterar telefone\n");

    printf("Escolha: ");
    scanf("%d",&opcao);

    printf("Digite o novo valor: ");
    scanf(" %[^\n]", valor);

    char sql[300]; //definição da quantidade de caracteres para montagem das querys no sql

    switch(opcao){

        case 1: sprintf(sql,"UPDATE clientes SET nome='%s' WHERE id=%d;",valor,id); break;
        case 2: sprintf(sql,"UPDATE clientes SET cpf='%s' WHERE id=%d;",valor,id); break;
        case 3: sprintf(sql,"UPDATE clientes SET email='%s' WHERE id=%d;",valor,id); break;
        case 4: sprintf(sql,"UPDATE clientes SET endereco='%s' WHERE id=%d;",valor,id); break;
        case 5: sprintf(sql,"UPDATE clientes SET telefone='%s' WHERE id=%d;",valor,id); break;
        default:
            printf("Opcao invalida\n");
            return;
    }

    char *errMsg = 0; //verificação de erro

    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);

    if(rc != SQLITE_OK){
        printf("Erro SQL: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    else{
        printf("Cliente atualizado com sucesso!\n");
    }
}

void excluirCliente(){//função para excluir um cliente do banco de dados, solicitando ao usuário o ID do cliente que deseja excluir. A função inclui uma confirmação de exclusão para evitar exclusões acidentais. Se o usuário confirmar a exclusão, a função constrói uma consulta SQL de exclusão e executa a consulta usando sqlite3_exec. Caso haja algum erro durante a execução da consulta, a função exibe uma mensagem de erro; caso contrário, confirma que o cliente foi excluído com sucesso.

    int id;
    char confirm;
    char sql[200];

    buscarCliente();

    printf("\nDigite o ID do cliente que deseja excluir: ");
    scanf("%d",&id);

    printf("Tem certeza que deseja excluir este cliente? (s/n): "); //confirmação de exclusão
    scanf(" %c",&confirm);

    if(confirm != 's' && confirm != 'S'){
        printf("Operacao cancelada.\n");
        return;
    }

    sprintf(sql,"DELETE FROM clientes WHERE id=%d;",id);

    char *erro = 0;

    int rc = sqlite3_exec(db, sql, 0, 0, &erro);

    if(rc != SQLITE_OK){
        printf("Erro ao excluir cliente: %s\n",erro);
        sqlite3_free(erro);
    }
    else{
        printf("Cliente excluido com sucesso!\n");
    }

}



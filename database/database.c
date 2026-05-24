#include <stdio.h>
#include <time.h>
#include "../include/cliente.h"
#include "database.h"
#include "../sqlite/sqlite3.h"

sqlite3 *db;

// função interna para injetar os produtos e serviços automaticamente
void popularServicosEProdutos() {
    char *sql = 
    "INSERT OR IGNORE INTO servicos (id, nome, descricao, preco, estoque) VALUES "
    "(1, 'Banho', 'Banho completo com higienizacao', 50.00, -1),"
    "(2, 'Tosa', 'Tosa completa conforme a raca', 60.00, -1),"
    "(3, 'Tosa higienica', 'Tosa das regioes sanitarias e patas', 40.00, -1),"
    "(4, 'Consulta veterinaria', 'Consulta presencial com medico veterinario', 120.00, -1),"
    "(5, 'Vacinacao', 'Aplicacao de vacinas essenciais', 80.00, -1),"
    "(6, 'Aplicacao de medicamentos', 'Injecao ou administracao de remedios', 25.00, -1),"
    "(7, 'Hospedagem para pets', 'Diaria de hotelzinho para pet', 90.00, -1),"
    "(8, 'Creche para pets', 'Periodo de creche/daycare diario', 45.00, -1),"
    "(9, 'Passeio para caes', 'Sessao de dog walking', 35.00, -1),"
    "(10, 'Adestramento', 'Aula de adestramento comportamental', 100.00, -1),"
    "(11, 'Transporte pet', 'Servico de taxi dog leva e traz', 30.00, -1),"
    "(12, 'Atendimento veterinario domiciliar', 'Consulta feita na residencia do cliente', 180.00, -1),"
    "(13, 'Limpeza de ouvido', 'Higienizacao do conduto auditivo', 20.00, -1),"
    "(14, 'Corte de unhas', 'Aparagem segura das unhas do pet', 15.00, -1),"
    "(15, 'Hidratacao de pelos', 'Tratamento de pelagem pós-banho', 40.00, -1),"
    "(16, 'Escovacao dental', 'Escovacao e aplicacao de gel dental', 20.00, -1),"
    "(17, 'Microchipagem', 'Aplicacao de microchip de identificacao', 110.00, -1),"
    "(18, 'Exames laboratoriais', 'Coleta para analises clinicas', 70.00, -1),"
    "(19, 'Cirurgias veterinarias', 'Procedimentos cirurgicos gerais', 500.00, -1),"
    "(20, 'Atendimento emergencial veterinario', 'Plantao de urgencia e emergencia', 200.00, -1),"
    "(21, 'Racao para caes', 'Alimento completo para caes', 89.90, 100),"
    "(22, 'Racao para gatos', 'Alimento completo para gatos', 79.90, 100),"
    "(23, 'Petiscos', 'Snacks e bifinhos para agrado', 12.50, 100),"
    "(24, 'Brinquedos para pets', 'Mordedores e brinquedos interativos', 25.00, 100),"
    "(25, 'Caminha para pets', 'Cama confortavel tamanho padrao', 120.00, 100),"
    "(26, 'Coleiras', 'Coleira ajustavel de nylon', 18.00, 100),"
    "(27, 'Guias para passeio', 'Guia reforcada com mosquetao', 29.90, 100),"
    "(28, 'Peitorais', 'Coleira peitoral ergonomica', 45.00, 100),"
    "(29, 'Comedouros', 'Pote de alimentacao inox', 22.00, 100),"
    "(30, 'Bebedouros', 'Pote ou fonte de agua para pets', 25.00, 100),"
    "(31, 'Caixa de transporte', 'Caixa segura para viagens e consultas', 95.00, 100),"
    "(32, 'Caixa de areia para gatos', 'Bandeja plastica para dejetos', 30.00, 100),"
    "(33, 'Areia sanitaria', 'Pacote de areia absorvente para gatos', 19.90, 100),"
    "(34, 'Shampoo pet', 'Shampoo neutro para caes e gatos', 24.90, 100),"
    "(35, 'Condicionador pet', 'Condicionador suave para pelos', 24.90, 100),"
    "(36, 'Escova para pelos', 'Rascadeira para remocao de pelos mortos', 15.00, 100),"
    "(37, 'Cortador de unhas', 'Alicate especifico para unhas pet', 35.00, 100),"
    "(38, 'Tapete higienico', 'Pacote de tapetes descartaveis', 59.90, 100),"
    "(39, 'Roupas para pets', 'Roupinhas de frio tamanhos variados', 40.00, 100),"
    "(40, 'Medicamentos veterinarios', 'Remedios gerais sob prescricao', 50.00, 100),"
    "(41, 'Antipulgas', 'Tablete ou pipeta de protecao', 85.00, 100),"
    "(42, 'Vermifugos', 'Comprimido de amplo espectro', 28.00, 100),"
    "(43, 'Perfume pet', 'Fragrancia suave pós-banho', 18.90, 100),"
    "(44, 'Lencos umedecidos para pets', 'Lencos higienizadores sem alcool', 14.50, 100),"
    "(45, 'Arranhador para gatos', 'Arranhador de sisal com brinquedo', 75.00, 100),"
    "(46, 'Casinhas para pets', 'Casa plastica para area externa', 190.00, 100),"
    "(47, 'Aquarios', 'Aquario de vidro basico', 130.00, 100),"
    "(48, 'Produtos para aquarismo', 'Racao para peixes e anticloro', 15.00, 100),"
    "(49, 'Gaiolas para aves', 'Gaiola metalica malha fina', 85.00, 100),"
    "(50, 'Produtos para roedores', 'Serragem e racao prensada', 12.00, 100);";

    char *erro = 0;
    sqlite3_exec(db, sql, 0, 0, &erro);
    if(erro) {
        sqlite3_free(erro); // Ignora silenciosamente se já existirem os IDs
    }
}

int abrirDB(){
    int rc = sqlite3_open("Petshop.db", &db);

    if(rc){
        printf("Erro ao abrir o banco: %s\n", sqlite3_errmsg(db));
        return 1; 
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0); 

    sqlite3_exec(db,
    "CREATE TABLE IF NOT EXISTS uso_beneficios ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "assinatura_id INTEGER,"
    "servico_id INTEGER,"
    "mes INTEGER,"
    "ano INTEGER,"
    "FOREIGN KEY(assinatura_id) REFERENCES assinatura(id));",
    0, 0, 0);
    
    printf("Banco conectado com sucesso!\n");
    
    // Cria a tabela, ativa os gatilhos de estoque e injeta a lista automaticamente
    criarTabelaServicos();
    criarTriggerEstoque();
    popularServicosEProdutos();

    // migration: adiciona colunas novas caso o banco já existia sem elas
    sqlite3_exec(db, "ALTER TABLE servicos ADD COLUMN tipo TEXT DEFAULT 'SERVICO';", 0, 0, 0);
    sqlite3_exec(db, "ALTER TABLE servicos ADD COLUMN estoque_minimo INTEGER DEFAULT 0;", 0, 0, 0);

    return 0;
}

void obterDataAtualBR(char *buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d-%02d-%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900); 
}

void criarTriggerEstoque(){
    char *sql =
    "CREATE TRIGGER IF NOT EXISTS dar_baixa_estoque "
    "AFTER INSERT ON itens_pedido "
    "FOR EACH ROW "
    "BEGIN "
    "   UPDATE servicos "
    "   SET estoque = estoque - NEW.quantidade "
    "   WHERE id = NEW.servico_id AND estoque > -1; "
    "END;";
    
    char *erro = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &erro);
    if(rc != SQLITE_OK){ printf("Erro ao criar trigger: %s\n", erro); sqlite3_free(erro); }
}

void fecharDB(){
    sqlite3_close(db);
}
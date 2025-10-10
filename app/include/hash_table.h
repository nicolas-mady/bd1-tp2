#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "artigo.h"
#include <fstream>
#include <vector>

class HashTable
{
    private:
    std::string filename;
    int table_size;
    int record_size;
    int records_per_block;

    public:
    HashTable(const std::string &filename, int table_size);
    ~HashTable();

    // Inicializa o arquivo hash
    bool initialize();

    // Insere um registro
    bool insert(const Artigo &artigo);

    // Busca um registro por ID
    BuscaEstatisticas search(int id, Artigo &artigo);

    // Obtém estatísticas do arquivo
    int getTotalBlocks();

    private:
    // Calcula a posição no arquivo para um ID
    long getPosition(int id);

    // Lê um bloco do arquivo
    bool readBlock(int block_num, char *buffer);

    // Escreve um bloco no arquivo
    bool writeBlock(int block_num, const char *buffer);

    // Encontra posição livre em um bloco para inserção
    bool findFreeSlot(int block_num, int &slot);

    // Verifica se arquivo existe
    bool fileExists();
};

#endif // HASH_TABLE_H
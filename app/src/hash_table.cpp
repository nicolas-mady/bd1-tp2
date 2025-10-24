#include "hash_table.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

HashTable::HashTable(const std::string &filename, int table_size)
    : filename(filename), table_size(table_size) {
  record_size = Artigo::getRecordSize();
  records_per_block = BLOCK_SIZE / record_size;
  if (records_per_block == 0)
    records_per_block = 1;
}

HashTable::~HashTable() {
  // Destrutor - não há recursos específicos para limpar
}

bool HashTable::initialize() {
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Erro ao criar arquivo hash: " << filename << std::endl;
    return false;
  }

  // Inicializa arquivo com blocos vazios
  char empty_block[BLOCK_SIZE];
  memset(empty_block, 0, BLOCK_SIZE);

  int total_blocks = (table_size + records_per_block - 1) / records_per_block;

  for (int i = 0; i < total_blocks; i++) {
    file.write(empty_block, BLOCK_SIZE);
  }

  file.close();

  std::cout << "Arquivo hash inicializado: " << filename << std::endl;
  std::cout << "Tamanho da tabela: " << table_size << " registros" << std::endl;
  std::cout << "Total de blocos: " << total_blocks << std::endl;
  std::cout << "Registros por bloco: " << records_per_block << std::endl;

  return true;
}

bool HashTable::insert(const Artigo &artigo) {
  int hash_value = Artigo::hash_id(artigo.id, table_size);
  int block_num = hash_value / records_per_block;

  char buffer[BLOCK_SIZE];
  if (!readBlock(block_num, buffer)) {
    return false;
  }

  // Procura slot livre no bloco
  int slot;
  if (!findFreeSlot(block_num, slot)) {
    // Tratamento de colisão - linear probing
    int original_block = block_num;
    int total_blocks = getTotalBlocks();

    do {
      block_num = (block_num + 1) % total_blocks;
      if (findFreeSlot(block_num, slot)) {
        break;
      }
    } while (block_num != original_block);

    if (block_num == original_block) {
      std::cerr << "Tabela hash cheia!" << std::endl;
      return false;
    }

    // Lê o novo bloco
    if (!readBlock(block_num, buffer)) {
      return false;
    }
  }

  // Insere o registro no slot encontrado
  char *slot_position = buffer + slot * record_size;

  // Marca como ocupado (primeiro int = ID do registro)
  memcpy(slot_position, &artigo.id, sizeof(int));

  // Serializa o resto do registro
  std::ofstream temp_stream("/tmp/temp_serialize", std::ios::binary);
  artigo.serialize(temp_stream);
  temp_stream.close();

  std::ifstream temp_read("/tmp/temp_serialize", std::ios::binary);
  temp_read.read(slot_position, record_size);
  temp_read.close();

  // Remove arquivo temporário
  unlink("/tmp/temp_serialize");

  return writeBlock(block_num, buffer);
}

SearchStats HashTable::search(int id, Artigo &artigo) {
  SearchStats stats;
  stats.total_blocos = getTotalBlocks();

  int hash_value = Artigo::hash_id(id, table_size);
  int block_num = hash_value / records_per_block;
  int original_block = block_num;

  char buffer[BLOCK_SIZE];

  do {
    stats.blocos_lidos++;

    if (!readBlock(block_num, buffer)) {
      return stats;
    }

    // Verifica todos os slots do bloco
    for (int slot = 0; slot < records_per_block; slot++) {
      int offset = slot * record_size;

      // Lê o ID do slot (primeiro int)
      int stored_id;
      memcpy(&stored_id, buffer + offset, sizeof(int));

      // Verifica se slot está ocupado e se o ID confere
      if (stored_id == id) {
        // Deserializa o registro
        std::ofstream temp_write("/tmp/temp_deserialize", std::ios::binary);
        temp_write.write(buffer + offset, record_size);
        temp_write.close();

        std::ifstream temp_read("/tmp/temp_deserialize", std::ios::binary);
        artigo.deserialize(temp_read);
        temp_read.close();

        unlink("/tmp/temp_deserialize");

        stats.encontrado = true;

        return stats;
      }
    }

    // Linear probing para o próximo bloco
    block_num = (block_num + 1) % stats.total_blocos;

  } while (block_num != original_block);

  return stats;
}

int HashTable::getTotalBlocks() {
  if (!fileExists())
    return 0;

  struct stat st;
  if (stat(filename.c_str(), &st) != 0) {
    return 0;
  }

  return st.st_size / BLOCK_SIZE;
}

long HashTable::getPosition(int id) {
  int hash_value = Artigo::hash_id(id, table_size);
  int block_num = hash_value / records_per_block;
  int slot = hash_value % records_per_block;

  return (long)block_num * BLOCK_SIZE + slot * record_size;
}

bool HashTable::readBlock(int block_num, char *buffer) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Erro ao abrir arquivo para leitura: " << filename
              << std::endl;
    return false;
  }

  file.seekg(block_num * BLOCK_SIZE);
  file.read(buffer, BLOCK_SIZE);

  if (file.gcount() != BLOCK_SIZE) {
    // Bloco pode estar no final do arquivo - preenche com zeros
    memset(buffer + file.gcount(), 0, BLOCK_SIZE - file.gcount());
  }

  file.close();
  return true;
}

bool HashTable::writeBlock(int block_num, const char *buffer) {
  std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
  if (!file) {
    std::cerr << "Erro ao abrir arquivo para escrita: " << filename
              << std::endl;
    return false;
  }

  file.seekp(block_num * BLOCK_SIZE);
  file.write(buffer, BLOCK_SIZE);
  file.close();

  return true;
}

bool HashTable::findFreeSlot(int block_num, int &slot) {
  char buffer[BLOCK_SIZE];
  if (!readBlock(block_num, buffer)) {
    return false;
  }

  for (int i = 0; i < records_per_block; i++) {
    int offset = i * record_size;
    int stored_id;
    memcpy(&stored_id, buffer + offset, sizeof(int));

    if (stored_id == 0) { // Slot livre (ID = 0)
      slot = i;
      return true;
    }
  }

  return false; // Bloco cheio
}

bool HashTable::fileExists() { return access(filename.c_str(), F_OK) == 0; }
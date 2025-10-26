#include "hash_table.h"
#include <cstring>
#include <fstream>
#include <iostream>

// Fixed table size based on the known CSV size (1021439 records)
#define FIXED_TABLE_SIZE 1500000 // ~1.5M slots for load factor of ~0.68

HashTable::HashTable(const std::string& filename, int table_size) : filename(filename), table_size(FIXED_TABLE_SIZE) {
  record_size = Artigo::getRecordSize();
  records_per_block = BLOCK_SIZE / record_size;
  if (records_per_block == 0)
    records_per_block = 1;
}

HashTable::~HashTable() {
  // Destructor - no specific resources to clean
}

bool HashTable::initialize() {
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Erro ao criar arquivo hash: " << filename << std::endl;
    return false;
  }

  // Initialize file with empty blocks
  char empty_block[BLOCK_SIZE];
  memset(empty_block, 0, BLOCK_SIZE);

  int total_blocks = (table_size + records_per_block - 1) / records_per_block;

  for (int i = 0; i < total_blocks; i++)
    file.write(empty_block, BLOCK_SIZE);

  file.close();

  std::cout << "Arquivo hash inicializado: " << filename << std::endl;
  std::cout << "Tamanho da tabela: " << table_size << " registros" << std::endl;
  std::cout << "Total de blocos: " << total_blocks << std::endl;
  std::cout << "Registros por bloco: " << records_per_block << std::endl;

  return true;
}

bool HashTable::insert(const Artigo& artigo) {
  int hash_value = Artigo::hash_id(artigo.id, table_size);
  int block_num = hash_value / records_per_block;

  char buffer[BLOCK_SIZE];
  if (!readBlock(block_num, buffer))
    return false;

  // Simple linear probing for collision resolution
  int slot;
  int original_block = block_num;
  int total_blocks = getTotalBlocks();

  do {
    if (findFreeSlot(block_num, slot)) {
      // Read the block if we moved to a different one
      if (block_num != hash_value / records_per_block) {
        if (!readBlock(block_num, buffer))
          return false;
      }
      break;
    }
    block_num = (block_num + 1) % total_blocks;
  } while (block_num != original_block);

  if (block_num == original_block && !findFreeSlot(block_num, slot)) {
    std::cerr << "Hash table full!" << std::endl;
    return false;
  }

  // Insert record at found slot
  char* slot_position = buffer + slot * record_size;

  // Simple serialization - store ID first, then the title
  memcpy(slot_position, &artigo.id, sizeof(int));
  // Copy the title (char array)
  memcpy(slot_position + sizeof(int), artigo.titulo,
         std::min((size_t) (record_size - sizeof(int)), (size_t) MAX_TITULO));

  return writeBlock(block_num, buffer);
}

BuscaEstatisticas HashTable::search(int id, Artigo& artigo) {
  BuscaEstatisticas stats;
  stats.total_blocos = getTotalBlocks();

  int hash_value = Artigo::hash_id(id, table_size);
  int block_num = hash_value / records_per_block;
  int original_block = block_num;

  char buffer[BLOCK_SIZE];

  do {
    stats.blocos_lidos++;

    if (!readBlock(block_num, buffer))
      return stats;

    // Check all slots in block
    for (int slot = 0; slot < records_per_block; slot++) {
      int offset = slot * record_size;
      int stored_id;
      memcpy(&stored_id, buffer + offset, sizeof(int));

      if (stored_id == id) {
        // Simple deserialization - get the title back
        artigo.id = stored_id;
        // Copy title from buffer to artigo struct
        memcpy(artigo.titulo, buffer + offset + sizeof(int),
               std::min((size_t) MAX_TITULO, (size_t) (record_size - sizeof(int))));
        artigo.titulo[MAX_TITULO] = '\0'; // Ensure null termination

        stats.encontrado = true;
        return stats;
      }
    }

    // Linear probing to next block
    block_num = (block_num + 1) % stats.total_blocos;

  } while (block_num != original_block);

  return stats;
}

int HashTable::getTotalBlocks() {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (!file)
    return 0;

  long file_size = file.tellg();
  file.close();
  return file_size / BLOCK_SIZE;
}

long HashTable::getPosition(int id) {
  int hash_value = Artigo::hash_id(id, table_size);
  int block_num = hash_value / records_per_block;
  int slot = hash_value % records_per_block;

  return (long) block_num * BLOCK_SIZE + slot * record_size;
}

bool HashTable::readBlock(int block_num, char* buffer) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Erro ao abrir arquivo para leitura: " << filename << std::endl;
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

bool HashTable::writeBlock(int block_num, const char* buffer) {
  std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
  if (!file) {
    std::cerr << "Erro ao abrir arquivo para escrita: " << filename << std::endl;
    return false;
  }

  file.seekp(block_num * BLOCK_SIZE);
  file.write(buffer, BLOCK_SIZE);
  file.close();

  return true;
}

bool HashTable::findFreeSlot(int block_num, int& slot) {
  char buffer[BLOCK_SIZE];
  if (!readBlock(block_num, buffer))
    return false;

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

bool HashTable::fileExists() {
  std::ifstream file(filename);
  return file.good();
}

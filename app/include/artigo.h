#ifndef ARTIGO_H
#define ARTIGO_H

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

// Tamanhos máximos dos campos
#define MAX_TITULO 300
#define MAX_AUTORES 150
#define MIN_SNIPPET 100
#define MAX_SNIPPET 1024
#define BLOCK_SIZE 4096

// Estrutura do registro de artigo
struct Artigo
{
  int id;
  char titulo[MAX_TITULO + 1];
  int ano;
  char autores[MAX_AUTORES + 1];
  int citacoes;
  time_t atualizacao;
  char snippet[MAX_SNIPPET + 1];

  // Construtor padrão
  Artigo();

  // Construtor com parâmetros
  Artigo(int id, const std::string &titulo, int ano, const std::string &autores,
         int citacoes, const std::string &data_atualizacao,
         const std::string &snippet);

  // Métodos para serialização
  void serialize(std::ostream &out) const;
  void deserialize(std::istream &in);

  // Método para imprimir registro
  void print() const;

  // Método para calcular hash do ID
  static int hash_id(int id, int table_size);

  // Método para obter tamanho do registro
  static size_t getRecordSize();
};

// Estrutura para estatísticas de busca
struct BuscaEstatisticas
{
  int blocos_lidos;
  int total_blocos;
  bool encontrado;

  BuscaEstatisticas() : blocos_lidos(0), total_blocos(0), encontrado(false) {}
};

// Estrutura para entrada do índice primário (ID -> posição no arquivo hash)
struct PrimIdxEntry
{
  int id;
  long posicao_hash;

  PrimIdxEntry() : id(0), posicao_hash(0) {}
  PrimIdxEntry(int id, long pos) : id(id), posicao_hash(pos) {}
};

// Estrutura para entrada do índice secundário (Título -> ID)
struct SecIdxEntry
{
  char titulo[MAX_TITULO + 1];
  int id;

  SecIdxEntry() : id(0) { std::memset(titulo, 0, sizeof(titulo)); }

  SecIdxEntry(const std::string &t, int i) : id(i)
  {
    std::strncpy(titulo, t.c_str(), MAX_TITULO);
    titulo[MAX_TITULO] = '\0';
  }
};

// Funções utilitárias
std::string trim(const std::string &str);
time_t parseDateTime(const std::string &datetime_str);
std::string formatDateTime(time_t timestamp);

#endif // ARTIGO_H
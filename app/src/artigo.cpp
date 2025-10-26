// #include "artigo.h"
// #include <cstring>

// Record::Record(
//     int id,
//     const char* title,
//     int year,
//     const char* authors,
//     int cites,
//     long dateTime,
//     const char* snippet
// ) : id(id), year(year), cites(cites), dateTime(dateTime) {
//     strncpy(this->title, title, MAX_TITLE);
//     // this->title[MAX_TITLE - 1] = '\0';
//     strncpy(this->authors, authors, MAX_AUTHORS);
//     // this->authors[MAX_AUTHORS - 1] = '\0';
//     strncpy(this->snippet, snippet, MAX_SNIPPET);
//     // this->snippet[MAX_SNIPPET - 1] = '\0';
// }

// // Implementação do construtor padrão
// Artigo::Artigo() : id(0), ano(0), citacoes(0), atualizacao(0) {
//   memset(titulo, 0, sizeof(titulo));
//   memset(autores, 0, sizeof(autores));
//   memset(snippet, 0, sizeof(snippet));
// }

// // Implementação do construtor com parâmetros
// Artigo::Artigo(
//   int id,
//   const std::string &titulo,
//   int ano,
//   const std::string &autores,
//   int citacoes,
//   const std::string &update,
//   const std::string &snippet
// ) : id(id), ano(ano), citacoes(citacoes) {

//   // Copia título (limitando tamanho)
//   strncpy(this->titulo, titulo.c_str(), MAX_TITULO);
//   this->titulo[MAX_TITULO] = '\0';

//   // Copia autores (limitando tamanho)
//   strncpy(this->autores, autores.c_str(), MAX_AUTORES);
//   this->autores[MAX_AUTORES] = '\0';

//   // Copia snippet (limitando tamanho)
//   strncpy(this->snippet, snippet.c_str(), MAX_SNIPPET);
//   this->snippet[MAX_SNIPPET] = '\0';

//   // Converte data/hora
//   this->atualizacao = parseDateTime(update);
// }

// // Serialização do registro
// void Artigo::serialize(std::ostream &out) const {
//   out.write(reinterpret_cast<const char *>(&id), sizeof(id));
//   out.write(titulo, sizeof(titulo));
//   out.write(reinterpret_cast<const char *>(&ano), sizeof(ano));
//   out.write(autores, sizeof(autores));
//   out.write(reinterpret_cast<const char *>(&citacoes), sizeof(citacoes));
//   out.write(reinterpret_cast<const char *>(&atualizacao), sizeof(atualizacao));
//   out.write(snippet, sizeof(snippet));
// }

// // Deserialização do registro

// // Imprime o registro formatado
// void Artigo::print() const {
//   std::cout << "ID: " << id << std::endl;
//   std::cout << "Título: " << titulo << std::endl;
//   std::cout << "Ano: " << ano << std::endl;
//   std::cout << "Autores: " << autores << std::endl;
//   std::cout << "Citações: " << citacoes << std::endl;
//   std::cout << "Atualização: " << formatDateTime(atualizacao) << std::endl;
//   std::cout << "Snippet: " << snippet << std::endl;
// }

// // Função hash para o ID
// int Artigo::hash_id(int id, int table_size) { return id % table_size; }

// // Retorna o tamanho fixo do registro
// size_t Artigo::getRecordSize() {
//   return sizeof(int) +                      // id
//          sizeof(char) * (MAX_TITULO + 1) +  // titulo
//          sizeof(int) +                      // ano
//          sizeof(char) * (MAX_AUTORES + 1) + // autores
//          sizeof(int) +                      // citacoes
//          sizeof(time_t) +                   // atualizacao
//          sizeof(char) * (MAX_SNIPPET + 1);  // snippet
// }

// std::string formatDateTime(time_t timestamp) {
//   char buffer[32];
//   struct tm *tm_info = localtime(&timestamp);
//   strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
//   return std::string(buffer);
// }

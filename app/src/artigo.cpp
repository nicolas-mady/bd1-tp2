#include "artigo.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Implementação do construtor padrão
Artigo::Artigo() : id(0), ano(0), citacoes(0), atualizacao(0)
{
    memset(titulo, 0, sizeof(titulo));
    memset(autores, 0, sizeof(autores));
    memset(snippet, 0, sizeof(snippet));
}

// Implementação do construtor com parâmetros
Artigo::Artigo(int id, const std::string &titulo, int ano,
               const std::string &autores, int citacoes,
               const std::string &data_atualizacao,
               const std::string &snippet)
    : id(id), ano(ano), citacoes(citacoes)
{

    // Copia título (limitando tamanho)
    strncpy(this->titulo, titulo.c_str(), MAX_TITULO);
    this->titulo[MAX_TITULO] = '\0';

    // Copia autores (limitando tamanho)
    strncpy(this->autores, autores.c_str(), MAX_AUTORES);
    this->autores[MAX_AUTORES] = '\0';

    // Copia snippet (limitando tamanho)
    strncpy(this->snippet, snippet.c_str(), MAX_SNIPPET);
    this->snippet[MAX_SNIPPET] = '\0';

    // Converte data/hora
    this->atualizacao = parseDateTime(data_atualizacao);
}

// Serialização do registro
void Artigo::serialize(std::ostream &out) const
{
    out.write(reinterpret_cast<const char *>(&id), sizeof(id));
    out.write(titulo, sizeof(titulo));
    out.write(reinterpret_cast<const char *>(&ano), sizeof(ano));
    out.write(autores, sizeof(autores));
    out.write(reinterpret_cast<const char *>(&citacoes), sizeof(citacoes));
    out.write(reinterpret_cast<const char *>(&atualizacao), sizeof(atualizacao));
    out.write(snippet, sizeof(snippet));
}

// Deserialização do registro
void Artigo::deserialize(std::istream &in)
{
    in.read(reinterpret_cast<char *>(&id), sizeof(id));
    in.read(titulo, sizeof(titulo));
    in.read(reinterpret_cast<char *>(&ano), sizeof(ano));
    in.read(autores, sizeof(autores));
    in.read(reinterpret_cast<char *>(&citacoes), sizeof(citacoes));
    in.read(reinterpret_cast<char *>(&atualizacao), sizeof(atualizacao));
    in.read(snippet, sizeof(snippet));
}

// Imprime o registro formatado
void Artigo::print() const
{
    std::cout << "ID: " << id << std::endl;
    std::cout << "Título: " << titulo << std::endl;
    std::cout << "Ano: " << ano << std::endl;
    std::cout << "Autores: " << autores << std::endl;
    std::cout << "Citações: " << citacoes << std::endl;
    std::cout << "Atualização: " << formatDateTime(atualizacao) << std::endl;
    std::cout << "Snippet: " << snippet << std::endl;
}

// Função hash para o ID
int Artigo::hash_id(int id, int table_size)
{
    return id % table_size;
}

// Retorna o tamanho fixo do registro
size_t Artigo::getRecordSize()
{
    return sizeof(int) +                      // id
           sizeof(char) * (MAX_TITULO + 1) +  // titulo
           sizeof(int) +                      // ano
           sizeof(char) * (MAX_AUTORES + 1) + // autores
           sizeof(int) +                      // citacoes
           sizeof(time_t) +                   // atualizacao
           sizeof(char) * (MAX_SNIPPET + 1);  // snippet
}

// Funções utilitárias

std::string trim(const std::string &str)
{
    std::string trimmed = str;

    // Remove espaços do início
    trimmed.erase(trimmed.begin(),
                  std::find_if(trimmed.begin(), trimmed.end(),
                               [](unsigned char ch)
                               { return !std::isspace(ch); }));

    // Remove espaços do final
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(),
                               [](unsigned char ch)
                               { return !std::isspace(ch); })
                      .base(),
                  trimmed.end());

    return trimmed;
}

time_t parseDateTime(const std::string &datetime_str)
{
    struct tm tm = {};
    std::istringstream ss(datetime_str);

    // Formato esperado: "YYYY-MM-DD HH:MM:SS"
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail())
    {
        // Tenta formato alternativo
        ss.clear();
        ss.str(datetime_str);
        ss >> std::get_time(&tm, "%d/%m/%Y %H:%M:%S");
    }

    return mktime(&tm);
}

std::string formatDateTime(time_t timestamp)
{
    char buffer[32];
    struct tm *tm_info = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return std::string(buffer);
}
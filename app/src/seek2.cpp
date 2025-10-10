#include "artigo.h"
#include "hash_table.h"
#include "simple_btree.h"
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <iomanip>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " \"<Título>\"" << std::endl;
        std::cerr << "Exemplo: " << argv[0] << " \"Design and implementation of an immersive virtual reality system\"" << std::endl;
        return 1;
    }

    std::string titulo = argv[1];

    // Verifica se variáveis de ambiente estão definidas
    std::string data_dir = std::getenv("DATA_DIR") ? std::getenv("DATA_DIR") : "/data/db";
    std::string hash_file = data_dir + "/dados.hash";
    std::string secondary_index_file = data_dir + "/indice_secundario.btree";

    std::cout << "=== SEEK2 - Busca via Índice Secundário ===" << std::endl;
    std::cout << "Procurando título: \"" << titulo << "\"" << std::endl;
    std::cout << "Índice secundário: " << secondary_index_file << std::endl;
    std::cout << "Arquivo de dados: " << hash_file << std::endl;

    // Inicializa índice secundário
    SimpleIndiceSecundario indice_secundario(secondary_index_file);

    // Mede tempo de busca no índice
    auto start_time = std::chrono::high_resolution_clock::now();

    // Busca no índice secundário
    IndiceSecundarioEntry key_entry(titulo, 0);
    IndiceSecundarioEntry result_entry;
    BuscaEstatisticas index_stats = indice_secundario.search(key_entry, result_entry);

    std::cout << std::endl
              << "=== BUSCA NO ÍNDICE SECUNDÁRIO ===" << std::endl;
    std::cout << "Blocos lidos no índice: " << index_stats.blocos_lidos << std::endl;
    std::cout << "Total de blocos do índice: " << index_stats.total_blocos << std::endl;

    if (!index_stats.encontrado)
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        std::cout << "Título não encontrado no índice secundário" << std::endl;
        std::cout << "Tempo total de busca: " << duration.count() << " µs" << std::endl;
        return 1;
    }

    std::cout << "Título encontrado no índice - ID do artigo: " << result_entry.id << std::endl;

    // Agora busca o registro completo no arquivo de dados usando o ID encontrado
    int table_size = 100000; // Valor padrão
    HashTable hash_table(hash_file, table_size);

    Artigo artigo;
    BuscaEstatisticas data_stats = hash_table.search(result_entry.id, artigo);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << std::endl
              << "=== BUSCA NO ARQUIVO DE DADOS ===" << std::endl;
    std::cout << "Blocos lidos no arquivo de dados: " << data_stats.blocos_lidos << std::endl;

    std::cout << std::endl
              << "=== RESULTADOS DA BUSCA ===" << std::endl;

    if (data_stats.encontrado)
    {
        std::cout << "REGISTRO ENCONTRADO:" << std::endl;
        std::cout << "-------------------" << std::endl;
        artigo.print();

        // Verifica se o título realmente corresponde
        std::string titulo_encontrado = artigo.titulo;
        if (titulo_encontrado != titulo)
        {
            std::cout << std::endl
                      << "AVISO: Título no registro difere do buscado!" << std::endl;
            std::cout << "Buscado: \"" << titulo << "\"" << std::endl;
            std::cout << "Encontrado: \"" << titulo_encontrado << "\"" << std::endl;
        }
    }
    else
    {
        std::cout << "REGISTRO NÃO ENCONTRADO NO ARQUIVO DE DADOS" << std::endl;
        std::cout << "INCONSISTÊNCIA: ID " << result_entry.id
                  << " existe no índice mas não no arquivo de dados!" << std::endl;
    }

    std::cout << std::endl
              << "=== ESTATÍSTICAS TOTAIS ===" << std::endl;
    std::cout << "Total de blocos lidos (índice + dados): "
              << (index_stats.blocos_lidos + data_stats.blocos_lidos) << std::endl;
    std::cout << "Blocos do índice secundário: " << index_stats.total_blocos << std::endl;
    std::cout << "Tempo total de busca: " << duration.count() << " µs" << std::endl;

    // Comparação de eficiência
    if (index_stats.total_blocos > 0 && data_stats.total_blocos > 0)
    {
        double index_percent = (double)index_stats.blocos_lidos / index_stats.total_blocos * 100.0;
        double total_blocks = index_stats.total_blocos + data_stats.total_blocos;
        double total_read = index_stats.blocos_lidos + data_stats.blocos_lidos;
        double overall_percent = total_read / total_blocks * 100.0;

        std::cout << "Porcentagem do índice acessada: " << std::fixed
                  << std::setprecision(2) << index_percent << "%" << std::endl;
        std::cout << "Porcentagem total dos arquivos acessada: " << std::fixed
                  << std::setprecision(2) << overall_percent << "%" << std::endl;
    }

    std::cout << std::endl
              << "=== ANÁLISE DE BUSCA POR TÍTULO ===" << std::endl;
    std::cout << "Busca por título é indireta:" << std::endl;
    std::cout << "1. Busca no índice secundário (Título -> ID)" << std::endl;
    std::cout << "2. Busca no arquivo de dados (ID -> Registro completo)" << std::endl;
    std::cout << "Total de acessos: " << (index_stats.blocos_lidos + data_stats.blocos_lidos) << " blocos" << std::endl;

    return data_stats.encontrado ? 0 : 1;
}
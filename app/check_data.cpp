#include "b+tree.h"
#include <iostream>

int main()
{
    std::cout << "=== Verificação de Dados no Índice Secundário ===" << std::endl;

    BPlusTree<std::pair<std::string, int>> bptree(6);

    if (!bptree.loadFromFile("data/db/idx2.bin"))
    {
        std::cerr << "Erro ao carregar índice" << std::endl;
        return 1;
    }

    std::cout << "Árvore carregada com " << bptree.getTotalNodesCount() << " nós" << std::endl;

    // Buscar qualquer dado nas primeiras folhas
    std::cout << "Buscando primeiras entradas..." << std::endl;

    auto results = bptree.searchBySubstring(""); // Buscar qualquer coisa

    std::cout << "Encontradas " << results.size() << " entradas nas primeiras folhas" << std::endl;

    if (!results.empty())
    {
        std::cout << "Primeiras 10 entradas:" << std::endl;
        for (size_t i = 0; i < std::min(results.size(), size_t(10)); i++)
        {
            std::cout << "[" << (i + 1) << "] \"" << results[i].first << "\" -> " << results[i].second << std::endl;
        }

        // Testar busca por fragmentos comuns
        std::vector<std::string> common_words = {"the", "a", "an", "of", "in", "for", "and", "to", "with"};

        std::cout << "\nTestando palavras comuns:" << std::endl;
        for (const auto &word : common_words)
        {
            auto word_results = bptree.searchBySubstring(word);
            if (!word_results.empty())
            {
                std::cout << "'" << word << "': " << word_results.size() << " resultados" << std::endl;
                break; // Parar no primeiro encontrado
            }
        }
    }

    return 0;
}
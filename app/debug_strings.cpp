#include "b+tree.h"
#include <iostream>

int main()
{
    std::cout << "=== Debug Específico das Strings ===" << std::endl;

    BPlusTree<std::pair<std::string, int>> bptree(6);

    if (!bptree.loadFromFile("data/db/idx2.bin"))
    {
        std::cerr << "Erro ao carregar índice" << std::endl;
        return 1;
    }

    std::cout << "Árvore carregada" << std::endl;

    // Buscar qualquer string não vazia
    auto results = bptree.searchBySubstring("");

    std::cout << "Total de resultados: " << results.size() << std::endl;

    if (!results.empty())
    {
        std::cout << "Primeiros 10 resultados (debug strings):" << std::endl;
        for (size_t i = 0; i < std::min(results.size(), size_t(10)); i++)
        {
            std::cout << "[" << (i + 1) << "] length=" << results[i].first.length()
                      << " content=\"" << results[i].first << "\" -> " << results[i].second << std::endl;

            // Debug byte por byte se for string vazia
            if (results[i].first.empty())
            {
                std::cout << "    String vazia detectada!" << std::endl;
            }
            else if (results[i].first.length() < 50)
            {
                std::cout << "    Primeiros chars: ";
                for (char c : results[i].first)
                {
                    if (isprint(c))
                        std::cout << c;
                    else
                        std::cout << "[" << (int)c << "]";
                }
                std::cout << std::endl;
            }
        }
    }

    return 0;
}
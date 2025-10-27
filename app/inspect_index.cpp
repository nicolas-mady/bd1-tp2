#include "b+tree.h"
#include <iostream>

int main()
{
    std::cout << "=== Inspeção do Índice Secundário ===" << std::endl;

    BPlusTree<std::pair<std::string, int>> bptree(6);

    if (!bptree.loadFromFile("data/db/idx2.bin"))
    {
        std::cerr << "Erro ao carregar índice" << std::endl;
        return 1;
    }

    std::cout << "Árvore carregada com " << bptree.getTotalNodesCount() << " nós" << std::endl;

    // Buscar qualquer par que comece com 'A'
    for (char c = 'A'; c <= 'Z'; c++)
    {
        std::string prefix(1, c);
        auto result = bptree.search({prefix, 0});
        if (result != nullptr)
        {
            std::cout << "Encontrado nó para prefixo '" << c << "'" << std::endl;
            std::cout << "Primeiras entradas do nó:" << std::endl;
            for (size_t i = 0; i < std::min(size_t(5), result->keys.size()); i++)
            {
                std::cout << "  \"" << result->keys[i].first << "\" -> " << result->keys[i].second << std::endl;
            }
            break;
        }
    }

    // Tentar com minúsculas
    for (char c = 'a'; c <= 'z'; c++)
    {
        std::string prefix(1, c);
        auto result = bptree.search({prefix, 0});
        if (result != nullptr)
        {
            std::cout << "Encontrado nó para prefixo '" << c << "'" << std::endl;
            std::cout << "Primeiras entradas do nó:" << std::endl;
            for (size_t i = 0; i < std::min(size_t(5), result->keys.size()); i++)
            {
                std::cout << "  \"" << result->keys[i].first << "\" -> " << result->keys[i].second << std::endl;
            }
            break;
        }
    }

    return 0;
}
#include "b+tree.h"
#include <iostream>

int main()
{
    std::cout << "=== Teste com Índice Secundário - Carregamento Completo ===" << std::endl;

    // Criar uma árvore simples para testar
    BPlusTree<std::pair<std::string, int>> test_tree(6);
    test_tree.insert({"apple", 1});
    test_tree.insert({"banana", 2});
    test_tree.insert({"cherry", 3});

    // Salvar e recarregar para testar
    if (test_tree.saveToFile("test_secondary.bin"))
    {
        std::cout << "Árvore de teste salva" << std::endl;

        BPlusTree<std::pair<std::string, int>> loaded_tree(6);
        if (loaded_tree.loadFromFile("test_secondary.bin"))
        {
            std::cout << "Árvore de teste carregada com lazy loading" << std::endl;
            std::cout << "Total de nós: " << loaded_tree.getTotalNodesCount() << std::endl;

            // Testar busca
            auto result = loaded_tree.search({"apple", 1});
            if (result)
            {
                std::cout << "Encontrou 'apple'!" << std::endl;
                std::cout << "Chaves no nó:" << std::endl;
                for (const auto &pair : result->keys)
                {
                    std::cout << "  \"" << pair.first << "\" -> " << pair.second << std::endl;
                }
            }
            else
            {
                std::cout << "Não encontrou 'apple'" << std::endl;
            }

            // Testar busca parcial
            result = loaded_tree.search({"banana", 0});
            if (result)
            {
                std::cout << "Encontrou nó com 'banana'!" << std::endl;
            }
        }
    }

    return 0;
}
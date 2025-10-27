#include "b+tree.h"
#include <iostream>

// Programa para testar carregamento completo (sem lazy loading)
int main()
{
    std::cout << "=== Teste de Carregamento Completo (Debug) ===" << std::endl;

    // Criar uma árvore de teste simples
    BPlusTree<std::pair<std::string, int>> test_tree(6);

    // Inserir alguns dados de teste similares aos reais
    test_tree.insert({"A study of machine learning", 1});
    test_tree.insert({"Design and implementation", 2});
    test_tree.insert({"Poster: 3D sketching and flexible input", 3});
    test_tree.insert({"The analysis of algorithms", 4});
    test_tree.insert({"An approach to data mining", 5});

    std::cout << "Árvore de teste criada com 5 entradas" << std::endl;

    // Salvar
    test_tree.saveToFile("test_debug.bin");
    std::cout << "Árvore salva em test_debug.bin" << std::endl;

    // Carregar com lazy loading
    BPlusTree<std::pair<std::string, int>> loaded_tree(6);
    if (loaded_tree.loadFromFile("test_debug.bin"))
    {
        std::cout << "Árvore carregada com lazy loading" << std::endl;
        std::cout << "Total de nós: " << loaded_tree.getTotalNodesCount() << std::endl;

        // Testar busca por substring
        auto results = loaded_tree.searchBySubstring("3D");
        std::cout << "Busca por '3D': " << results.size() << " resultados" << std::endl;

        results = loaded_tree.searchBySubstring("Design");
        std::cout << "Busca por 'Design': " << results.size() << " resultados" << std::endl;

        results = loaded_tree.searchByPrefix("Poster");
        std::cout << "Busca por prefixo 'Poster': " << results.size() << " resultados" << std::endl;

        // Mostrar todos os resultados
        results = loaded_tree.searchBySubstring("");
        std::cout << "Busca por substring vazia: " << results.size() << " resultados" << std::endl;

        for (const auto &pair : results)
        {
            std::cout << "  \"" << pair.first << "\" -> " << pair.second << std::endl;
        }
    }

    return 0;
}
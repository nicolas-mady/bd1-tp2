#include "b+tree.h"
#include <iostream>
#include <chrono>

void testPerformance(const std::string &name, const std::string &filename, int m)
{
    std::cout << "\n=== " << name << " ===" << std::endl;

    // Teste com árvore de inteiros
    if (name.find("Primary") != std::string::npos)
    {
        BPlusTree<int> btree(m);

        auto start = std::chrono::high_resolution_clock::now();
        bool loaded = btree.loadFromFile(filename);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        if (loaded)
        {
            std::cout << "✓ Carregamento inicial: " << duration.count() << " µs" << std::endl;
            std::cout << "✓ Nós totais: " << btree.getTotalNodesCount() << std::endl;
            std::cout << "✓ Nós carregados inicialmente: " << btree.getLoadedNodesCount() << std::endl;

            // Teste de busca
            std::vector<int> test_ids = {1, 1000, 100000, 1000000};
            for (int id : test_ids)
            {
                auto search_start = std::chrono::high_resolution_clock::now();
                auto result = btree.search(id);
                auto search_end = std::chrono::high_resolution_clock::now();
                auto search_time = std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start);

                std::cout << "  ID " << id << ": "
                          << (result ? "FOUND" : "NOT FOUND")
                          << " (time: " << search_time.count() << " µs, "
                          << "nodes loaded: " << btree.getLoadedNodesCount() << ")" << std::endl;
            }
        }
        else
        {
            std::cout << "✗ Falha no carregamento" << std::endl;
        }
    }
    // Teste com árvore de pares
    else
    {
        BPlusTree<std::pair<std::string, int>> btree(m);

        auto start = std::chrono::high_resolution_clock::now();
        bool loaded = btree.loadFromFile(filename);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        if (loaded)
        {
            std::cout << "✓ Carregamento inicial: " << duration.count() << " µs" << std::endl;
            std::cout << "✓ Nós totais: " << btree.getTotalNodesCount() << std::endl;
            std::cout << "✓ Nós carregados inicialmente: " << btree.getLoadedNodesCount() << std::endl;

            // Teste de busca
            std::vector<std::string> test_titles = {"A", "The", "An", "Design"};
            for (const std::string &title : test_titles)
            {
                auto search_start = std::chrono::high_resolution_clock::now();
                auto result = btree.search({title, 0});
                auto search_end = std::chrono::high_resolution_clock::now();
                auto search_time = std::chrono::duration_cast<std::chrono::microseconds>(search_end - search_start);

                std::cout << "  Title \"" << title << "\": "
                          << (result ? "FOUND" : "NOT FOUND")
                          << " (time: " << search_time.count() << " µs, "
                          << "nodes loaded: " << btree.getLoadedNodesCount() << ")" << std::endl;
            }
        }
        else
        {
            std::cout << "✗ Falha no carregamento" << std::endl;
        }
    }
}

int main()
{
    std::cout << "=== TESTE DE PERFORMANCE - LAZY LOADING B+ TREES ===" << std::endl;

    testPerformance("Primary Index", "data/db/idx1.bin", 170);
    testPerformance("Secondary Index", "data/db/idx2.bin", 6);

    return 0;
}
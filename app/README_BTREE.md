# Árvores B+ com Persistência em Disco

Esta implementação estende a classe `BPlusTree` para suportar salvamento e carregamento de árvores B+ no disco, tanto para tipos `int` quanto para `std::pair<std::string, int>`.

## Funcionalidades Implementadas

### 1. Salvamento no Disco
```cpp
bool saveToFile(const std::string& filename)
```
- Salva a árvore B+ completa em um arquivo binário
- Preserva a estrutura da árvore, incluindo ponteiros next entre folhas
- Funciona com qualquer tipo suportado (int, std::pair<std::string, int>)

### 2. Carregamento do Disco
```cpp
bool loadFromFile(const std::string& filename)
```
- Carrega uma árvore B+ previamente salva
- Reconstrói completamente a estrutura da árvore
- Restaura todos os ponteiros e conexões

## Tipos Suportados

### 1. BPlusTree<int>
- Árvore para índices primários (IDs)
- Otimizada para busca rápida por números inteiros

### 2. BPlusTree<std::pair<std::string, int>>
- Árvore para índices secundários (título, ID)
- Suporta otimização de prefixo para strings
- Comparação automática: primeiro por string, depois por int

## Exemplo de Uso

### Criar e Salvar
```cpp
// Criar árvore de inteiros
BPlusTree<int> arvore_ids(170);
arvore_ids.insert(123456);
arvore_ids.insert(789012);

// Salvar no disco
arvore_ids.saveToFile("data/db/idx1.bin");

// Criar árvore de pares
BPlusTree<std::pair<std::string, int>> arvore_titulos(6);
arvore_titulos.insert({"Machine Learning", 123456});
arvore_titulos.insert({"Deep Learning", 789012});

// Salvar no disco
arvore_titulos.saveToFile("data/db/idx2.bin");
```

### Carregar e Usar
```cpp
// Carregar árvore de inteiros
BPlusTree<int> arvore_ids(170);
if (arvore_ids.loadFromFile("data/db/idx1.bin")) {
    // Buscar por ID
    if (arvore_ids.search(123456) != nullptr) {
        std::cout << "ID encontrado!" << std::endl;
    }
}

// Carregar árvore de pares
BPlusTree<std::pair<std::string, int>> arvore_titulos(6);
if (arvore_titulos.loadFromFile("data/db/idx2.bin")) {
    // Buscar por título
    auto resultado = arvore_titulos.search({"Machine Learning", 123456});
    if (resultado != nullptr) {
        std::cout << "Título encontrado!" << std::endl;
    }
}
```

## Programas de Teste

### simple_test
Teste básico das funcionalidades de save/load:
```bash
./simple_test
```

### btree_test
Teste com dados reais carregados:
```bash
./btree_test load_test
```

## Integração com upload.cpp

O programa `upload.cpp` foi modificado para:
1. Inserir dados em ambas as árvores (IDs e títulos)
2. Salvar automaticamente as árvores em:
   - `data/db/idx1.bin` - Índice primário (IDs)
   - `data/db/idx2.bin` - Índice secundário (títulos)

## Formato do Arquivo Binário

O arquivo salvo contém:
1. Metadados da árvore (valor de m)
2. Número total de nós
3. Dados de cada nó:
   - ID do nó
   - Flag isLeaf
   - Chaves do nó
   - IDs dos filhos (para nós internos)
   - ID do próximo nó (para folhas)
4. ID do nó raiz

## Considerações de Performance

- O salvamento percorre toda a árvore uma vez
- O carregamento reconstrói a árvore em duas passadas
- Arquivos são compactos (formato binário)
- Tempo de carregamento é proporcional ao tamanho da árvore

## Limitações Atuais

- Não há limpeza automática da árvore existente no loadFromFile
- Sem verificação de integridade do arquivo
- Sem compressão dos dados salvos
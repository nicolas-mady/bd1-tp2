#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "record.h"
#include <bits/stdc++.h>

// #include <fstream>
// #include <queue>
// #include <unordered_map>
// #include <unordered_set>

template <typename T>
class BPlusTree {
private:

  // Helper function to print keys
  template <typename U = T>
  typename std::enable_if<!std::is_same<U, std::pair<std::string, int>>::value>::type
  print_key(const T& key) const {
    std::cout << key << std::endl;
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::pair<std::string, int>>::value>::type
  print_key(const std::pair<std::string, int>& key) const {
    std::cout << "(" << key.second << ", " << key.first << ")" << std::endl;
  }

  struct Node {
    bool isLeaf;
    std::vector<T> keys;
    std::vector<Node*> children;
    Node* next;

    // Para lazy loading
    bool isLoaded;
    int nodeId;
    std::streampos filePosition;
  };

  Node* root;
  int m;

  // Para lazy loading
  mutable std::ifstream lazyFile;
  std::string fileName;
  mutable std::unordered_map<int, Node*> nodeCache;
  std::unordered_map<int, std::streampos> nodePositions;
  bool isLazyMode;

  // Helper functions for serialization
  void saveNodeToFile(std::ofstream& file, Node* node, std::unordered_map<Node*, int>& nodeIds, int& nextId) {
    if (!node)
      return;

    // Assign ID to this node if not already assigned
    if (nodeIds.find(node) == nodeIds.end())
      nodeIds[node] = nextId++;

    int nodeId = nodeIds[node];
    file.write(reinterpret_cast<const char*>(&nodeId), sizeof(int));
    file.write(reinterpret_cast<const char*>(&node->isLeaf), sizeof(bool));

    // Save keys
    size_t keyCount = node->keys.size();
    file.write(reinterpret_cast<const char*>(&keyCount), sizeof(size_t));
    for (const auto& key : node->keys)
      saveKey(file, key);

    // Save children for internal nodes
    if (!node->isLeaf) {
      size_t childCount = node->children.size();
      file.write(reinterpret_cast<const char*>(&childCount), sizeof(size_t));
      for (auto child : node->children) {
        if (nodeIds.find(child) == nodeIds.end())
          nodeIds[child] = nextId++;
        int childId = nodeIds[child];
        file.write(reinterpret_cast<const char*>(&childId), sizeof(int));
      }
    }

    // Save next pointer for leaf nodes
    if (node->isLeaf) {
      int nextId = -1;
      if (node->next) {
        if (nodeIds.find(node->next) == nodeIds.end())
          nodeIds[node->next] = nextId++;
        nextId = nodeIds[node->next];
      }
      file.write(reinterpret_cast<const char*>(&nextId), sizeof(int));
    }
  }

  // Template specializations for saving different key types
  template <typename U = T>
  typename std::enable_if<std::is_same<U, int>::value>::type
  saveKey(std::ofstream& file, const int& key) {
    file.write(reinterpret_cast<const char*>(&key), sizeof(int));
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::pair<std::string, int>>::value>::type
  saveKey(std::ofstream& file, const std::pair<std::string, int>& key) {
    size_t strLen = key.first.length();
    file.write(reinterpret_cast<const char*>(&strLen), sizeof(size_t));
    file.write(key.first.c_str(), strLen);
    file.write(reinterpret_cast<const char*>(&key.second), sizeof(int));
  }

  // Template specializations for loading different key types
  template <typename U = T>
  typename std::enable_if<std::is_same<U, int>::value, int>::type
  loadKey(std::ifstream& file) const {
    int key;
    file.read(reinterpret_cast<char*>(&key), sizeof(int));
    return key;
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::pair<std::string, int>>::value, std::pair<std::string, int>>::type
  loadKey(std::ifstream& file) const {
    size_t strLen;
    file.read(reinterpret_cast<char*>(&strLen), sizeof(size_t));
    std::string str(strLen, '\0');
    file.read(&str[0], strLen);
    int value;
    file.read(reinterpret_cast<char*>(&value), sizeof(int));
    return std::make_pair(str, value);
  }

  // Método para carregar um nó específico do disco
  Node* loadNodeById(int nodeId) const {
    // Verificar se já está no cache
    auto it = nodeCache.find(nodeId);
    if (it != nodeCache.end())
      return it->second;

    // Verificar se temos a posição do nó
    auto posIt = nodePositions.find(nodeId);
    if (posIt == nodePositions.end())
      return nullptr;

    // Abrir arquivo se necessário
    if (!lazyFile.is_open()) {
      lazyFile.open(fileName, std::ios::binary);
      if (!lazyFile)
        return nullptr;
    }

    // Ir para a posição do nó
    lazyFile.seekg(posIt->second);

    // Ler dados do nó
    Node* node = new Node();
    node->nodeId = nodeId;
    node->isLoaded = true;

    int readNodeId;
    lazyFile.read(reinterpret_cast<char*>(&readNodeId), sizeof(int));
    lazyFile.read(reinterpret_cast<char*>(&node->isLeaf), sizeof(bool));

    // Carregar chaves
    size_t keyCount;
    lazyFile.read(reinterpret_cast<char*>(&keyCount), sizeof(size_t));
    node->keys.reserve(keyCount);
    for (size_t i = 0; i < keyCount; i++)
      node->keys.push_back(loadKey(lazyFile));

    // Carregar IDs dos filhos (para nós internos)
    if (!node->isLeaf) {
      size_t childCount;
      lazyFile.read(reinterpret_cast<char*>(&childCount), sizeof(size_t));
      node->children.resize(childCount);
      for (size_t i = 0; i < childCount; i++) {
        int childId;
        lazyFile.read(reinterpret_cast<char*>(&childId), sizeof(int));
        // Criar placeholder para o filho (será carregado sob demanda)
        Node* childPlaceholder = new Node();
        childPlaceholder->nodeId = childId;
        childPlaceholder->isLoaded = false;
        node->children[i] = childPlaceholder;
      }
    }

    // Carregar ID do próximo nó (para folhas)
    if (node->isLeaf) {
      int nextId;
      lazyFile.read(reinterpret_cast<char*>(&nextId), sizeof(int));
      if (nextId != -1) {
        Node* nextPlaceholder = new Node();
        nextPlaceholder->nodeId = nextId;
        nextPlaceholder->isLoaded = false;
        node->next = nextPlaceholder;
      } else {
        node->next = nullptr;
      }
    }

    // Adicionar ao cache
    nodeCache[nodeId] = node;
    return node;
  }

  // Método para garantir que um nó está carregado
  Node* ensureLoaded(Node* node) const {
    if (!node)
      return nullptr;

    if (node->isLoaded || !isLazyMode)
      return node;

    Node* loadedNode = loadNodeById(node->nodeId);
    if (loadedNode && loadedNode != node) {
      // Copiar dados do nó carregado para o placeholder
      node->isLeaf = loadedNode->isLeaf;
      node->keys = loadedNode->keys;
      node->children = loadedNode->children;
      node->next = loadedNode->next;
      node->isLoaded = true;
      node->filePosition = loadedNode->filePosition;

      // Atualizar cache para apontar para o nó original
      nodeCache[node->nodeId] = node;

      // Não deletar loadedNode se for diferente do cache
      if (nodeCache.find(node->nodeId) != nodeCache.end() &&
          nodeCache[node->nodeId] != loadedNode)
        delete loadedNode;
    }
    return node->isLoaded ? node : nullptr;
  }

  std::vector<Node*> findLeaf(const T& key) const {
    std::vector<Node*> path;
    auto node = root;

    if (!node)
      return path; // Verificação de segurança

    while (node && !node->isLeaf) {
      // Garantir que o nó está carregado
      node = ensureLoaded(node);
      if (!node)
        break;

      path.push_back(node);
      auto it = std::upper_bound(node->keys.begin(), node->keys.end(), key);

      if (it == node->keys.end()) {
        if (node->children.empty())
          break;
        node = node->children.back();
      } else {
        int i = std::distance(node->keys.begin(), it);
        if (i >= (int) node->children.size())
          break;
        node = node->children[i];
      }
    }

    // Garantir que a folha está carregada
    if (node) {
      node = ensureLoaded(node);
      if (node)
        path.push_back(node);
    }
    return path;
  }

  void insertInternal(Node* parent, Node* child, const T& key) {
    auto it = std::upper_bound(parent->keys.begin(), parent->keys.end(), key);
    int i = std::distance(parent->keys.begin(), it);
    parent->keys.insert(it, key);
    parent->children.insert(parent->children.begin() + i + 1, child);

    if (parent->keys.size() > 2 * m) {
      Node* newParent = new Node();
      newParent->isLeaf = false;

      T midKey = parent->keys[m];
      parent->keys.erase(parent->keys.begin() + m);

      newParent->keys.assign(parent->keys.begin() + m, parent->keys.end());
      parent->keys.erase(parent->keys.begin() + m, parent->keys.end());

      newParent->children.assign(parent->children.begin() + m + 1, parent->children.end());
      parent->children.erase(parent->children.begin() + m + 1, parent->children.end());

      Node* grandParent = findParent(root, parent);
      if (grandParent == nullptr) {
        Node* newRoot = new Node();
        newRoot->isLeaf = false;
        newRoot->keys.push_back(midKey);
        newRoot->children.push_back(parent);
        newRoot->children.push_back(newParent);
        root = newRoot;
      } else {
        insertInternal(grandParent, newParent, midKey);
      }
    }
  }

  Node* findParent(Node* node, Node* child) {
    if (node == nullptr || node->isLeaf)
      return nullptr;

    for (auto c : node->children)
      if (c == child)
        return node;

    for (auto c : node->children) {
      Node* parent = findParent(c, child);
      if (parent != nullptr)
        return parent;
    }
    return nullptr;
  }

  // Generic separator function (for int, etc.)
  template <typename U = T>
  typename std::enable_if<!std::is_same<U, std::pair<std::string, int>>::value, T>::type
  get_separator(const T& key, const T& last_key_in_prev_node) {
    return key;
  }

  // Specialized separator function for std::string (prefix optimization)
  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::string>::value, std::string>::type
  get_separator(const std::string& key, const std::string& last_key_in_prev_node) {
    size_t len = 0;
    while (len < key.length() && len < last_key_in_prev_node.length() && key[len] == last_key_in_prev_node[len])
      len++;
    if (len < key.length())
      return key.substr(0, len + 1);
    return key;
  }

  // Specialized separator function for std::pair<std::string, int> (prefix optimization on string part)
  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::pair<std::string, int>>::value, std::pair<std::string, int>>::type
  get_separator(const std::pair<std::string, int>& key, const std::pair<std::string, int>& last_key_in_prev_node) {
    const std::string& key_str = key.first;
    const std::string& last_key_str = last_key_in_prev_node.first;

    size_t len = 0;
    while (len < key_str.length() && len < last_key_str.length() && key_str[len] == last_key_str[len])
      len++;

    if (len < key_str.length())
      return std::make_pair(key_str.substr(0, len + 1), key.second);
    return key;
  }

public:

  BPlusTree(int m) : m(m), isLazyMode(false) {
    root = new Node();
    root->isLeaf = true;
    root->next = nullptr;
    root->isLoaded = true;
    root->nodeId = -1;
  }

  void insert(const T& key) {
    auto path = findLeaf(key);
    auto leaf = path.back();

    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    leaf->keys.insert(it, key);

    if (leaf->keys.size() > 2 * m) {
      Node* newLeaf = new Node();
      newLeaf->isLeaf = true;

      T last_key_in_old_leaf = leaf->keys.back();

      newLeaf->keys.assign(leaf->keys.begin() + m, leaf->keys.end());
      leaf->keys.erase(leaf->keys.begin() + m, leaf->keys.end());

      newLeaf->next = leaf->next;
      leaf->next = newLeaf;

      T midKey = get_separator(newLeaf->keys[0], last_key_in_old_leaf);
      Node* parent = findParent(root, leaf);

      if (parent == nullptr) {
        Node* newRoot = new Node();
        newRoot->isLeaf = false;
        newRoot->keys.push_back(midKey);
        newRoot->children.push_back(leaf);
        newRoot->children.push_back(newLeaf);
        root = newRoot;
      } else {
        insertInternal(parent, newLeaf, midKey);
      }
    }
  }

  Node* search(const T& key) const {
    auto path = findLeaf(key);
    if (path.empty())
      return nullptr;

    auto leaf = path.back();
    leaf = ensureLoaded(leaf);
    if (!leaf)
      return nullptr;

    return std::binary_search(leaf->keys.begin(), leaf->keys.end(), key) ? leaf : nullptr;
  }

  void traverse() const {
    auto node = root;
    while (!node->isLeaf)
      node = node->children[0];

    while (node != nullptr) {
      for (const auto& key : node->keys)
        print_key(key);
      node = node->next;
    }
    std::cout << std::endl;
  }

  // Save the B+ tree to a binary file
  int saveToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Erro: não foi possível criar o arquivo " << filename << std::endl;
      return -1;
    }

    // Save tree metadata
    file.write(reinterpret_cast<const char*>(&m), sizeof(int));

    // Create node ID mapping
    std::unordered_map<Node*, int> nodeIds;
    int nextId = 0;

    // Count total nodes first
    std::queue<Node*> nodeQueue;
    std::unordered_set<Node*> visited;
    nodeQueue.push(root);
    visited.insert(root);

    while (!nodeQueue.empty()) {
      Node* current = nodeQueue.front();
      nodeQueue.pop();

      if (!current->isLeaf) {
        for (auto child : current->children) {
          if (visited.find(child) == visited.end()) {
            nodeQueue.push(child);
            visited.insert(child);
          }
        }
      }

      if (current->isLeaf && current->next && visited.find(current->next) == visited.end()) {
        nodeQueue.push(current->next);
        visited.insert(current->next);
      }
    }

    int totalNodes = visited.size();
    file.write(reinterpret_cast<const char*>(&totalNodes), sizeof(int));

    // Save all nodes
    for (Node* node : visited)
      saveNodeToFile(file, node, nodeIds, nextId);

    // Save root ID
    int rootId = nodeIds[root];
    file.write(reinterpret_cast<const char*>(&rootId), sizeof(int));

    int numBlocks = (file.tellp() + BLOCK_SIZE - 1) / BLOCK_SIZE;
    file.close();

    return numBlocks;
  }

  // Load the B+ tree from a binary file with lazy loading
  bool loadFromFile(const std::string& filename) {
    fileName = filename;
    isLazyMode = true;
    nodeCache.clear();
    nodePositions.clear();

    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Erro: não foi possível abrir o arquivo " << filename << std::endl;
      return false;
    }

    // Load tree metadata
    file.read(reinterpret_cast<char*>(&m), sizeof(int));

    // Load total nodes
    int totalNodes;
    file.read(reinterpret_cast<char*>(&totalNodes), sizeof(int));

    // Primeira passada: mapear posições dos nós no arquivo
    for (int i = 0; i < totalNodes; i++) {
      std::streampos currentPos = file.tellg();

      int nodeId;
      file.read(reinterpret_cast<char*>(&nodeId), sizeof(int));

      // Guardar posição deste nó
      nodePositions[nodeId] = currentPos;

      bool isLeaf;
      file.read(reinterpret_cast<char*>(&isLeaf), sizeof(bool));

      // Pular chaves
      size_t keyCount;
      file.read(reinterpret_cast<char*>(&keyCount), sizeof(size_t));
      for (size_t j = 0; j < keyCount; j++)
        skipKey(file);

      // Pular children para nós internos
      if (!isLeaf) {
        size_t childCount;
        file.read(reinterpret_cast<char*>(&childCount), sizeof(size_t));
        file.seekg(childCount * sizeof(int), std::ios::cur);
      }

      // Pular next pointer para folhas
      if (isLeaf)
        file.seekg(sizeof(int), std::ios::cur);
    }

    // Carregar apenas o nó raiz inicialmente
    int rootId;
    file.read(reinterpret_cast<char*>(&rootId), sizeof(int));

    root = loadNodeById(rootId);
    if (!root) {
      std::cerr << "Erro: não foi possível carregar nó raiz" << std::endl;
      return false;
    }

    file.close();
    return true;
  }

  // Método auxiliar para pular uma chave no arquivo
  template <typename U = T>
  typename std::enable_if<std::is_same<U, int>::value>::type
  skipKey(std::ifstream& file) const {
    file.seekg(sizeof(int), std::ios::cur);
  }

  template <typename U = T>
  typename std::enable_if<std::is_same<U, std::pair<std::string, int>>::value>::type
  skipKey(std::ifstream& file) const {
    size_t strLen;
    file.read(reinterpret_cast<char*>(&strLen), sizeof(size_t));
    file.seekg(strLen + sizeof(int), std::ios::cur);
  }

  // Estatísticas de carregamento
  int getLoadedNodesCount() const {
    return nodeCache.size();
  }

  int getTotalNodesCount() const {
    return nodePositions.size();
  }

  // Limpar cache (liberar memória)
  void clearCache() {
    if (!isLazyMode)
      return;

    for (auto& pair : nodeCache)
      if (pair.second && pair.second != root)
        delete pair.second;
    nodeCache.clear();

    // Manter apenas a raiz no cache
    if (root)
      nodeCache[root->nodeId] = root;
  }
};

#endif // BPLUSTREE_H

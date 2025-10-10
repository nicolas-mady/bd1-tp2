# TP2 - Bancos de Dados I# TP2 - Banco de Dados I - Sistema de Indexação de Artigos Científicos



**Universidade Federal do Amazonas**  Este projeto implementa um sistema de armazenamento e consulta de artigos científicos utilizando estruturas de arquivo de dados e índices em memória secundária, desenvolvido para a disciplina de Bancos de Dados I da UFAM.

**Instituto de Computação**  

**Disciplina: Bancos de Dados I - 2025/02**  ## 📋 Descrição

**Professor: Altigran Soares da Silva**

O sistema implementa:

## Visão Geral- **Arquivo de dados**: Organizado por hashing para armazenamento dos registros de artigos

- **Índice primário**: B+Tree em memória secundária indexando por ID

Este projeto implementa um sistema de armazenamento e consulta de dados sobre artigos científicos utilizando:- **Índice secundário**: B+Tree em memória secundária indexando por título



- **Arquivo de dados**: Organizado por **hashing** com resolução de colisões por linear probing### Estrutura do Registro de Artigo

- **Índice primário**: **B+Tree** armazenada em memória secundária para busca por ID

- **Índice secundário**: **B+Tree** armazenada em memória secundária para busca por título| Campo | Tipo | Descrição |

|-------|------|-----------|

## Estrutura do Projeto| ID | inteiro | Código identificador do artigo |

| Título | alfa 300 | Título do artigo |

```| Ano | inteiro | Ano de publicação |

/app| Autores | alfa 150 | Lista dos autores |

├── src/                     # Código fonte C++| Citações | inteiro | Número de citações |

│   ├── artigo.cpp          # Estrutura de dados do artigo| Atualização | data e hora | Data da última atualização |

│   ├── hash_table.cpp      # Implementação da tabela hash| Snippet | alfa 100-1024 | Resumo textual |

│   ├── upload.cpp          # Programa de carga de dados

│   ├── findrec.cpp         # Busca direta no arquivo hash## 🚀 Programas Implementados

│   ├── seek1.cpp           # Busca via índice primário

│   └── seek2.cpp           # Busca via índice secundário1. **`upload <file>`**: Carregamento inicial dos dados criando o banco de dados

├── include/                 # Headers2. **`findrec <ID>`**: Busca direta no arquivo de dados por ID

│   ├── artigo.h            # Definições da estrutura de artigo3. **`seek1 <ID>`**: Busca por ID através do índice primário

│   ├── hash_table.h        # Interface da tabela hash4. **`seek2 <Titulo>`**: Busca por título através do índice secundário

│   └── simple_btree.h      # Implementação simplificada de B+Tree

├── bin/                     # Binários compilados## 🏗️ Estrutura do Projeto

├── data/                    # Dados de entrada e arquivos gerados

│   ├── input.csv           # Arquivo CSV de entrada```

│   ├── sample.csv          # Dados de exemplo/app

│   └── db/                 # Arquivos de dados e índices gerados├── src/                     # Código fonte C++

├── Dockerfile              # Construção da imagem Docker│   ├── upload.cpp          # Programa de carregamento

├── docker-compose.yml      # Orquestração de serviços│   ├── findrec.cpp         # Busca direta

├── Makefile               # Automação de build│   ├── seek1.cpp           # Busca por índice primário

└── README.md              # Esta documentação│   ├── seek2.cpp           # Busca por índice secundário

```│   ├── artigo.cpp          # Estrutura do registro

│   ├── hash_table.cpp      # Implementação da tabela hash

## Estrutura dos Dados│   └── btree.cpp           # Implementação da B+Tree

├── include/                 # Headers

### Registro de Artigo│   ├── artigo.h            # Definições do registro

- **ID** (int): Identificador único│   ├── hash_table.h        # Interface da tabela hash

- **Título** (char[301]): Título do artigo│   └── btree.h             # Interface da B+Tree

- **Ano** (int): Ano de publicação├── bin/                    # Binários gerados

- **Autores** (char[151]): Lista de autores├── data/                   # Dados de entrada e persistência

- **Citações** (int): Número de citações│   ├── input.csv           # Arquivo CSV de entrada

- **Atualização** (time_t): Data/hora da última atualização│   ├── sample.csv          # Dados de exemplo

- **Snippet** (char[1025]): Resumo do artigo│   ├── artigo.csv.gz       # Dataset completo comprimido

│   └── db/                 # Arquivos de dados e índices (criado em runtime)

### Arquivos Gerados├── tests/                  # Testes (opcional)

- **dados.hash**: Arquivo principal com registros organizados por hash├── Dockerfile              # Construção da imagem Docker

- **indice_primario.btree**: Índice B+Tree para busca por ID├── docker-compose.yml      # Orquestração dos serviços

- **indice_secundario.btree**: Índice B+Tree para busca por título├── Makefile               # Build system

├── README.md              # Este arquivo

## Programas Implementados└── TP2_documentação.pdf   # Documentação técnica

```

### 1. upload `<arquivo_csv>`

Carrega dados do arquivo CSV e cria:## 🔧 Compilação e Execução

- Arquivo de dados organizado por hashing

- Índice primário (ID → posição)### Build Local

- Índice secundário (Título → ID)

```bash

**Exemplo:**# Compilar todos os programas

```bashmake build

./bin/upload /data/input.csv

```# Limpar arquivos gerados

make clean

### 2. findrec `<ID>`

Busca direta no arquivo hash por ID do artigo.# Extrair dados comprimidos (se necessário)

Mostra estatísticas de blocos acessados.make extract-data

```

**Exemplo:**

```bash### Build com Docker

./bin/findrec 123

``````bash

# Construir imagem Docker

### 3. seek1 `<ID>`make docker-build

Busca por ID usando o índice primário B+Tree.# ou

Compara eficiência com busca direta.docker build -t tp2-bd1 .



**Exemplo:**# Build com docker-compose

```bashdocker-compose build

./bin/seek1 123```

```

## 🐳 Execução com Docker

### 4. seek2 `<Título>`

Busca por título usando o índice secundário B+Tree.### Execução Individual dos Programas

Realiza busca indireta (Título → ID → Registro).

```bash

**Exemplo:**# 1. Upload dos dados

```bashdocker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/upload /data/input.csv

./bin/seek2 "Design and implementation of virtual reality"

```# 2. Busca direta por ID

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/findrec 123

## Build Local

# 3. Busca por índice primário

### Pré-requisitosdocker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek1 123

- g++ com suporte a C++17

- make# 4. Busca por índice secundário (título completo entre aspas)

- Docker (opcional)docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek2 "Design and implementation of an immersive virtual reality system based on a smartphone platform."

```

### Compilação

```bash### Execução com Docker Compose

# Compilar todos os programas

make build```bash

# Upload dos dados

# Limpar arquivos objetodocker-compose --profile upload up upload

make clean

# Testes de busca

# Ver ajudadocker-compose --profile findrec up findrec

make helpdocker-compose --profile seek1 up seek1

```docker-compose --profile seek2 up seek2



### Execução Local# Modo interativo para testes manuais

```bashdocker-compose --profile interactive up interactive

# Preparar dados (extrair CSV comprimido se necessário)```

make extract-data

### Usando Makefile com Docker

# Executar upload

./bin/upload data/sample.csv```bash

# Upload

# Executar buscasmake docker-run-upload

./bin/findrec 1

./bin/seek1 5# Buscas (necessário ajustar parâmetros no Makefile)

./bin/seek2 "Título do artigo"make docker-run-findrec

```make docker-run-seek1

make docker-run-seek2

## Build e Execução com Docker```



### Construir Imagem## 📁 Arquivos Gerados

```bash

# Build da imagemO sistema cria os seguintes arquivos no diretório `/data/db`:

make docker-build

# ou- **`dados.hash`**: Arquivo de dados organizado por hashing

docker build -t tp2-bd1 .- **`indice_primario.btree`**: Índice primário (ID → posição no arquivo hash)

```- **`indice_secundario.btree`**: Índice secundário (Título → ID)



### Execução dos Programas## 🔍 Exemplos de Uso



**Upload de dados:**### Preparação dos Dados

```bash

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/upload /data/sample.csv```bash

```# Se necessário, extrair dados comprimidos

cd /app && gunzip -k data/artigo.csv.gz

**Busca direta por ID:**

```bash# Ou usar dados de exemplo

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/findrec 5cp data/sample.csv data/input.csv

``````



**Busca via índice primário:**### Sequência Completa de Testes

```bash

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek1 3```bash

```# 1. Build

docker build -t tp2-bd1 .

**Busca via índice secundário:**

```bash# 2. Upload (usando dados de exemplo)

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek2 "Título exato do artigo"docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/upload /data/sample.csv

```

# 3. Testes de busca

### Execução com Docker Composedocker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/findrec 1

```bashdocker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek1 1

# Upload de dadosdocker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek2 "Poster: 3D sketching and flexible input for surface design: A case study."

docker-compose --profile upload up```



# Busca interativa## 📊 Saída Esperada

docker-compose --profile interactive up

```Cada programa exibe:

- **Caminhos dos arquivos** utilizados

## Variáveis de Ambiente- **Registro encontrado** (se existir) com todos os campos

- **Estatísticas de acesso**:

- **CSV_PATH**: Caminho para arquivo CSV de entrada (padrão: `/data/input.csv`)  - Quantidade de blocos lidos

- **DATA_DIR**: Diretório para arquivos de dados (padrão: `/data/db`)  - Total de blocos do arquivo

- **LOG_LEVEL**: Nível de log (error, warn, info, debug)  - Tempo de execução em microssegundos

  - Porcentagem do arquivo acessada

## Exemplo de Uso Completo

### Exemplo de Saída do `findrec`

```bash

# 1. Build da imagem```

docker build -t tp2-bd1 .=== FINDREC - Busca Direta no Arquivo Hash ===

Procurando ID: 1

# 2. Upload dos dadosArquivo: /data/db/dados.hash

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/upload /data/sample.csv

=== RESULTADOS DA BUSCA ===

# 3. Busca diretaREGISTRO ENCONTRADO:

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/findrec 1-------------------

ID: 1

# 4. Busca via índice primárioTítulo: Poster: 3D sketching and flexible input for surface design: A case study.

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek1 1Ano: 2013

Autores: Anamary Leal|Doug A. Bowman

# 5. Busca via índice secundárioCitações: 0

docker run --rm -v $(pwd)/data:/data tp2-bd1 ./bin/seek2 "Design and implementation of an immersive virtual reality system based on a smartphone platform."Atualização: 2016-07-28 09:36:29

```Snippet: Poster: 3D sketching and flexible input...



## Exemplo de Saída=== ESTATÍSTICAS DE ACESSO ===

Blocos lidos: 1

### UploadTotal de blocos no arquivo: 157

```Tempo de busca: 245 µs

=== UPLOAD - Carregamento de Dados ===Porcentagem do arquivo acessada: 0.64%

Arquivo CSV: /data/sample.csv```

Diretório de dados: /data/db

Total de registros estimado: 20## ⚙️ Configuração

Registros inseridos: 20

Tempo de execução: 4 ms### Variáveis de Ambiente

Taxa de inserção: 5000 registros/s

```- **`CSV_PATH`**: Caminho para o arquivo CSV de entrada (padrão: `/data/input.csv`)

- **`DATA_DIR`**: Diretório para arquivos de dados e índices (padrão: `/data/db`)

### Busca Direta (findrec)- **`LOG_LEVEL`**: Nível de log - error, warn, info, debug (padrão: `info`)

```

=== FINDREC - Busca Direta no Arquivo Hash ===### Parâmetros de Performance

Procurando ID: 5

REGISTRO ENCONTRADO:- **Tamanho da tabela hash**: Calculado automaticamente como 10/7 do número de registros

ID: 5- **Fator de carga**: ~0.7 para reduzir colisões

Título: Design and implementation...- **Tamanho do bloco**: 4KB (definido em `BLOCK_SIZE`)

Blocos lidos: 1- **Ordem da B+Tree**: 100 chaves por nó

Total de blocos no arquivo: 14

Tempo de busca: 1029 µs## 🐛 Troubleshooting

Porcentagem do arquivo acessada: 7.14%

```### Problemas Comuns



### Busca via Índice (seek1)1. **"Arquivo CSV não encontrado"**

```   - Verifique se o arquivo está montado corretamente no volume `/data`

=== SEEK1 - Busca via Índice Primário ===   - Use caminho absoluto: `/data/input.csv`

Procurando ID: 5

Blocos lidos no índice: 12. **"Erro ao criar diretório de dados"**

Total de blocos lidos (índice + dados): 2   - Verifique permissões do volume montado

Tempo total de busca: 1064 µs   - O diretório `/data/db` é criado automaticamente

Porcentagem total dos arquivos acessada: 13.33%

```3. **"Registro não encontrado"**

   - Verifique se o upload foi executado com sucesso

## Estruturas de Dados Implementadas   - Confirme se o ID ou título existe nos dados



### Hash Table4. **"Tabela hash cheia"**

- **Função hash**: `ID % table_size`   - Arquivo de dados muito pequeno para o número de registros

- **Resolução de colisões**: Linear probing   - Reexecute o upload ou aumente o fator de carga

- **Tamanho de bloco**: 4096 bytes

- **Fator de carga**: ~0.7### Debug



### B+Tree (Implementação Simplificada)```bash

- **Índice primário**: ID → posição no arquivo hash# Executar em modo interativo para debug

- **Índice secundário**: Título → ID do artigodocker run --rm -it -v $(pwd)/data:/data tp2-bd1 bash

- **Busca**: Binária em memória + persistência em disco

- **Inserção**: Ordenada automática# Verificar arquivos criados

ls -la /data/db/

## Decisões de Projeto

# Executar programas com output detalhado

1. **Serialização binária** para eficiência de E/SLOG_LEVEL=debug ./bin/upload /data/input.csv

2. **Implementação simplificada de B+Tree** usando std::vector para demonstrar conceitos```

3. **Linear probing** para resolução de colisões por simplicidade

4. **Campos de tamanho fixo** para facilitar cálculos de offset## 📋 Requisitos Técnicos

5. **Estatísticas detalhadas** para análise de performance

- **Linguagem**: C++17

## Arquivos de Dados Persistentes- **Compilador**: g++ com suporte a C++17

- **OS**: Linux (Debian Bookworm)

Os arquivos são criados no diretório `/data/db/`:- **Docker**: >= 20.10

- **dados.hash**: ~57KB (arquivo principal)- **Memória**: >= 512MB para datasets pequenos

- **indice_primario.btree**: ~2.4KB (índice por ID)- **Disco**: Espaço para ~3x o tamanho do CSV (dados + índices)

- **indice_secundario.btree**: ~31KB (índice por título)

## 🏆 Decisões de Projeto

## Limitações e Melhorias Futuras

### Estruturas de Dados

### Limitações Atuais

- B+Tree simplificada (não otimizada para muitos dados)1. **Hashing com Linear Probing**: Resolução simples de colisões

- Sem tratamento de overflow em campos de texto2. **B+Tree Simplificada**: Implementação básica para demonstração dos conceitos

- Hash table com tamanho fixo3. **Registros de Tamanho Fixo**: Facilita cálculos de posição e leitura

4. **Serialização Binária**: Eficiência no armazenamento

### Melhorias Sugeridas

- Implementar B+Tree completa com splits/merges### Escolhas de Implementação

- Adicionar índice por ano de publicação

- Otimizar serialização com compressão- **Blocos de 4KB**: Compatível com sistemas de arquivo modernos

- Implementar cache de blocos- **Índices em Arquivo**: Persistência entre execuções

- Adicionar suporte a consultas por range- **Medição de Performance**: Tempo em microssegundos, contagem de blocos

- **Tratamento de Erros**: Mensagens claras e códigos de retorno apropriados

## Testado e Validado

## 👥 Equipe

✅ Compilação local e via Docker  

✅ Upload de dados CSV  - **Desenvolvimento**: [Nome dos integrantes]

✅ Busca direta por hash funcionando  - **Documentação**: [Responsável pela documentação]

✅ Busca via índice primário funcionando  - **Testes**: [Responsável pelos testes]

✅ Busca via índice secundário funcionando  

✅ Estatísticas de blocos acessados  ## 📄 Licença

✅ Medição de tempo de execução  

✅ Persistência de dados entre execuções  Este projeto é desenvolvido para fins acadêmicos na disciplina de Bancos de Dados I - UFAM.


---

**Data de Entrega**: 24/10/2025  
**Equipe**: [Inserir nomes dos integrantes]
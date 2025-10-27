# Makefile para o projeto BD1-TP2

# Compilador e flags
CXX = g++
CXXFLAGS = -I./include
LDFLAGS = 

# Diretórios
SRCDIR = src
INCDIR = include
BINDIR = bin
DATADIR = data

# Arquivos fonte
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
HEADERS = $(wildcard $(INCDIR)/*.h)

# Executáveis
EXECUTABLES = $(BINDIR)/findrec $(BINDIR)/seek1 $(BINDIR)/seek2 $(BINDIR)/upload

# Regra principal
all: directories $(EXECUTABLES)

# Criar diretórios necessários
directories:
	@mkdir -p $(BINDIR)
	@mkdir -p $(DATADIR)/db

# Regras para cada executável
$(BINDIR)/findrec: $(SRCDIR)/findrec.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(BINDIR)/seek1: $(SRCDIR)/seek1.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(BINDIR)/seek2: $(SRCDIR)/seek2.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(BINDIR)/upload: $(SRCDIR)/upload.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# Limpeza
clean:
	rm -rf $(BINDIR)
	rm -rf $(DATADIR)/db

# Limpeza completa (incluindo dados)
distclean: clean
	rm -rf $(DATADIR)

# Rebuild completo
rebuild: clean all

# Upload de dados (assumindo que artigo.csv está em data/)
upload-data: $(BINDIR)/upload
	./$(BINDIR)/upload $(DATADIR)/artigo.csv

# Testes rápidos
test: $(EXECUTABLES)
	@echo "=== Testando executáveis ==="
	@if [ -f $(DATADIR)/db/hash.bin ]; then \
		echo "Testando findrec com ID 1:"; \
		./$(BINDIR)/findrec 1; \
		echo; \
		echo "Testando seek1 com ID 1:"; \
		./$(BINDIR)/seek1 1; \
		echo; \
		echo "Testando seek2 com título '3D':"; \
		./$(BINDIR)/seek2 "3D"; \
	else \
		echo "Base de dados não encontrada. Execute 'make upload-data' primeiro."; \
	fi

# Informações sobre o projeto
info:
	@echo "=== Informações do Projeto BD1-TP2 ==="
	@echo "Executáveis disponíveis:"
	@echo "  - findrec: Busca registro por ID usando hash"
	@echo "  - seek1:   Busca registro por ID usando índice B+"
	@echo "  - seek2:   Busca registro por título usando índice B+"
	@echo "  - upload:  Carrega dados do CSV para o banco"
	@echo ""
	@echo "Uso:"
	@echo "  make            - Compila todos os executáveis"
	@echo "  make upload-data - Carrega dados do arquivo CSV"
	@echo "  make test       - Executa testes básicos"
	@echo "  make clean      - Remove executáveis e base de dados"
	@echo "  make info       - Mostra esta informação"

# Declarar targets que não são arquivos
.PHONY: all clean distclean rebuild upload-data test info directories

# Dependências automáticas para headers
$(BINDIR)/%: $(SRCDIR)/%.cpp $(HEADERS) | directories
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)
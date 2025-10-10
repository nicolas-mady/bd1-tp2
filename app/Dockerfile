# exemplo base: pode usar debian:bookworm-slim ou ubuntu:24.04
FROM ubuntu:22.04

# Evita prompts interativos durante instalação
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ make cmake && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copia apenas os arquivos necessários primeiro (melhor cache)
COPY include/ include/
COPY src/ src/
COPY Makefile .

# compile usando Makefile padrão (ajuste conforme seu build)
RUN make clean && make build

# diretório para dados persistentes (montado como volume)
VOLUME ["/data"]

# variáveis de ambiente para facilitar testes
ENV CSV_PATH=/data/input.csv \
    DATA_DIR=/data/db \
    LOG_LEVEL=info

# por padrão, mostra ajuda dos binários
CMD ["bash", "-lc", "echo 'Use: docker run ... upload|findrec|seek1|seek2'; ls -l bin/"]

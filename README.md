# Comandos

```sh
cd app/
```

## Build da imagem
```sh
docker build -t bd1-tp2 .
```

## Execução dos programas

### Upload
```sh
docker run --rm -v $(pwd)/data:/app/data bd1-tp2 ./bin/upload data/artigo.csv
```

### Testes individuais
```sh
docker run --rm -v $(pwd)/data:/app/data bd1-tp2 ./bin/findrec 1
```
```sh
docker run --rm -v $(pwd)/data:/app/data bd1-tp2 ./bin/seek1 1
```
```sh
docker run --rm -v $(pwd)/data:/app/data bd1-tp2 ./bin/seek2 "3D"
```

# Layout
```
app/
├── data/
    ├── db/          (binários gerados)
        ├── hash.bin
        ├── idx1.bin
        └── idx2.bin
```

# Exemplo
entrada:
```sh
docker run --rm -v $(pwd)/data:/app/data bd1-tp2 ./bin/seek1 1
```
saída:
```
=== seek1 1 ===
Buscando em data/db/idx1.bin
 [221 ms] 3 blocos lidos
         ID: 1
     Título: Poster: 3D sketching and flexible input for surface design: A case study.
        Ano: 2013
    Autores: Anamary Leal|Doug A. Bowman
   Citações: 0
Atualização: 2016-07-28 13:36:29
    Snippet: Poster: 3D sketching and flexible input for surface design: A case study. A Leal, DA Bowman -  Interfaces (3DUI), 2013 IEEE Symposium , 2013 - ieeexplore.ieee.org. ABSTRACT Designing three-dimensional (3D) surfaces is difficult in both the physical world  and in 3D modeling software, requiring background knowledge and skill. The goal of this  work is to make 3D surface design easier and more accessible through natural and  ..
```


### Para gerar o bin/
```sh
make
```
# Dominating Sets - AED Projeto

Implementação de algoritmos para cálculo de conjuntos dominantes em grafos.

## Compilação

Para compilar todos os programas:

```bash
make
```

## Testes

### Teste principal - Conjuntos Dominantes

```bash
./TestDominatingSets
```

Este programa testa os algoritmos de cálculo de conjuntos dominantes mínimos (cardinalidade e peso) nos grafos disponíveis (G_1.txt, G_2.txt, G_3.txt, G_4.txt).

### Outros testes

```bash
./TestGraphBasics      # Testa operações básicas de grafos
./TestIndicesSet       # Testa conjunto de índices
```

## Ficheiros de Grafos

Os grafos de teste estão nos ficheiros:
- `G_1.txt` 
- `G_2.txt` 
- `G_3.txt` 
- `G_4.txt` 
- `DG_2.txt` 

## Limpar

Para remover ficheiros compilados:

```bash
make clean
```

## Análise de Memória

Para verificar fugas de memória com Valgrind:

```bash
valgrind --leak-check=full ./TestDominatingSets
```

# README

## Desenvolvedores
João Victor – Matrícula: 
Gabriel Santos – Matrícula: 
Luís Gustavo Morais Cardoso – Matrícula: 202220819

##Projeto: Conversão CSV para Binário e Ordenação Externa (Merge Multiway)

## Sobre o Projeto
Este projeto implementa, em C++, um sistema de manipulação de registros esportivos extraídos de um arquivo CSV, com armazenamento direto em arquivo binário. Oferece operações de inserção, visualização, alteração e troca de registros em posições arbitrárias, sem jamais carregar todo o arquivo na memória. Para ordenação dos dados, utiliza o algoritmo de Merge Multiway (ordenador externo), capaz de processar grandes volumes de registros.

### Estruturas
- Struct Registro
Guarda os campos de cada atleta:

int    id;
char   name[100];
char   team[50];
char   games[100];
char   year[50];
char   season[50];

- Classe Manipula
Encapsula todas as operações sobre o arquivo binário:

transfereRegistros(...) – converte CSV em registros binários.

imprimeTodosRegistros(...) – lista o conteúdo atual.

ordenaExternaPorID(...) – cria runs, ordena e chama o merge multiway.

mergeArquivosOrdenados(...) – intercala várias partições ordenadas.

adicionaNaPosicao(...) – insere um registro em posição específica.

imprimeRegistrosOrdenados(...) – lista o arquivo já ordenado.

### Funcionalidades
C – Converte e carrega todos os registros do CSV (data_athlete_event.csv) para arquivo.bin.

L – Lista todos os registros armazenados em arquivo.bin.

O – Ordena externamente arquivo.bin por id (crescente), produzindo arquivo_ordenado.bin.

LO – Lista os registros de arquivo_ordenado.bin.

A <posição> – Adiciona um novo registro em qualquer posição (0‑based), deslocando o restante.

F – Finaliza o programa.

## Instruções de Uso
1- Compilação
g++ -Wall -Wextra -std=c++11 main.cpp -o programa
(Substitua main.cpp pelo nome do seu arquivo fonte.)

2- Execução
./programa
Será exibido o menu de comandos.

3- Carregar dados
Digite C para ler data_athlete_event.csv e gerar arquivo.bin.

4- Listar registros
L – mostra tudo em arquivo.bin.
LO – mostra tudo em arquivo_ordenado.bin (após ordenar).

5- Ordenar
Digite O para executar o Merge Multiway:
Gera runs ordenados em arquivos temporários.
Intercala todos, formando arquivo_ordenado.bin.

6- Inserir em posição
Digite A e informe a posição de inserção (0‑based), em seguida preencha os campos.

7- Finalizar
Digite F para sair.

## Requisitos 
Linguagem: C++11 ou superior
Bibliotecas: <iostream>, <fstream>, <sstream>, <string>, <cstring>
Compilador: g++ (MinGW no Windows ou equivalente em Linux)

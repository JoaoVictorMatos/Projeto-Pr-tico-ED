# README

## Desenvolvedores

- **João Victor**
- **Gabriel Santos**
- **Luís Gustavo Morais** 

---

## Sobre o Projeto

Este projeto implementa um sistema em C++ que realiza a conversão de registros a partir de um arquivo CSV para um arquivo binário. O sistema oferece funcionalidades para visualização, inserção, alteração, troca e ordenação de registros diretamente no arquivo, sem carregar todo o conteúdo em memória. A ordenação externa é feita utilizando o método de **intercalação de dois caminhos**.

---

## Estruturas

### `struct Registro`

Estrutura utilizada para armazenar os dados dos atletas. Contém os seguintes campos:

- int    id : Identificador do atleta;
- char   name[100] : Nome do atleta;
- char   city[50] : Cidade;
- char   sport[50] : Modalidade esportiva;
- char   event[100] : Evento esportivo;
- char   noc[10] : Código do Comitê Olímpico Nacional;

## Funcionalidades
- **converterCSVParaBinario()**:
Lê os dados de um arquivo CSV e escreve os registros no formato binário.

- **imprimirTodos()**:
Exibe todos os registros salvos no arquivo binário.

- **visualizarIntervalo()**:
Mostra os registros entre duas posições específicas.

- **inserirNaPosicao()**:
Insere um novo registro em qualquer posição do arquivo binário, utilizando um arquivo temporário para realocar os dados.

- **alterarRegistro()**:
Altera um registro existente com base em sua posição no arquivo.

- **trocarRegistros()**:
Troca o conteúdo de duas posições do arquivo binário.

- **ordenarArquivo()**:
Realiza a ordenação externa do arquivo binário usando intercalação de dois caminhos, com blocos de tamanho crescente.

## Instruções de Uso
1. Compilação

g++ -Wall -Wextra -std=c++11 main.cpp -o programa

2. Execução

./programa (O menu de comandos será exibido no terminal)

3. Menu Principal

1 - Converter CSV para Binário
2 - Inserir Registro em Posição Específica
3 - Visualizar Intervalo de Registros
4 - Alterar Registro por Posição
5 - Trocar Registros
6 - Imprimir Todos os Registros
7 - Ordenar Arquivo (Intercalação Externa)
0 - Sair do Programa

## Requisitos
- Linguagem: C++

- Bibliotecas: iostream, fstream, cstring, cstdlib

- Sistema de Arquivos: Leitura e escrita em arquivos binários utilizando fstream

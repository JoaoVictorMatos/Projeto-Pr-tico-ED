# README

## Desenvolvedores

- **João Victor**
- **Gabriel Santos**
- **Luís Gustavo Morais Cardoso** 

##Projeto: Manipulação de Arquivo Binário e Ordenação Externa

## Sobre o Projeto
Este projeto implementa um sistema em C++ que realiza a conversão de registros a partir de um arquivo CSV para um arquivo binário. O sistema oferece funcionalidades para visualização, inserção, alteração, troca e ordenação de registros diretamente no arquivo, sem carregar todo o conteúdo em memória. A ordenação externa é feita utilizando o método de intercalação de dois caminhos.

### Estruturas

**Struct Registro**
Estrutura utilizada para armazenar os dados dos atletas. Contém os seguintes campos:

int **id**: identificador do atleta;

chat **name**: nome do atleta;

char **city**: cidade;

char **sport**: modalidade esportiva;

char **event**: evento esportivo;

char **noc**: código do Comitê Olímpico Nacional.

### Funcionalidades

1. **converterCSVParaBinario()**: Lê os dados de um arquivo CSV e escreve os registros no formato binário.

2. **imprimirTodos()**: Exibe todos os registros salvos no arquivo binário.

3. **visualizarIntervalo()**: Mostra os registros entre duas posições específicas.

4. **inserirNaPosicao()**: Insere um novo registro em qualquer posição do arquivo binário, utilizando um arquivo temporário para realocar os dados.

5. **alterarRegistro()**: Altera um registro existente com base em sua posição no arquivo.

6. **trocarRegistros()**: Troca o conteúdo de duas posições do arquivo binário.

7. **ordenarArquivo()**: Realiza a ordenação externa do arquivo binário usando intercalação de dois caminhos, com blocos de tamanho crescente.

## Instruções de Uso

1. **Compilação**
g++ -Wall -Wextra -std=c++11 main.cpp -o programa
(Substitua main.cpp pelo nome do seu arquivo fonte.)

2. **Execução**
./programa
Será exibido o menu de comandos.

3. **Menu Principal**
Ao executar, o usuário terá acesso ao seguinte menu de opções:

1: Converter CSV para Binário
2: Inserir Registro em Posição Específica
3: Visualizar Intervalo de Registros
4: Alterar Registro por Posição
5: Trocar Registros
6: Imprimir Todos os Registros
7: Ordenar Arquivo (Intercalação Externa)
0: Sair do Programa


## Requisitos 

**Linguagem**: C++
**Bibliotecas**: <iostream>, <fstream>, <cstring>, <cstdlib>
**Sistema de arquivos**: leitura e escrita em arquivos binários com uso de fstream

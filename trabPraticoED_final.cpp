#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;

#define MAX_NAME 100
#define MAX_CITY 50
#define MAX_SPORT 50
#define MAX_EVENT 100
#define MAX_NOC 10

struct Registro
{
    int id;
    char name[MAX_NAME];
    char city[MAX_CITY];
    char sport[MAX_SPORT];
    char event[MAX_EVENT];
    char noc[MAX_NOC];
};

void converterCSVParaBinario(const char *nomeCsv, const char *nomeBinario)
{
    ifstream arquivoCsv(nomeCsv);
    ofstream arquivoBinario(nomeBinario, ios::binary);

    if (!arquivoCsv || !arquivoBinario)
    {
        cout << "Erro ao abrir os arquivos.\n";
        return;
    }

    string linha;
    getline(arquivoCsv, linha); // Ignora cabeçalho

    while (getline(arquivoCsv, linha))
    {
        Registro registro;
        char linhaConvertida[1024];
        strncpy(linhaConvertida, linha.c_str(), sizeof(linhaConvertida));
        linhaConvertida[sizeof(linhaConvertida) - 1] = '\0';

        char *token = strtok(linhaConvertida, ",");
        registro.id = atoi(token);

        token = strtok(NULL, ",");
        strncpy(registro.name, token, MAX_NAME);

        token = strtok(NULL, ",");
        strncpy(registro.city, token, MAX_CITY);

        token = strtok(NULL, ",");
        strncpy(registro.sport, token, MAX_SPORT);

        token = strtok(NULL, ",");
        strncpy(registro.event, token, MAX_EVENT);

        token = strtok(NULL, ",");
        strncpy(registro.noc, token, MAX_NOC);

        arquivoBinario.write((char *)&registro, sizeof(Registro));
    }

    arquivoCsv.close();
    arquivoBinario.close();
    cout << "Conversao concluida.\n";
}

void imprimirRegistro(const Registro &registro)
{
    cout << registro.id << ", " << registro.name << ", " << registro.city << ", "
         << registro.sport << ", " << registro.event << ", " << registro.noc << endl;
}

void imprimirTodosRegistros(const char *nomeBinario)
{
    ifstream arquivo(nomeBinario, ios::binary);
    if (!arquivo)
    {
        cout << "Erro ao abrir o arquivo binario.\n";
        return;
    }
    Registro registro;
    while (arquivo.read((char *)&registro, sizeof(Registro)))
    {
        imprimirRegistro(registro);
    }
    arquivo.close();
}

void visualizarIntervalo(const char *nomeBinario, int inicio, int fim)
{
    ifstream arquivo(nomeBinario, ios::binary);
    if (!arquivo)
    {
        cout << "Erro ao abrir o arquivo.\n";
        return;
    }
    Registro registro;
    int posicao = 0;
    while (arquivo.read((char *)&registro, sizeof(Registro)))
    {
        if (posicao >= inicio && posicao <= fim)
        {
            imprimirRegistro(registro);
        }
        posicao++;
    }
    arquivo.close();
}

void alterarRegistro(const char *nomeBinario, int posicao, Registro novoRegistro)
{
    fstream arquivo(nomeBinario, ios::in | ios::out | ios::binary);
    if (!arquivo)
    {
        cout << "Erro ao abrir o arquivo.\n";
        return;
    }
    arquivo.seekp(posicao * sizeof(Registro));
    arquivo.write((char *)&novoRegistro, sizeof(Registro));
    arquivo.close();
}

void trocarRegistros(const char *nomeBinario, int posicao1, int posicao2)
{
    fstream arquivo(nomeBinario, ios::in | ios::out | ios::binary);
    if (!arquivo)
    {
        cout << "Erro ao abrir o arquivo.\n";
        return;
    }
    Registro registro1, registro2;

    arquivo.seekg(posicao1 * sizeof(Registro));
    arquivo.read((char *)&registro1, sizeof(Registro));

    arquivo.seekg(posicao2 * sizeof(Registro));
    arquivo.read((char *)&registro2, sizeof(Registro));

    arquivo.seekp(posicao1 * sizeof(Registro));
    arquivo.write((char *)&registro2, sizeof(Registro));

    arquivo.seekp(posicao2 * sizeof(Registro));
    arquivo.write((char *)&registro1, sizeof(Registro));

    arquivo.close();
}

void inserirNaPosicao(const char *nomeBinario, int posicao, Registro novoRegistro)
{
    ifstream arquivoEntrada(nomeBinario, ios::binary);
    ofstream arquivoTemp("temp.bin", ios::binary);
    if (!arquivoEntrada || !arquivoTemp)
    {
        cout << "Erro ao abrir arquivos.\n";
        return;
    }
    Registro registroAtual;
    int contador = 0;
    while (arquivoEntrada.read((char *)&registroAtual, sizeof(Registro)))
    {
        if (contador == posicao)
        {
            arquivoTemp.write((char *)&novoRegistro, sizeof(Registro));
        }
        arquivoTemp.write((char *)&registroAtual, sizeof(Registro));
        contador++;
    }
    if (posicao >= contador)
    {
        arquivoTemp.write((char *)&novoRegistro, sizeof(Registro));
    }
    arquivoEntrada.close();
    arquivoTemp.close();
    remove(nomeBinario);
    rename("temp.bin", nomeBinario);
}

void ordenarArquivo(const char *nomeBinario)
{
    const char *arquivoTemp1 = "temp1.bin";
    const char *arquivoTemp2 = "temp2.bin";

    bool arquivoOrdenado = false;
    int tamanhoBloco = 1;

    while (!arquivoOrdenado)
    {
        ifstream entrada(nomeBinario, ios::binary);
        ofstream saida1(arquivoTemp1, ios::binary);
        ofstream saida2(arquivoTemp2, ios::binary);

        Registro registro;
        int totalRegistros = 0;

        while (entrada.read((char *)&registro, sizeof(Registro)))
        {
            if (totalRegistros / tamanhoBloco % 2 == 0)
                saida1.write((char *)&registro, sizeof(Registro));
            else
                saida2.write((char *)&registro, sizeof(Registro));
            totalRegistros++;
        }
        entrada.close();
        saida1.close();
        saida2.close();

        ifstream leitura1(arquivoTemp1, ios::binary);
        ifstream leitura2(arquivoTemp2, ios::binary);
        ofstream saidaFinal(nomeBinario, ios::binary);

        Registro registro1, registro2;
        bool temRegistro1 = leitura1.read((char *)&registro1, sizeof(Registro)).gcount() == sizeof(Registro);
        bool temRegistro2 = leitura2.read((char *)&registro2, sizeof(Registro)).gcount() == sizeof(Registro);

        while (temRegistro1 || temRegistro2)
        {
            int i = 0, j = 0;
            Registro *buffer1 = new Registro[tamanhoBloco];
            Registro *buffer2 = new Registro[tamanhoBloco];

            while (i < tamanhoBloco && temRegistro1)
            {
                buffer1[i] = registro1;
                i++;
                temRegistro1 = leitura1.read((char *)&registro1, sizeof(Registro)).gcount() == sizeof(Registro);
            }

            while (j < tamanhoBloco && temRegistro2)
            {
                buffer2[j] = registro2;
                j++;
                temRegistro2 = leitura2.read((char *)&registro2, sizeof(Registro)).gcount() == sizeof(Registro);
            }

            int x = 0, y = 0;
            while (x < i && y < j)
            {
                if (buffer1[x].id <= buffer2[y].id)
                {
                    saidaFinal.write((char *)&buffer1[x], sizeof(Registro));
                    x++;
                }
                else
                {
                    saidaFinal.write((char *)&buffer2[y], sizeof(Registro));
                    y++;
                }
            }

            while (x < i)
            {
                saidaFinal.write((char *)&buffer1[x], sizeof(Registro));
                x++;
            }
            while (y < j)
            {
                saidaFinal.write((char *)&buffer2[y], sizeof(Registro));
                y++;
            }

            delete[] buffer1;
            delete[] buffer2;
        }

        leitura1.close();
        leitura2.close();
        saidaFinal.close();

        tamanhoBloco *= 2;
        if (tamanhoBloco >= totalRegistros)
        {
            arquivoOrdenado = true;
        }
    }

    remove(arquivoTemp1);
    remove(arquivoTemp2);
    cout << "Ordenacao concluida.\n";
}

int main()
{
    int opcao;
    char nomeCsv[] = "data_athlete_event.csv";
    char nomeBinario[] = "arquivo.bin";

    do
    {
        cout << "\nMenu:\n";
        cout << "1 - Converter CSV para Binario\n";
        cout << "2 - Inserir Registro\n";
        cout << "3 - Visualizar Intervalo\n";
        cout << "4 - Alterar Registro\n";
        cout << "5 - Trocar Registros\n";
        cout << "6 - Imprimir Todos\n";
        cout << "7 - Ordenar Arquivo (Intercalação Externa)\n";
        cout << "0 - Sair\n";
        cout << "Escolha: ";
        cin >> opcao;

        if (opcao == 1)
        {
            converterCSVParaBinario(nomeCsv, nomeBinario);
        }
        else if (opcao == 2)
        {
            Registro novoRegistro;
            cout << "ID: ";
            cin >> novoRegistro.id;
            cin.ignore();
            cout << "Name: ";
            cin.getline(novoRegistro.name, MAX_NAME);
            cout << "City: ";
            cin.getline(novoRegistro.city, MAX_CITY);
            cout << "Sport: ";
            cin.getline(novoRegistro.sport, MAX_SPORT);
            cout << "Event: ";
            cin.getline(novoRegistro.event, MAX_EVENT);
            cout << "NOC: ";
            cin.getline(novoRegistro.noc, MAX_NOC);
            int posicao;
            cout << "Posicao: ";
            cin >> posicao;
            inserirNaPosicao(nomeBinario, posicao, novoRegistro);
        }
        else if (opcao == 3)
        {
            int inicio, fim;
            cout << "De: ";
            cin >> inicio;
            cout << "Ate: ";
            cin >> fim;
            visualizarIntervalo(nomeBinario, inicio, fim);
        }
        else if (opcao == 4)
        {
            Registro novoRegistro;
            int posicao;
            cout << "Posicao: ";
            cin >> posicao;
            cout << "ID: ";
            cin >> novoRegistro.id;
            cin.ignore();
            cout << "Name: ";
            cin.getline(novoRegistro.name, MAX_NAME);
            cout << "City: ";
            cin.getline(novoRegistro.city, MAX_CITY);
            cout << "Sport: ";
            cin.getline(novoRegistro.sport, MAX_SPORT);
            cout << "Event: ";
            cin.getline(novoRegistro.event, MAX_EVENT);
            cout << "NOC: ";
            cin.getline(novoRegistro.noc, MAX_NOC);
            alterarRegistro(nomeBinario, posicao, novoRegistro);
        }
        else if (opcao == 5)
        {
            int posicao1, posicao2;
            cout << "Posicao 1: ";
            cin >> posicao1;
            cout << "Posicao 2: ";
            cin >> posicao2;
            trocarRegistros(nomeBinario, posicao1, posicao2);
        }
        else if (opcao == 6)
        {
            imprimirTodosRegistros(nomeBinario);
        }
        else if (opcao == 7)
        {
            ordenarArquivo(nomeBinario);
        }
    } while (opcao != 0);

    return 0;
}

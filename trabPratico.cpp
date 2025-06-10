#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#define MAX_BLOCOS 100
#define TAMANHO_BLOCO 100

using namespace std;

// registro dos dados
struct Registro
{
    int id;
    char name[100];
    char team[50];
    char games[100];
    char year[50];
    char season[50];
};

// classe responsável pela manipulação dos dados
class Manipula
{
public:
    void transfereRegistros(fstream &arqBin, ifstream &arqCSV);
    void imprimeTodosRegistros(fstream &arqBin);
    void ordenaExternaPorID(const char *arquivoOriginal);
    void mergeArquivosOrdenados(const char *arquivosTemporarios[], int total, const char *arquivoFinal);
    void adicionaNaPosicao(const char *nomeArquivo, const Registro &novoRegistro, int posicao);
};

void Manipula::transfereRegistros(fstream &arqBin, ifstream &arqCSV)
{
    string linha;
    getline(arqCSV, linha); // ignora cabeçalho

    while (getline(arqCSV, linha))
    {
        stringstream ss(linha);
        string campo;
        Registro reg;

        getline(ss, campo, ',');
        reg.id = stoi(campo);

        getline(ss, campo, ',');
        strncpy(reg.name, campo.c_str(), sizeof(reg.name));
        reg.name[sizeof(reg.name) - 1] = '\0';

        getline(ss, campo, ',');
        strncpy(reg.team, campo.c_str(), sizeof(reg.team));
        reg.team[sizeof(reg.team) - 1] = '\0';

        getline(ss, campo, ',');
        strncpy(reg.games, campo.c_str(), sizeof(reg.games));
        reg.games[sizeof(reg.games) - 1] = '\0';

        getline(ss, campo, ',');
        strncpy(reg.year, campo.c_str(), sizeof(reg.year));
        reg.year[sizeof(reg.year) - 1] = '\0';

        getline(ss, campo, ',');
        strncpy(reg.season, campo.c_str(), sizeof(reg.season));
        reg.season[sizeof(reg.season) - 1] = '\0';

        arqBin.write(reinterpret_cast<char *>(&reg), sizeof(Registro));
    }
}

void Manipula::imprimeTodosRegistros(fstream &arqBin)
{
    Registro reg;
    arqBin.clear();
    arqBin.seekg(0, ios::beg);

    while (arqBin.read(reinterpret_cast<char *>(&reg), sizeof(Registro)))
    {
        cout << "ID: " << reg.id << endl;
        cout << "Name: " << reg.name << endl;
        cout << "Team: " << reg.team << endl;
        cout << "Games: " << reg.games << endl;
        cout << "Year: " << reg.year << endl;
        cout << "Season: " << reg.season << endl;
        cout << "--------------------------" << endl;
    }
}

void Manipula::ordenaExternaPorID(const char *arquivoOriginal)
{
    fstream arq(arquivoOriginal, ios::in | ios::binary);
    if (!arq.is_open())
    {
        cerr << "Erro ao abrir o arquivo para ordenação externa.\n";
        return;
    }

    Registro buffer[TAMANHO_BLOCO];
    char nomesTemporarios[MAX_BLOCOS][20];
    int totalTemporarios = 0;
    bool terminou = false;

    while (!terminou && totalTemporarios < MAX_BLOCOS)
    {
        int count = 0;

        while (count < TAMANHO_BLOCO && arq.read(reinterpret_cast<char *>(&buffer[count]), sizeof(Registro)))
        {
            count++;
        }

        if (count == 0)
        {
            terminou = true;
        }
        else
        {
            // Bubble sort por ID
            for (int i = 0; i < count - 1; ++i)
            {
                for (int j = 0; j < count - i - 1; ++j)
                {
                    if (buffer[j].id > buffer[j + 1].id)
                    {
                        Registro temp = buffer[j];
                        buffer[j] = buffer[j + 1];
                        buffer[j + 1] = temp;
                    }
                }
            }

            sprintf(nomesTemporarios[totalTemporarios], "temp%d.bin", totalTemporarios);
            fstream temp(nomesTemporarios[totalTemporarios], ios::out | ios::binary);
            if (!temp.is_open())
            {
                cerr << "Erro ao criar arquivo temporário.\n";
                return;
            }

            for (int i = 0; i < count; ++i)
            {
                temp.write(reinterpret_cast<char *>(&buffer[i]), sizeof(Registro));
            }

            temp.close();
            totalTemporarios++;
        }
    }

    arq.close();

    const char *nomes[MAX_BLOCOS];
    for (int i = 0; i < totalTemporarios; ++i)
        nomes[i] = nomesTemporarios[i];

    mergeArquivosOrdenados(nomes, totalTemporarios, "arquivo_ordenado.bin");

    for (int i = 0; i < totalTemporarios; ++i)
        remove(nomesTemporarios[i]);
}

void Manipula::mergeArquivosOrdenados(const char *arquivosTemporarios[], int total, const char *arquivoFinal)
{
    fstream arquivos[MAX_BLOCOS];
    Registro buffer[MAX_BLOCOS];
    bool ativo[MAX_BLOCOS];

    for (int i = 0; i < total; ++i)
    {
        arquivos[i].open(arquivosTemporarios[i], ios::in | ios::binary);
        if (!arquivos[i].is_open())
        {
            cerr << "Erro ao abrir arquivo temporário para mesclagem.\n";
            return;
        }

        arquivos[i].read(reinterpret_cast<char *>(&buffer[i]), sizeof(Registro));
        ativo[i] = arquivos[i].gcount() == sizeof(Registro);
    }

    fstream saida(arquivoFinal, ios::out | ios::binary);
    if (!saida.is_open())
    {
        cerr << "Erro ao criar arquivo final ordenado.\n";
        return;
    }

    bool finalizado = false;
    while (!finalizado)
    {
        int menor = -1;

        for (int i = 0; i < total; ++i)
        {
            if (ativo[i])
            {
                if (menor == -1 || buffer[i].id < buffer[menor].id)
                    menor = i;
            }
        }

        if (menor == -1)
        {
            finalizado = true;
        }
        else
        {
            saida.write(reinterpret_cast<char *>(&buffer[menor]), sizeof(Registro));
            arquivos[menor].read(reinterpret_cast<char *>(&buffer[menor]), sizeof(Registro));
            ativo[menor] = arquivos[menor].gcount() == sizeof(Registro);
        }
    }

    saida.close();

    for (int i = 0; i < total; ++i)
        arquivos[i].close();
}

void Manipula::adicionaNaPosicao(const char *nomeArquivo, const Registro &novoRegistro, int posicao)
{
    fstream arq(nomeArquivo, ios::in | ios::out | ios::binary);
    if (!arq.is_open())
    {
        cerr << "Erro ao abrir o arquivo!" << endl;
        return;
    }

    // Descobrir tamanho total de registros
    arq.seekg(0, ios::end);
    int totalRegistros = arq.tellg() / sizeof(Registro);

    if (posicao > totalRegistros)
    {
        cerr << "Posição inválida: maior que o total de registros." << endl;
        arq.close();
        return;
    }

    // Ler todos os registros a partir da posição para um buffer
    int n = totalRegistros - posicao;
    Registro *buffer = new Registro[n];

    arq.seekg(posicao * sizeof(Registro), ios::beg);
    for (int i = 0; i < n; ++i)
        arq.read(reinterpret_cast<char *>(&buffer[i]), sizeof(Registro));

    // Voltar para a posição e escrever o novo registro
    arq.seekp(posicao * sizeof(Registro), ios::beg);
    arq.write(reinterpret_cast<const char *>(&novoRegistro), sizeof(Registro));

    // Reescrever os registros seguintes
    for (int i = 0; i < n; ++i)
        arq.write(reinterpret_cast<const char *>(&buffer[i]), sizeof(Registro));

    delete[] buffer;
    arq.close();
}

int main()
{
    ifstream dadosCSV("data_athlete_event.csv", ios::in);
    if (!dadosCSV.is_open())
    {
        cerr << "Erro: Não foi possível abrir o arquivo CSV!" << endl;
        return 1;
    }

    fstream dadosBIN("arquivo.bin", ios::out | ios::binary);
    if (!dadosBIN.is_open())
    {
        cerr << "Erro: Não foi possível criar o arquivo binário!" << endl;
        return 1;
    }

    cout << "----PROJETO PRATICO ESTRUTURA DE DADOS----" << endl;
    cout << "----Alunos: Joao Victor, Gabriel Santos e Luis Gustavo----" << endl;
    cout << "##Lista de comandos:##" << endl;
    cout << "C - para copiar os registros do arquivo CSV para o arquivo binário." << endl;
    cout << "L - para listar todos os registros" << endl;
    cout << "O - para ordenar os registros de forma externa no arquivo binário" << endl;
    cout << "Lo - para Listar os registros ordenados." << endl;
    cout << "A +... - para adicionar um registro em determinada posicao" << endl;
    cout << "F - finalizar o programa" << endl;

    string comando;
    cin >> comando;
    // vou terminar (JV)
    do
    {
        switch (comando)
        {
        case comando == "C":
            /* code */
            break;

        default:
            break;
        }
    } while (comando != "F");

    return 0;
}

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

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
    void adicionar(fstream &arqBin);
    void visualizaEntre(fstream &arqBin);
    void alteraRegistro(fstream &arqBin);
    void trocaRegistro(fstream &arqBin);
    void imprimeTodosRegistros(fstream &arqBin);
};

// construtor não necessário

void Manipula::transfereRegistros(fstream &arqBin, ifstream &arqCSV)
{

    string linha;

    // Ignora o cabeçalho do CSV
    getline(arqCSV, linha);

    while (getline(arqCSV, linha))
    {
        stringstream ss(linha);
        string campo;
        Registro reg;

        // Lê e converte os campos
        // ID
        getline(ss, campo, ',');
        reg.id = stoi(campo);

        // Name
        getline(ss, campo, ',');
        strncpy(reg.name, campo.c_str(), sizeof(reg.name));
        reg.name[sizeof(reg.name) - 1] = '\0';

        // Team
        getline(ss, campo, ',');
        strncpy(reg.team, campo.c_str(), sizeof(reg.team));
        reg.team[sizeof(reg.team) - 1] = '\0';

        // Games
        getline(ss, campo, ',');
        strncpy(reg.games, campo.c_str(), sizeof(reg.games));
        reg.games[sizeof(reg.games) - 1] = '\0';

        // Year
        getline(ss, campo, ',');
        strncpy(reg.year, campo.c_str(), sizeof(reg.year));
        reg.year[sizeof(reg.year) - 1] = '\0';

        // Season
        getline(ss, campo, ',');
        strncpy(reg.season, campo.c_str(), sizeof(reg.season));
        reg.season[sizeof(reg.season) - 1] = '\0';

        // Escreve o registro no arquivo binário
        arqBin.write(reinterpret_cast<char *>(&reg), sizeof(Registro));
    }
}

void Manipula::imprimeTodosRegistros(fstream &arqBin)
{
    Registro reg;
    arqBin.clear();            // limpa EOF flag
    arqBin.seekg(0, ios::beg); // volta para o início

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

int main()
{
    // abre o arquivo CSV
    ifstream dadosCSV("data_athlete_event.csv", ios::in);
    if (!dadosCSV.is_open())
    {
        cerr << "Erro: Não foi possível abrir o arquivo CSV!" << endl;
        return 1;
    }

    // Cria (ou sobrescreve) o arquivo binário
    fstream dadosBIN("arquivo.bin", ios::out | ios::binary);
    if (!dadosBIN.is_open())
    {
        cerr << "Erro: Não foi possível criar o arquivo binário!" << endl;
        return 1;
    }

    Manipula m;
    m.transfereRegistros(dadosBIN, dadosCSV);

    dadosBIN.close();
    dadosCSV.close();

    // Agora reabre para leitura
    fstream leituraBIN("arquivo.bin", ios::in | ios::binary);
    if (!leituraBIN.is_open())
    {
        cerr << "Erro: Não foi possível abrir o arquivo binário para leitura!" << endl;
        return 1;
    }

    cout << "Registros copiados com sucesso:\n";
    m.imprimeTodosRegistros(leituraBIN);
    leituraBIN.close();

    return 0;
}

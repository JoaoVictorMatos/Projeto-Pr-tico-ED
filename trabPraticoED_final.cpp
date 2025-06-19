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

struct Registro {
    int id;
    char name[MAX_NAME];
    char city[MAX_CITY];
    char sport[MAX_SPORT];
    char event[MAX_EVENT];
    char noc[MAX_NOC];
};

// Conversão CSV para Binário
void converterCSVParaBinario(const char* csvFile, const char* binFile) {
    ifstream csv(csvFile);
    ofstream bin(binFile, ios::binary);
    if (!csv || !bin) {
        cout << "Erro ao abrir arquivos.\n";
        return;
    }

    string linha;
    getline(csv, linha); // Ignorar cabeçalho

    while (getline(csv, linha)) {
        Registro r;
        char linhaC[1024];
        strncpy(linhaC, linha.c_str(), sizeof(linhaC));
        linhaC[sizeof(linhaC)-1] = '\0';

        char* token = strtok(linhaC, ",");
        r.id = atoi(token);

        token = strtok(NULL, ",");
        strncpy(r.name, token, MAX_NAME);

        token = strtok(NULL, ",");
        strncpy(r.city, token, MAX_CITY);

        token = strtok(NULL, ",");
        strncpy(r.sport, token, MAX_SPORT);

        token = strtok(NULL, ",");
        strncpy(r.event, token, MAX_EVENT);

        token = strtok(NULL, ",");
        strncpy(r.noc, token, MAX_NOC);

        bin.write((char*)&r, sizeof(Registro));
    }

    csv.close();
    bin.close();
    cout << "Conversao concluida.\n";
}

// Impressão de um Registro
void imprimirRegistro(const Registro& r) {
    cout << r.id << ", " << r.name << ", " << r.city << ", " << r.sport << ", " << r.event << ", " << r.noc << endl;
}

// Impressão de todos os registros
void imprimirTodos(const char* binFile) {
    ifstream bin(binFile, ios::binary);
    if (!bin) {
        cout << "Erro ao abrir arquivo binario.\n";
        return;
    }
    Registro r;
    while (bin.read((char*)&r, sizeof(Registro))) {
        imprimirRegistro(r);
    }
    bin.close();
}

// Visualizar intervalo
void visualizarIntervalo(const char* binFile, int inicio, int fim) {
    ifstream bin(binFile, ios::binary);
    if (!bin) {
        cout << "Erro ao abrir arquivo.\n";
        return;
    }
    Registro r;
    int pos = 0;
    while (bin.read((char*)&r, sizeof(Registro))) {
        if (pos >= inicio && pos <= fim) {
            imprimirRegistro(r);
        }
        pos++;
    }
    bin.close();
}

// Alterar registro
void alterarRegistro(const char* binFile, int pos, Registro novo) {
    fstream bin(binFile, ios::in | ios::out | ios::binary);
    if (!bin) {
        cout << "Erro ao abrir arquivo.\n";
        return;
    }
    bin.seekp(pos * sizeof(Registro));
    bin.write((char*)&novo, sizeof(Registro));
    bin.close();
}

// Trocar registros
void trocarRegistros(const char* binFile, int pos1, int pos2) {
    fstream bin(binFile, ios::in | ios::out | ios::binary);
    if (!bin) {
        cout << "Erro ao abrir arquivo.\n";
        return;
    }
    Registro r1, r2;

    bin.seekg(pos1 * sizeof(Registro));
    bin.read((char*)&r1, sizeof(Registro));

    bin.seekg(pos2 * sizeof(Registro));
    bin.read((char*)&r2, sizeof(Registro));

    bin.seekp(pos1 * sizeof(Registro));
    bin.write((char*)&r2, sizeof(Registro));

    bin.seekp(pos2 * sizeof(Registro));
    bin.write((char*)&r1, sizeof(Registro));

    bin.close();
}

// Inserção sem carregar tudo: via arquivo temporário
void inserirNaPosicao(const char* binFile, int pos, Registro novo) {
    ifstream in(binFile, ios::binary);
    ofstream out("temp.bin", ios::binary);
    if (!in || !out) {
        cout << "Erro ao abrir arquivos.\n";
        return;
    }
    Registro r;
    int i = 0;
    while (in.read((char*)&r, sizeof(Registro))) {
        if (i == pos) {
            out.write((char*)&novo, sizeof(Registro));
        }
        out.write((char*)&r, sizeof(Registro));
        i++;
    }
    if (pos >= i) {
        out.write((char*)&novo, sizeof(Registro));
    }
    in.close();
    out.close();
    remove(binFile);
    rename("temp.bin", binFile);
}

// Ordenação externa simples: Intercalação de dois caminhos
void ordenarArquivo(const char* binFile) {
    const char* temp1 = "temp1.bin";
    const char* temp2 = "temp2.bin";

    bool ordenado = false;
    int bloco = 1;

    while (!ordenado) {
        ifstream in(binFile, ios::binary);
        ofstream out1(temp1, ios::binary);
        ofstream out2(temp2, ios::binary);

        Registro r;
        int count = 0;

        while (in.read((char*)&r, sizeof(Registro))) {
            if (count / bloco % 2 == 0)
                out1.write((char*)&r, sizeof(Registro));
            else
                out2.write((char*)&r, sizeof(Registro));
            count++;
        }
        in.close();
        out1.close();
        out2.close();

        ifstream f1(temp1, ios::binary);
        ifstream f2(temp2, ios::binary);
        ofstream out(binFile, ios::binary);

        Registro r1, r2;
        bool has1 = (bool)f1.read((char*)&r1, sizeof(Registro));
		bool has2 = (bool)f2.read((char*)&r2, sizeof(Registro));

        while (has1 || has2) {
            int i = 0, j = 0;
            Registro* buffer1 = new Registro[bloco];
            Registro* buffer2 = new Registro[bloco];

            while (i < bloco && has1) {
                buffer1[i] = r1;
                i++;
			has1 = f1.read((char*)&r1, sizeof(Registro)).gcount() == sizeof(Registro);

            }

            while (j < bloco && has2) {
                buffer2[j] = r2;
                j++;
			has2 = f2.read((char*)&r2, sizeof(Registro)).gcount() == sizeof(Registro);
            }

            int x = 0, y = 0;
            while (x < i && y < j) {
                if (buffer1[x].id <= buffer2[y].id) {
                    out.write((char*)&buffer1[x], sizeof(Registro));
                    x++;
                } else {
                    out.write((char*)&buffer2[y], sizeof(Registro));
                    y++;
                }
            }

            while (x < i) {
                out.write((char*)&buffer1[x], sizeof(Registro));
                x++;
            }
            while (y < j) {
                out.write((char*)&buffer2[y], sizeof(Registro));
                y++;
            }

            delete[] buffer1;
            delete[] buffer2;
        }

        f1.close();
        f2.close();
        out.close();

        bloco *= 2;
        if (bloco >= count) {
            ordenado = true;
        }
    }

    remove(temp1);
    remove(temp2);
    cout << "Ordenacao concluida.\n";
}

int main() {
    int opcao;
    char csvFile[] = "data_athlete_event.csv";
    char binFile[] = "arquivo.bin";

    do {
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

        if (opcao == 1) {
            converterCSVParaBinario(csvFile, binFile);
        } else if (opcao == 2) {
            Registro novo;
            cout << "ID: "; cin >> novo.id; cin.ignore();
            cout << "Name: "; cin.getline(novo.name, MAX_NAME);
            cout << "City: "; cin.getline(novo.city, MAX_CITY);
            cout << "Sport: "; cin.getline(novo.sport, MAX_SPORT);
            cout << "Event: "; cin.getline(novo.event, MAX_EVENT);
            cout << "NOC: "; cin.getline(novo.noc, MAX_NOC);
            int pos;
            cout << "Posicao: "; cin >> pos;
            inserirNaPosicao(binFile, pos, novo);
        } else if (opcao == 3) {
            int ini, fim;
            cout << "De: "; cin >> ini;
            cout << "Ate: "; cin >> fim;
            visualizarIntervalo(binFile, ini, fim);
        } else if (opcao == 4) {
            Registro novo;
            int pos;
            cout << "Posicao: "; cin >> pos;
            cout << "ID: "; cin >> novo.id; cin.ignore();
            cout << "Name: "; cin.getline(novo.name, MAX_NAME);
            cout << "City: "; cin.getline(novo.city, MAX_CITY);
            cout << "Sport: "; cin.getline(novo.sport, MAX_SPORT);
            cout << "Event: "; cin.getline(novo.event, MAX_EVENT);
            cout << "NOC: "; cin.getline(novo.noc, MAX_NOC);
            alterarRegistro(binFile, pos, novo);
        } else if (opcao == 5) {
            int pos1, pos2;
            cout << "Posicao 1: "; cin >> pos1;
            cout << "Posicao 2: "; cin >> pos2;
            trocarRegistros(binFile, pos1, pos2);
        } else if (opcao == 6) {
            imprimirTodos(binFile);
        } else if (opcao == 7) {
            ordenarArquivo(binFile);
        }
    } while (opcao != 0);

    return 0;
}

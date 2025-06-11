#include <cstring>  // Para strncpy
#include <fstream>  // Para ifstream, fstream
#include <iostream> // Para cin, cout, cerr, endl
#include <sstream>  // Para stringstream
#include <string>   // Para std::string

#define MAX_BLOCOS 1000
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
    // Adicionar um método para listar registros ordenados, se "LO" for diferente de "L" após "O"
    void imprimeRegistrosOrdenados(const char *arquivoOrdenado);
};

void Manipula::transfereRegistros(fstream &arqBin, ifstream &arqCSV)
{
    // *** Importante: Limpar flags de erro e voltar ao início do CSV para reuso ***
    arqCSV.clear();
    arqCSV.seekg(0, ios::beg);

    string linha;
    getline(arqCSV, linha); // ignora cabeçalho

    // *** Importante: Posicionar o ponteiro de escrita no arquivo binário ***
    // Se você quer adicionar ao final, use ios::end. Se você quer sobrescrever,
    // a abertura inicial com ios::out | ios::binary já faz isso.
    // Como estamos na função transfereRegistros, assumimos que queremos escrever
    // desde o início ou continuar de onde parou. Para garantir que o arquivo binário
    // está pronto para a escrita desde o início (após um possível truncamento na abertura),
    // ou para adicionar novos registros, posicione o ponteiro.
    // Para um 'C' inicial, o arquivo provavelmente estará vazio, então seekp(0) é ok.
    // Se 'C' puder ser chamado várias vezes para anexar, você precisaria de arqBin.seekp(0, ios::end);
    // Para esta função, vamos assumir que ela sobrescreve o arquivo binário.
    arqBin.seekp(0, ios::beg);

    while (getline(arqCSV, linha))
    {
        stringstream ss(linha);
        string campo;
        Registro reg;

        // É crucial usar std::getline para cada campo
        if (!getline(ss, campo, ','))
            continue; // Se não conseguir ler ID, pula
        try
        {
            reg.id = stoi(campo);
        }
        catch (const std::invalid_argument &e)
        {
            cerr << "Erro ao converter ID: " << campo << " - " << e.what() << endl;
            continue; // Pula para a próxima linha
        }
        catch (const std::out_of_range &e)
        {
            cerr << "ID fora do range: " << campo << " - " << e.what() << endl;
            continue; // Pula para a próxima linha
        }

        if (!getline(ss, campo, ','))
            continue;
        strncpy(reg.name, campo.c_str(), sizeof(reg.name) - 1);
        reg.name[sizeof(reg.name) - 1] = '\0';

        if (!getline(ss, campo, ','))
            continue;
        strncpy(reg.team, campo.c_str(), sizeof(reg.team) - 1);
        reg.team[sizeof(reg.team) - 1] = '\0';

        if (!getline(ss, campo, ','))
            continue;
        strncpy(reg.games, campo.c_str(), sizeof(reg.games) - 1);
        reg.games[sizeof(reg.games) - 1] = '\0';

        if (!getline(ss, campo, ','))
            continue;
        strncpy(reg.year, campo.c_str(), sizeof(reg.year) - 1);
        reg.year[sizeof(reg.year) - 1] = '\0';

        if (!getline(ss, campo, '\r'))
        { // Lê até o '\r' ou final da linha para o último campo
            if (ss.eof())
            { // Último campo pode não ter vírgula ou \r, apenas final da linha
                strncpy(reg.season, campo.c_str(), sizeof(reg.season) - 1);
                reg.season[sizeof(reg.season) - 1] = '\0';
            }
            else
            {
                continue; // Se falhar, pula
            }
        }
        else
        {
            strncpy(reg.season, campo.c_str(), sizeof(reg.season) - 1);
            reg.season[sizeof(reg.season) - 1] = '\0';
        }

        arqBin.write(reinterpret_cast<char *>(&reg), sizeof(Registro));
    }
    arqBin.flush(); // Garante que os dados são escritos no arquivo
}

void Manipula::imprimeTodosRegistros(fstream &arqBin)
{
    Registro reg;
    arqBin.clear();            // Limpa quaisquer flags de erro
    arqBin.seekg(0, ios::beg); // Volta para o início do arquivo para leitura

    cout << "\n--- LISTANDO TODOS OS REGISTROS ---\n";
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
    if (arqBin.eof())
    {
        cout << "Fim do arquivo binário.\n";
    }
    else if (arqBin.fail())
    {
        cerr << "Erro de leitura no arquivo binário.\n";
    }
    // Não precisa de arqBin.clear() e arqBin.seekg(0, ios::beg) aqui
    // Se você vai chamar a função de novo, a própria função já faz isso.
}

void Manipula::ordenaExternaPorID(const char *arquivoOriginal)
{
    // Implementação da função de ordenação externa.
    // Lembre-se que "arquivo.bin" é o arquivo que você quer ordenar.
    // O resultado da ordenação externa será "arquivo_ordenado.bin".
    // Certifique-se de que a função de mergeArquivosOrdenados realmente cria
    // um NOVO arquivo "arquivo_ordenado.bin", e não modifica o original.
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

    cout << "Iniciando ordenação externa..." << endl;

    // Etapa 1: Leitura, ordenação interna e escrita de blocos temporários
    while (!terminou && totalTemporarios < MAX_BLOCOS)
    {
        int count = 0;

        // Lê um bloco de registros
        while (count < TAMANHO_BLOCO && arq.read(reinterpret_cast<char *>(&buffer[count]), sizeof(Registro)))
        {
            count++;
        }

        if (count == 0) // Fim do arquivo original
        {
            terminou = true;
        }
        else
        {
            // Bubble sort por ID dentro do buffer (bloco)
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

            // Cria e escreve no arquivo temporário
            sprintf(nomesTemporarios[totalTemporarios], "temp%d.bin", totalTemporarios);
            fstream temp(nomesTemporarios[totalTemporarios], ios::out | ios::binary);
            if (!temp.is_open())
            {
                cerr << "Erro ao criar arquivo temporário.\n";
                arq.close();
                return;
            }

            for (int i = 0; i < count; ++i)
            {
                temp.write(reinterpret_cast<char *>(&buffer[i]), sizeof(Registro));
            }

            temp.close();
            totalTemporarios++;
            cout << "Bloco " << totalTemporarios << " ordenado e salvo em " << nomesTemporarios[totalTemporarios - 1] << endl;
        }
    }

    arq.close(); // Fecha o arquivo original

    // Etapa 2: Mesclagem dos arquivos temporários
    const char *nomes_ptrs[MAX_BLOCOS]; // Array de ponteiros para const char
    for (int i = 0; i < totalTemporarios; ++i)
        nomes_ptrs[i] = nomesTemporarios[i];

    cout << "Iniciando mesclagem dos arquivos temporários..." << endl;
    mergeArquivosOrdenados(nomes_ptrs, totalTemporarios, "arquivo_ordenado.bin");
    cout << "Arquivos temporários mesclados para arquivo_ordenado.bin" << endl;

    // Etapa 3: Exclusão dos arquivos temporários
    for (int i = 0; i < totalTemporarios; ++i)
    {
        remove(nomesTemporarios[i]);
        cout << "Arquivo temporário " << nomesTemporarios[i] << " removido." << endl;
    }
    cout << "Ordenação externa concluída com sucesso." << endl;
}

void Manipula::mergeArquivosOrdenados(const char *arquivosTemporarios[], int total, const char *arquivoFinal)
{
    fstream arquivos[MAX_BLOCOS];
    Registro buffer[MAX_BLOCOS];
    bool ativo[MAX_BLOCOS]; // Indica se o arquivo temporário ainda tem registros

    // Abre todos os arquivos temporários e lê o primeiro registro de cada um
    for (int i = 0; i < total; ++i)
    {
        arquivos[i].open(arquivosTemporarios[i], ios::in | ios::binary);
        if (!arquivos[i].is_open())
        {
            cerr << "Erro ao abrir arquivo temporário para mesclagem: " << arquivosTemporarios[i] << "\n";
            // Fechar os arquivos já abertos antes de retornar
            for (int j = 0; j < i; ++j)
                arquivos[j].close();
            return;
        }

        // Tenta ler o primeiro registro; se falhar, o arquivo está vazio ou com erro
        arquivos[i].read(reinterpret_cast<char *>(&buffer[i]), sizeof(Registro));
        ativo[i] = arquivos[i].gcount() == sizeof(Registro);
    }

    // Abre o arquivo de saída final
    fstream saida(arquivoFinal, ios::out | ios::binary | ios::trunc); // trunc para garantir que está limpo
    if (!saida.is_open())
    {
        cerr << "Erro ao criar arquivo final ordenado: " << arquivoFinal << "\n";
        // Fechar todos os arquivos temporários antes de retornar
        for (int i = 0; i < total; ++i)
            arquivos[i].close();
        return;
    }

    bool finalizado = false;
    while (!finalizado)
    {
        int menor_indice = -1; // Índice do arquivo com o menor ID

        // Encontra o registro com o menor ID entre os arquivos ativos
        for (int i = 0; i < total; ++i)
        {
            if (ativo[i])
            {
                if (menor_indice == -1 || buffer[i].id < buffer[menor_indice].id)
                    menor_indice = i;
            }
        }

        if (menor_indice == -1) // Todos os arquivos estão vazios ou foram processados
        {
            finalizado = true;
        }
        else
        {
            // Escreve o registro com o menor ID no arquivo final
            saida.write(reinterpret_cast<char *>(&buffer[menor_indice]), sizeof(Registro));

            // Tenta ler o próximo registro do arquivo de onde o menor ID foi retirado
            arquivos[menor_indice].read(reinterpret_cast<char *>(&buffer[menor_indice]), sizeof(Registro));
            // Atualiza o status de atividade do arquivo
            ativo[menor_indice] = arquivos[menor_indice].gcount() == sizeof(Registro);
        }
    }

    saida.close();

    // Fecha todos os arquivos temporários
    for (int i = 0; i < total; ++i)
        arquivos[i].close();
}

void Manipula::adicionaNaPosicao(const char *nomeArquivo, const Registro &novoRegistro, int posicao)
{
    fstream arq(nomeArquivo, ios::in | ios::out | ios::binary);
    if (!arq.is_open())
    {
        cerr << "Erro ao abrir o arquivo para adição!" << endl;
        return;
    }

    // Descobrir tamanho total de registros
    arq.seekg(0, ios::end);
    long long tamanho_bytes = arq.tellg();
    int totalRegistros = tamanho_bytes / sizeof(Registro);

    if (posicao < 0 || posicao > totalRegistros) // Permite adicionar na última posição (totalRegistros)
    {
        cerr << "Posição inválida. Posição deve ser entre 0 e " << totalRegistros << " (inclusive)." << endl;
        arq.close();
        return;
    }

    // Se a posição é no final, simplesmente adiciona e sai
    if (posicao == totalRegistros)
    {
        arq.seekp(posicao * sizeof(Registro), ios::beg);
        arq.write(reinterpret_cast<const char *>(&novoRegistro), sizeof(Registro));
        cout << "Registro adicionado no final do arquivo." << endl;
        arq.close();
        return;
    }

    // Mover os registros existentes
    // Cria um arquivo temporário para armazenar os dados após a posição de inserção
    fstream temp_arq("temp_add.bin", ios::out | ios::binary);
    if (!temp_arq.is_open())
    {
        cerr << "Erro ao criar arquivo temporário para adição.\n";
        arq.close();
        return;
    }

    // Copia os registros a partir da posição para o arquivo temporário
    Registro reg_temp;
    arq.seekg(posicao * sizeof(Registro), ios::beg);
    while (arq.read(reinterpret_cast<char *>(&reg_temp), sizeof(Registro)))
    {
        temp_arq.write(reinterpret_cast<char *>(&reg_temp), sizeof(Registro));
    }
    temp_arq.close(); // Fecha o arquivo temporário de leitura

    // Reposiciona o ponteiro de escrita no arquivo original para a posição de inserção
    arq.seekp(posicao * sizeof(Registro), ios::beg);
    // Escreve o novo registro
    arq.write(reinterpret_cast<const char *>(&novoRegistro), sizeof(Registro));

    // Abre o arquivo temporário para leitura
    temp_arq.open("temp_add.bin", ios::in | ios::binary);
    if (!temp_arq.is_open())
    {
        cerr << "Erro ao reabrir arquivo temporário para leitura.\n";
        arq.close();
        return;
    }

    // Reescreve os registros que estavam no arquivo temporário de volta ao original
    while (temp_arq.read(reinterpret_cast<char *>(&reg_temp), sizeof(Registro)))
    {
        arq.write(reinterpret_cast<char *>(&reg_temp), sizeof(Registro));
    }

    temp_arq.close();
    remove("temp_add.bin"); // Remove o arquivo temporário

    cout << "Registro adicionado na posição " << posicao << " com sucesso." << endl;
    arq.close();
}

// Novo método para listar registros do arquivo ordenado
void Manipula::imprimeRegistrosOrdenados(const char *arquivoOrdenado)
{
    fstream arq(arquivoOrdenado, ios::in | ios::binary);
    if (!arq.is_open())
    {
        cerr << "Erro: Não foi possível abrir o arquivo ordenado (" << arquivoOrdenado << ")!" << endl;
        return;
    }
    imprimeTodosRegistros(arq); // Reutiliza a função de impressão
    arq.close();
}

int main()
{
    // O arquivo CSV é apenas para leitura
    ifstream dadosCSV("data_athlete_event.csv", ios::in);
    if (!dadosCSV.is_open())
    {
        cerr << "Erro: Não foi possível abrir o arquivo CSV!" << endl;
        return 1;
    }

    // O arquivo binário deve ser aberto para leitura e escrita, e não truncar por padrão
    // se você quiser manter os dados. Se for para começar do zero toda vez, `ios::trunc` é ok.
    // Usaremos `ios::in | ios::out | ios::binary` para permitir ambas as operações
    // e criá-lo se não existir.
    fstream dadosBIN("arquivo.bin", ios::in | ios::out | ios::binary);
    // Se o arquivo não existir na primeira execução, ios::in | ios::out não cria.
    // Para garantir a criação se não existir:
    if (!dadosBIN.is_open())
    {
        dadosBIN.open("arquivo.bin", ios::out | ios::binary);           // Tenta criar/truncar
        dadosBIN.close();                                               // Fecha para reabrir com as flags corretas
        dadosBIN.open("arquivo.bin", ios::in | ios::out | ios::binary); // Reabre para R/W
    }

    if (!dadosBIN.is_open())
    {
        cerr << "Erro: Nao foi possivel criar/abrir o arquivo binario!" << endl;
        dadosCSV.close();
        return 1;
    }

    Manipula m;

    cout << "----PROJETO PRATICO ESTRUTURA DE DADOS----" << endl;
    cout << "----Alunos: Joao Victor, Gabriel Santos e Luis Gustavo----" << endl;
    cout << "##Lista de comandos:##" << endl;
    cout << "C - para copiar os registros do arquivo CSV para o arquivo binário." << endl;
    cout << "L - para listar todos os registros do arquivo binário principal." << endl;
    cout << "O - para ordenar os registros de forma externa no arquivo binário e salvar em 'arquivo_ordenado.bin'." << endl;
    cout << "LO - para Listar os registros do arquivo ordenado 'arquivo_ordenado.bin'." << endl;
    cout << "A <posicao> - para adicionar um registro em determinada posicao (0-base)." << endl;
    cout << "F - finalizar o programa" << endl;

    string comando_str; // Usar std::string para comandos como "LO" e "A"

    do
    {
        cout << "\nO que deseja fazer? ";
        cin >> comando_str;

        if (comando_str == "C")
        {
            m.transfereRegistros(dadosBIN, dadosCSV);
            cout << "Registros copiados com sucesso para 'arquivo.bin'!" << endl;
        }
        else if (comando_str == "L")
        {
            m.imprimeTodosRegistros(dadosBIN);
        }
        else if (comando_str == "O")
        {
            m.ordenaExternaPorID("arquivo.bin"); // Passa o nome do arquivo a ser ordenado
        }
        else if (comando_str == "LO")
        {
            m.imprimeRegistrosOrdenados("arquivo_ordenado.bin"); // Lista do arquivo ordenado
        }
        else if (comando_str == "A")
        {
            int posicao;
            Registro novoReg; // Registro para adicionar

            cout << "Digite a posicao para adicionar (0-base): ";
            cin >> posicao;

            // Solicitar dados para o novo registro
            cout << "Digite o ID do novo registro: ";
            cin >> novoReg.id;
            cin.ignore(); // Limpar o buffer do newline após cin >> id

            cout << "Digite o Nome: ";
            cin.getline(novoReg.name, sizeof(novoReg.name));

            cout << "Digite o Time: ";
            cin.getline(novoReg.team, sizeof(novoReg.team));

            cout << "Digite os Games: ";
            cin.getline(novoReg.games, sizeof(novoReg.games));

            cout << "Digite o Ano: ";
            cin.getline(novoReg.year, sizeof(novoReg.year));

            cout << "Digite a Temporada: ";
            cin.getline(novoReg.season, sizeof(novoReg.season));

            m.adicionaNaPosicao("arquivo.bin", novoReg, posicao);
        }
        else if (comando_str == "F")
        {
            cout << "Finalizando o programa..." << endl;
        }
        else
        {
            cout << "Comando inválido. Por favor, digite C, L, O, LO, A ou F." << endl;
        }

    } while (comando_str != "F");

    // É crucial fechar os arquivos no final
    dadosCSV.close();
    dadosBIN.close();

    return 0;
}

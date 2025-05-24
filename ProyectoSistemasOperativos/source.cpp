#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <fstream>

using namespace std;

struct Program
{
    string programa;
    int rafagas;
    int llegada;
};

// Output formatting
ostream& operator<<(ostream& os, const Program& p)
{
    os << "Programa: " << p.programa << ", Rafagas: " << p.rafagas << ", Llegada: " << p.llegada;
    return os;
}

// Sort by arrival time (llegada)
struct CompareByPriority {
    bool operator()(const Program& a, const Program& b) const {
        return a.llegada < b.llegada;
    }
};

// Input function
Program PrgCreate()
{
    system("cls");
    Program temp;
    cout << "Menciona el nombre del programa: ";
    cin >> temp.programa;
    cout << "Cual es el numero de rafagas necesarias para el proyecto: ";
    cin >> temp.rafagas;
    cout << "Cual es la llegada del programa: ";
    cin >> temp.llegada;
    return temp;
}

// Main algorithm
void FIFO()
{
    queue<Program> programQueue;
    int input = 1;

    do
    {
        programQueue.push(PrgCreate());
        cout << "Quieres agregar otro programa? \n1. Si \n0. No" << endl;
        cin >> input;
    } while (input != 0);

    // Sort by arrival
    set<Program, CompareByPriority> sortedSet;
    while (!programQueue.empty())
    {
        sortedSet.insert(programQueue.front());
        programQueue.pop();
    }

    // Display sorted programs
    for (const Program& element : sortedSet)
    {
        programQueue.push(element);
    }

    queue<Program> current;
    int counter = 0;
    cout << programQueue.empty() << endl;
    bool completed = false;
    while (!completed)
    {
        if (!programQueue.empty())
        {
            if (programQueue.front().llegada == counter)
            {
                current.push(programQueue.front());
                programQueue.pop();
            }
        }
        if (!current.empty())
        {
            current.front().rafagas--;
            if (current.front().rafagas == 0) {
                cout << current.front().programa << endl;
                current.pop();
            }
        }
        if (programQueue.empty() && current.empty())
            completed = true;
        counter++;
    }
}

void RR()
{
    queue<Program> programQueue;
    int input = 1;
    const int quantum = 2;

    // Collect programs
    do
    {
        programQueue.push(PrgCreate());
        cout << "Quieres agregar otro programa? \n1. Si \n0. No" << endl;
        cin >> input;
    } while (input != 0);

    // Sort by arrival
    set<Program, CompareByPriority> sortedSet;
    while (!programQueue.empty())
    {
        sortedSet.insert(programQueue.front());
        programQueue.pop();
    }

    for (const Program& element : sortedSet)
    {
        programQueue.push(element);
    }

    deque<Program> readyQueue;
    int counter = 0;
    bool completed = false;

    while (!completed)
    {
        // Move newly arrived programs to the ready queue
        while (!programQueue.empty() && programQueue.front().llegada <= counter)
        {
            readyQueue.push_back(programQueue.front());
            programQueue.pop();
        }

        // Execute the front of the ready queue
        if (!readyQueue.empty())
        {
            Program current = readyQueue.front();
            readyQueue.pop_front();

            int executed = min(quantum, current.rafagas);
            for (int i = 0; i < executed; ++i)
            {
                // Simulate one unit of execution
                current.rafagas--;
                counter++;

                // During execution, check for new arrivals
                while (!programQueue.empty() && programQueue.front().llegada <= counter)
                {
                    readyQueue.push_back(programQueue.front());
                    programQueue.pop();
                }
            }

            // If program not finished, put it back in the queue
            if (current.rafagas > 0)
            {
                readyQueue.push_back(current);
            }
            else
            {
                cout << "Terminado: " << current.programa << " en tiempo " << counter << endl;
            }
        }
        else
        {
            // If no one is ready, time passes
            counter++;
        }

        // End condition
        if (programQueue.empty() && readyQueue.empty())
            completed = true;
    }
}
struct info {
    string nombre;
    string dueño;
    int tam;
    string permisos;
};
/*
void crear_archivo() {
    string nombre;
    cout << "Nombre del archivo: ";
    cin >> nombre;

    ofstream archivo(nombre); // crea el archivo
    if (archivo) 
    {
        cout << "Archivo creado: " << nombre << endl;
        archivo.close(); //cierrra el archivo y libera recursos 
    }
    else 
    {
        cout << "No se pudo crear el archivo \n";
    }
}

void leer_archivo()
{
    string nombre, linea;

    cout << "Nombre del archivo: ";
    cin >> nombre;

    ifstream archivo(nombre);
    if (archivo) 
    {
        cout << "Contenido del archivo:\n";

        while (getline(archivo, linea)) 
        {
            cout << linea << endl;
        }

        archivo.close();
    }
    else 
    {
        cout << "No se pudo abrir el archivo.\n";
    }
}

void escribir_Archivo() 
{
    string nombre, texto;

    cout << "Nombre del archivo: ";
    cin >> nombre;

    cout << "Escribe el texto: ";
    cin.ignore(); 
    getline(cin, texto);

    ofstream archivo(nombre, ios::app); // agregar
    if (archivo) 
    {
        archivo << texto << endl;
        cout << "se guardo el texto \n";
        archivo.close();
    }
    else {
        cout << "No se pudo escribir en el archivo.\n";
    }
}
*/
void crear_archivo() {
    string nombre;
    cout << "Nombre del archivo: ";
    cin >> nombre;

    int fd = open(nombre.c_str(), O_CREAT | O_WRONLY, 0644); // crea archivo
    if (fd != -1) {
        cout << "Archivo creado: " << nombre << endl;
        close(fd); // cierra el archivo
    }
    else {
        cout << "No se pudo crear el archivo\n";
    }
}

void escribir_archivo() {
    string nombre, texto;
    cout << "Nombre del archivo: ";
    cin >> nombre;
    cin.ignore(); // limpia buffer

    cout << "Escribe el texto: ";
    getline(cin, texto);

    int fd = open(nombre.c_str(), O_WRONLY | O_APPEND);
    if (fd != -1) {
        write(fd, texto.c_str(), texto.length());
        write(fd, "\n", 1);
        cout << "Texto guardado.\n";
        close(fd);
    }
    else {
        cout << "No se pudo escribir en el archivo\n";
    }
}

void leer_archivo() {
    string nombre;
    cout << "Nombre del archivo: ";
    cin >> nombre;

    int fd = open(nombre.c_str(), O_RDONLY);
    if (fd != -1) {
        char buffer[1024];
        int bytes;
        cout << "Contenido del archivo:\n";
        while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes] = '\0';
            cout << buffer;
        }
        close(fd);
    }
    else {
        cout << "No se pudo abrir el archivo\n";
    }
}
int main()
{
    RR();
    return 0;
}

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <list>
#include <set>

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
            completed;
        counter++;
    }
}

int main()
{
    FIFO();
    return 0;
}

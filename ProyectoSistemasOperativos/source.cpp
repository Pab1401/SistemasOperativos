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


int main()
{
    RR();
    return 0;
}

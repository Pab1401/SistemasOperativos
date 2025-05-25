#include <iostream> //Entrada/salida de consola
#include <fstream>  
#include <queue>    //Cola FIFO (queue)
#include <deque>    
#include <set>  //Conjuntos ordenados
#include <string>   //Uso de strings para validar varios caracteres
#include <unordered_map>
#include <vector>
#include <sys/types.h>   // Para pid_t
#include <unistd.h>      // Para fork(), exec()
#include <signal.h>      // Para kill() y señales (SIGSTOP, SIGCONT, etc)
#include <sys/wait.h>    // Para wait() 


using namespace std;

// ========== ESTRUCTURA DE USUARIO ==========
struct Usuario {
    string nombre;
    string tipo; // "root" o "user"
};

// ========== ESTRUCTURA DE PROCESOS ==========
//Nombre del programa,Cantidad de ciclos que se necesitan ejecutarse,En que momento llega al sistema
struct Program {
    //string programa; 
    //int rafagas; 
    //int llegada;    

    string programa;
    int rafagas = 0;
    int llegada = 0;

    Program() = default;

    Program(string p, int r, int l) : programa(p), rafagas(r), llegada(l) {}
};

//Sobrecarga del operador
ostream& operator<<(ostream& os, const Program& p) { // es el objeto Program que queremos imprimir
    os << "Programa: " << p.programa << ", Rafagas: " << p.rafagas << ", Llegada: " << p.llegada;
    return os; //se retorna "os" para encadenar impresiones
}
//Ejemplo: de retorno-> Programa: Paint, Rafagas: 4, Llegada: 2

//Define como se ordenan los programas en un set, se ordenan por el tiempo de llegada! de menor a mayor.
struct CompareByPriority {
    bool operator()(const Program& a, const Program& b) const {
        return a.llegada < b.llegada;
    }
};

//Se piden los datos
Program PrgCreate() {
    Program temp;
    cout << "Nombre del programa: ";
    cin >> temp.programa;
    cout << "Numero de rafagas necesarias: ";
    cin >> temp.rafagas;
    cout << "Tiempo de llegada: ";
    cin >> temp.llegada;
    return temp;
}

//Se agregan los procesos en una cola, según el proceso seleccionado 
void FIFO() {
    queue<Program> programQueue;
    int input = 1;

    do {
        programQueue.push(PrgCreate());
        cout << "¿Deseas agregar otro programa? (1=Sí, 0=No): ";
        cin >> input;
    } while (input != 0);

    //Los procesos primero se ordenan por orden de llegada, luego se reinsertan en una nueva cola donde estaran los procesos en orden creciente de tiempo de llegada.
    set<Program, CompareByPriority> sortedSet;
    while (!programQueue.empty()) {
        sortedSet.insert(programQueue.front());
        programQueue.pop();
    }

    for (const Program& p : sortedSet) {
        programQueue.push(p);
    }

    //Cada ciclo while representa una unidad de tiempo; cuando cel tiempo (counter) coincide con la llegada del proceso se mueve a la cola current
    queue<Program> current;
    int counter = 0;
    bool completed = false;

    while (!completed) {
        if (!programQueue.empty() && programQueue.front().llegada == counter) {
            current.push(programQueue.front());
            programQueue.pop();
        }

        if (!current.empty()) {
            current.front().rafagas--; //se reduce la cantidad de rafagas hasta llegar a cero
            if (current.front().rafagas == 0) {
                cout << "Finalizado: " << current.front().programa << endl;
                current.pop();
            }
            //se muestra cuado se finaliza el proceso
        }

        if (programQueue.empty() && current.empty())
            completed = true;

        counter++;
    }
}

//RR ejecuta un quantum de tiempo por proceso(máximo 2 unidades).Si el proceso no termina, vuelve al final de la cola. Si termina, se notifica con cout.
void RR() {
    queue<Program> programQueue;
    int input = 1;
    const int quantum = 2;

    do {
        programQueue.push(PrgCreate());
        cout << "¿Agregar otro programa? (1=Si, 0=No): ";
        cin >> input;
    } while (input != 0);

    set<Program, CompareByPriority> sortedSet;
    while (!programQueue.empty()) {
        sortedSet.insert(programQueue.front());
        programQueue.pop();
    }

    for (const Program& p : sortedSet) {
        programQueue.push(p);
    }

    deque<Program> readyQueue;
    int counter = 0;
    bool completed = false;

    while (!completed) {
        while (!programQueue.empty() && programQueue.front().llegada <= counter) {
            readyQueue.push_back(programQueue.front());
            programQueue.pop();
        }

        if (!readyQueue.empty()) {
            Program current = readyQueue.front();
            readyQueue.pop_front();

            int executed = min(quantum, current.rafagas);
            for (int i = 0; i < executed; ++i) {
                current.rafagas--;
                counter++;

                while (!programQueue.empty() && programQueue.front().llegada <= counter) {
                    readyQueue.push_back(programQueue.front());
                    programQueue.pop();
                }
            }

            if (current.rafagas > 0) {
                readyQueue.push_back(current);
            }
            else {
                cout << "Finalizado: " << current.programa << " en tiempo " << counter << endl;
            }
        }
        else {
            counter++;
        }

        if (programQueue.empty() && readyQueue.empty())
            completed = true;
    }
}

// ========== SISTEMA DE ARCHIVOS ==========
//Crea un archivo con permisos y propietario
class Archivo {
private:
    string nombre;
    string propietario;
    string permisos; // r (permiso de lectura), w(permisos de escritura), rw (permisos de lectura y escritura combinados)

public:
    Archivo(string _nombre, string _propietario, string _permisos)
        : nombre(_nombre), propietario(_propietario), permisos(_permisos) {
    }

    void crear() {
        ofstream archivo(nombre);
        if (archivo.is_open()) {
            archivo << "META:" << propietario << ":" << permisos << "\n";
            archivo.close();
            cout << "Archivo creado: " << nombre << endl;
        }
    }

    void escribir(string usuario, string contenido) {
        if (usuario == propietario || permisos.find('w') != string::npos || usuario == "root") {
            ofstream archivo(nombre, ios::app);
            archivo << usuario << ": " << contenido << "\n";
            archivo.close();
            cout << "Escritura exitosa.\n";
        }
        else {
            cout << "Permiso denegado para escribir en el archivo.\n";
        }
    }

    void leer(string usuario) {
        if (usuario == propietario || permisos.find('r') != string::npos || usuario == "root") {
            ifstream archivo(nombre);
            string linea;
            while (getline(archivo, linea)) {
                cout << linea << endl;
            }
            archivo.close();
        }
        else {
            cout << "Permiso denegado para leer el archivo.\n";
        }
    }

    void cerrar() {
        cout << "Archivo cerrado (simulado).\n";
    }
};

unordered_map<string, Archivo*> sistemaArchivos;

// Pide nombre, permisos y lo agrega a "unordered_map" llamado "sistemaArchivos"
void crearArchivo(string usuario) {
    string nombre, permisos;
    cout << "Nombre del archivo: ";
    cin >> nombre;
    cout << "Permisos (r, w, rw): ";
    cin >> permisos;

    Archivo* nuevo = new Archivo(nombre, usuario, permisos);
    nuevo->crear();
    sistemaArchivos[nombre] = nuevo;
}

//Cada opción verifica si el archivo existe y si el usuario tiene permisos adecuados.
void menuArchivos(string usuario) {
    int opcion;
    string nombre, contenido;

    do {
        cout << "\n--- MENÚ ARCHIVOS ---\n";
        cout << "1. Crear archivo\n2. Leer archivo\n3. Escribir archivo\n4. Cerrar archivo\n5. Volver\n";
        cin >> opcion;

        switch (opcion) {
        case 1:
            crearArchivo(usuario);
            break;
        case 2:
            cout << "Nombre del archivo a leer: ";
            cin >> nombre;
            if (sistemaArchivos.count(nombre)) {
                sistemaArchivos[nombre]->leer(usuario);
            }
            else {
                cout << "Archivo no existe.\n";
            }
            break;
        case 3:
            cout << "Nombre del archivo a escribir: ";
            cin >> nombre;
            cin.ignore();
            cout << "Contenido: ";
            getline(cin, contenido);
            if (sistemaArchivos.count(nombre)) {
                sistemaArchivos[nombre]->escribir(usuario, contenido);
            }
            else {
                cout << "Archivo no existe.\n";
            }
            break;
        case 4:
            cout << "Nombre del archivo a cerrar: ";
            cin >> nombre;
            if (sistemaArchivos.count(nombre)) {
                sistemaArchivos[nombre]->cerrar();
            }
            else {
                cout << "Archivo no existe.\n";
            }
            break;
        }
    } while (opcion != 5);
}




// pproceso real
struct Proceso
{
    pid_t pid;
    string nombre;
    string estado;
};

vector<Proceso> procesos;

void crearProceso()
{
    string nombre;
    cout << "Nombre simbólico del proceso: ";
    cin >> nombre;

    pid_t pid = fork();

    if (pid == 0) {
        // Hijo
        cout << "Soy el hijo con PID: " << getpid() << endl;
        // Espera un comando del padre
        pause(); // Detiene al hijo hasta que reciba una señal
        exit(0);
    }
    else if (pid > 0)
    {
        // Padre
        Proceso nuevo{ pid, nombre, "suspended" };
        procesos.push_back(nuevo);
        cout << "Proceso creado con PID: " << pid << " y suspendido." << endl;
        kill(pid, SIGSTOP); // Suspende de inmediato
    }
    else
    {
        cerr << "Error al crear el proceso." << endl;
    }
}

void ejecutarProceso()
{
    int index;
    cout << "Selecciona el índice del proceso a ejecutar: ";
    cin >> index;

    if (index >= 0 && index < procesos.size()) {
        pid_t pid = fork();
        if (pid == 0) {
            execlp("ls", "ls", "-l", NULL); // Reemplaza por otro si quieres
            perror("Error al ejecutar exec");
            exit(1);
        }
        else {
            cout << "Ejecutando comando en hijo con PID: " << pid << endl;
            waitpid(pid, NULL, 0);
        }
    }
    else {
        cout << "Índice inválido." << endl;
    }
}

void esperarProcesos() {
    for (auto& p : procesos)
    {
        int status;
        waitpid(p.pid, &status, WNOHANG);
        if (WIFEXITED(status))
        {
            p.estado = "terminated";
            cout << "Proceso " << p.nombre << " ha terminado." << endl;
        }
    }
}

void suspenderProceso()
{
    int index;
    cout << "Índice del proceso a suspender: ";
    cin >> index;
    if (index >= 0 && index < procesos.size())
    {
        kill(procesos[index].pid, SIGSTOP);
        procesos[index].estado = "suspended";
        cout << "Proceso suspendido." << endl;
    }
    else {
        cout << "Índice inválido." << endl;
    }
}

void reanudarProceso()
{
    int index;
    cout << "Índice del proceso a reanudar: ";
    cin >> index;
    if (index >= 0 && index < procesos.size())
    {
        kill(procesos[index].pid, SIGCONT);
        procesos[index].estado = "running";
        cout << "Proceso reanudado." << endl;
    }
    else {
        cout << "Índice inválido." << endl;
    }
}

void terminarProceso()
{
    int index;
    cout << "Índice del proceso a terminar: ";
    cin >> index;
    if (index >= 0 && index < procesos.size())
    {
        kill(procesos[index].pid, SIGKILL);
        procesos[index].estado = "terminated";
        cout << "Proceso terminado." << endl;
    }
    else {
        cout << "Índice inválido." << endl;
    }
}

void mostrarProcesos()
{
    cout << "\nLista de procesos:\n";
    for (int i = 0; i < procesos.size(); i++) {
        cout << i << ". PID: " << procesos[i].pid
            << ", Nombre: " << procesos[i].nombre
            << ", Estado: " << procesos[i].estado << endl;
    }
}

void menuProcesosReales()
{
    int opcion;
    do {
        cout << "\n--- Gestión de Procesos Reales ---\n";
        cout << "1. Crear proceso\n";
        cout << "2. Ejecutar comando (ls -l)\n";
        cout << "3. Esperar procesos\n";
        cout << "4. Suspender proceso\n";
        cout << "5. Reanudar proceso\n";
        cout << "6. Terminar proceso\n";
        cout << "7. Mostrar procesos\n";
        cout << "0. Salir al menú principal\n";
        cout << "Opción: ";
        cin >> opcion;

        switch (opcion)
        {
        case 1: crearProceso(); break;
        case 2: ejecutarProceso(); break;
        case 3: esperarProcesos(); break;
        case 4: suspenderProceso(); break;
        case 5: reanudarProceso(); break;
        case 6: terminarProceso(); break;
        case 7: mostrarProcesos(); break;
        }

    } while (opcion != 0);
}


// ========== LOGIN Y MENÚ PRINCIPAL ==========
//root o user, controla permisos en archivos
Usuario login() {
    Usuario u;
    cout << "---- LOGIN ----\n";
    cout << "Usuario: ";
    cin >> u.nombre;
    cout << "Tipo (root/user): ";
    cin >> u.tipo;
    return u;
}

//Menu principal del sistema
int main() {
    Usuario actual = login();
    int opcion;

    do {
        cout << "\n=== SISTEMA OPERATIVO SIMULADO ===\n";
        cout << "1. FIFO\n2. Round Robin\n3. Sistema de Archivos\n4. gestion de procesos reales\n5. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1:
            FIFO();
            break;
        case 2:
            RR();
            break;
        case 3:
            menuArchivos(actual.nombre);
            break;
        case 4:
            menuProcesosReales();
            break;
        case 5:
            cout << "Saliendo del sistema...\n";
            break;
        default:
            cout << "Opcion invalida.\n";
        }

    } while (opcion != 4);

    return 0;
}
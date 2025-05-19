#include <iostream>
#include <queue>
#include <vector>
#include <string>

using namespace std;

struct Program
{
	string programa;
	int rafagas;
	int llegada;
};

int main()
{

}

void FCFS()
{
	queue<Program> programList;
	int input = 1;
	do
	{
		programList.push(PrgCreate());
		cout << "Quieres agregar otro programa? \n1. Si \n0. No" << endl;
		cin >> input;
	} while (input != 0);
}

Program PrgCreate()
{
	system("cls");
	Program *temp = new Program;
	cout << "Menciona el nombre del programa: ";
	cin >> temp->programa;
	cout << "Cual es el numero de rafagas necesarias para el proyecto: ";
	cin >> temp->rafagas;
	cout << "Cual es la llegada del programa: ";
	cin >> temp->llegada;
	return *temp;
}


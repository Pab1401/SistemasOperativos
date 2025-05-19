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

void FIFO()
{
	queue<Program> programList;
	int input = 1;
	bool completed = false;
	do
	{
		programList.push(PrgCreate());
		cout << "Quieres agregar otro programa? \n1. Si \n0. No" << endl;
		cin >> input;
	} while (input != 0);
	int counter;
	while (!completed)
	{
		Program current;
		if (programList.front().llegada =< counter)
		current = programList.front();
		do
		{
			current.rafagas--;
		} while (current.rafagas > 0);
		programList.pop();
		if (programList.empty())
			completed = true;
	}
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


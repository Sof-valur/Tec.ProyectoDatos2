#include <future>
#include <iostream>  
#include "DemoServer.h" 
#include "json.cpp"
#include "Source.cpp"
#include "utils.cpp"
#include "Tarjeta.cpp"
using namespace nlohmann;
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
 

int x_size = 4, y_size = 4;
Tarjeta** tablero;

struct CustomCommand {
	std::string command;
	std::string btn;
};


CustomCommand parseCommand(std::string s) {

	CustomCommand cmd = {
	};
	std::string delim = "|";

	auto start = 0U;
	auto end = s.find(delim);

	cmd.command = s.substr(start, end - start);

	start = end + delim.length();
	end = s.find(delim, start);

	cmd.btn = s.substr(start, end - start);
	return cmd;
}

std::string commandToString(CustomCommand* cmd) {
	std::string result = cmd->command + "|" + cmd->btn;
	return result;
}

char* getSendMessage(char* input) { 
	std::string str;

	str.assign(input);

	CustomCommand cmd = parseCommand(str);
	if (cmd.command == "Click")
	{

		std::string delim = "x";

		auto start = 0U;
		auto end = cmd.btn.find(delim);

		std::string col = cmd.btn.substr(start, end - start);

		start = end + delim.length();
		end = cmd.btn.find(delim, start);
		
		std::string fil = cmd.btn.substr(start, end - start);

		int colInt = std::atoi(col.c_str());
		int filInt = std::atoi(fil.c_str());

		Tarjeta tarjeta = tablero[filInt][colInt];
		char* result = _strdup(tarjeta.imagen.c_str());
		return result;
	}
	return "sup";
} 

void Insert_into_2D_Array(Tarjeta** foo, Tarjeta insert,  int x_pos, int y_pos)
{
	if (x_pos < x_size && y_pos < y_size) {
		foo[x_pos][y_pos] = insert;    // insert_value lost post func exit?
	}
}

void Init_2D_Array(Tarjeta** foo, int x_size, int y_size)
{ 
	    // new alloc mem lost post func exit ?
	for (int i = 0;i < x_size;i++)
	{
		foo[i] = new Tarjeta[y_size];     // new alloc mem lost post func exit
	}
}


void initTablero(Tarjeta** tablero) {
	int fila = 0;
	int col = 0;

	int fox = 0, shiba = 0, duck = 0, dino = 0, drago = 0, fish = 0, gato = 0, rana = 0;
	while (fila != y_size)
	{
		int carta = 1 + (rand() % 8);
		string tipoDeCarta = "";
		switch (carta)
		{
		case 1:
			if (fox < 2)
			{
				fox++;
				tipoDeCarta = "fox";
			}
			break;
		case 2:
			if (dino < 2)
			{
				dino++;
				tipoDeCarta = "dino";
			}
			break;
		case 3:
			if (gato < 2)
			{
				gato++;
				tipoDeCarta = "gato";
			}
			break;
		case 4:
			if (rana < 2)
			{
				rana++;
				tipoDeCarta = "rana";
			}
			break;
		case 5:
			if (shiba < 2)
			{
				shiba++;
				tipoDeCarta = "shiba";
			}
			break;
		case 6:
			if (duck < 2)
			{
				duck++;
				tipoDeCarta = "duck";
			}
			break;
		case 7:
			if (drago < 2)
			{
				drago++;
				tipoDeCarta = "drago";
			}
			break;
		case 8:
			if (fish < 2)
			{
				fish++;
				tipoDeCarta = "fish";
			}
			break;
		}

		if (tipoDeCarta != "")
		{
			Tarjeta* t = new Tarjeta();
			t->imagen = tipoDeCarta;
			t->fila = fila;
			t->columna = col;
			Tarjeta tt = *t;
			Insert_into_2D_Array(tablero, tt, fila, col);
			
			if (col < x_size - 1 )
			{
				col++;
			}
			else if (fila < y_size - 1)
			{
				fila++;
				col = 0;
			}
			else if (fila == y_size - 1 && col == x_size - 1)
			{
				fila++; 
			}
		} 
	}
}

int main()
{
	tablero = new Tarjeta * [x_size];;
	Init_2D_Array(tablero, x_size, y_size); 
	initTablero(tablero);

	WSADATA wsa_data;
	SOCKADDR_IN server_addr, client_addr;

	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	const auto server = socket(AF_INET, SOCK_STREAM, 0);

	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5555);

	::bind(server, reinterpret_cast<SOCKADDR *>(&server_addr), sizeof(server_addr));
	listen(server, 0);

	cout << "Listening for incoming connections..." << endl;

	int client_addr_size = sizeof(client_addr);

	for (;;)
	{
		SOCKET client;

		if ((client = accept(server, reinterpret_cast<SOCKADDR *>(&client_addr), &client_addr_size)) != INVALID_SOCKET)
		{
			auto fut = async(launch::async, on_client_connect, client);
		}

		const auto last_error = WSAGetLastError();
		
		if(last_error > 0)
		{
			cout << "Error: " << last_error << endl;
		}
	}
}

void on_client_connect(SOCKET client)
{
	char buffer[1024];

	cout << "Client connected!" << endl;
	recv(client, buffer, sizeof(buffer), 0);
	char* msj = getSendMessage(buffer);
	send(client, msj, sizeof(msj), 0);
	cout << "Client says: " << buffer << endl;
	memset(buffer, 0, sizeof(buffer));

	closesocket(client);
	cout << "Client disconnected." << endl;
}


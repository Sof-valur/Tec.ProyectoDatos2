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
 

struct CustomCommand {
	std::string command;
	std::string btn;
};


CustomCommand* parseCommand(std::string s) {

	CustomCommand cmd = {
	};
	std::string delim = "|";

	auto start = 0U;
	auto end = s.find(delim);

	cmd.command = s.substr(start, end - start);

	start = end + delim.length();
	end = s.find(delim, start);

	cmd.btn = s.substr(start, end - start);
	return &cmd;
}

std::string commandToString(CustomCommand* cmd) {
	std::string result = cmd->command + "|" + cmd->btn;
	return result;
}

char* getSendMessage(char* input) { 
	std::string str;

	str.assign(input);

	auto cmd = parseCommand(str);

	return "sup";
}

void initTablero(const Tarjeta tablero[4][4]) {
	tablero = {};


	 
}

int main()
{
	Tarjeta tablero[4][4];
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


#pragma once 
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>  
#include <vcclr.h>
#include <string>   
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


namespace CppCLRWinformsProjekt {
	  
	using namespace System;
	using namespace System::IO;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Collections::Specialized;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;


	struct CustomCommand {
		std::string command;
		std::string btn;
	};


	CustomCommand* parseCommand(std::string s) {

		CustomCommand* cmd = {
		};
		std::string delim = "|";

		auto start = 0U;
		auto end = s.find(delim);

		cmd->command = s.substr(start, end - start);

		start = end + delim.length();
		end = s.find(delim, start);

		cmd->btn = s.substr(start, end - start);
		return cmd;
	}

	std::string commandToString(CustomCommand* cmd) {
		std::string result = cmd->command + "|" + cmd->btn;
		return result;
	}

	/// <summary>
	/// Zusammenfassung für Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		int filas = 4;
		private: System::Windows::Forms::RichTextBox^ rtb_resumen;
		private: System::Windows::Forms::TextBox^ txtUsuario1;
		private: System::Windows::Forms::TextBox^ txtUsuario2;
		private: System::Windows::Forms::Label^ lblUsuario1;
		private: System::Windows::Forms::Label^ lblUsuario2;
		private: System::Windows::Forms::Panel^ pnl_Usuarios;
		private: System::Windows::Forms::Button^ btnStart;
		private: System::Windows::Forms::Panel^ pnl_game;
		private: System::Windows::Forms::PictureBox^ turno_j2;
		private: System::Windows::Forms::PictureBox^ turno_j1;
		private: System::Windows::Forms::Label^ lbl_pts_j2;
		private: System::Windows::Forms::Label^ lbl_pts_j1;
		private: System::Windows::Forms::Label^ label2;
		private: System::Windows::Forms::Label^ label1;
		int columnas = 4;
		int pts_1 = -1;
		int pts_2 = -1;

	public:
		Form1(void)
		{  
			InitializeComponent();
			this->pnl_game->Enabled = false;
			int columnasAncho = 1 * columnas / 100;
			int filasAncho = 1 * filas / 100;
			this->tablero = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->tablero->ColumnCount = columnas;
			this->tablero->RowCount = filas;
			this->tablero->Location = System::Drawing::Point(13, 13);
			this->tablero->Name = L"tablero";
			this->tablero->Size = System::Drawing::Size(516, 342);
			this->tablero->TabIndex = 0;
			for (int i = 0; i < columnas; i++)
			{
				this->tablero->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, columnasAncho)));
			}

			for (int i = 0; i < filas; i++)
			{
				this->tablero->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, filasAncho)));
			}

			for (int col = 0; col < columnas; col++)
			{
				for (int fil = 0; fil < filas; fil++)
				{
					this->tablero->Controls->Add(createButton(col, fil), col, fil);
				}
			}

			this->Controls->Add(this->tablero);
			this->tablero->ResumeLayout(false);
			this->tablero->BringToFront();
			for each (Control^ cntrl in this->tablero->Controls)
			{
				cntrl->Hide();
			}
			this->pnl_game->Hide();
			this->tablero->Show();
		}

	private: System::Windows::Forms::Button^ createButton(int columna, int fila) {
		System::Windows::Forms::Button^ result = (gcnew System::Windows::Forms::Button());
		result->Location = System::Drawing::Point(261, 3);
		result->Name = columna + L"x" + fila;
		result->Size = System::Drawing::Size(50, 50);
		result->TabIndex = 0;
		result->Text = columna + L"x" + fila;
		result->UseVisualStyleBackColor = true;
		result->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
		return result;
	}
	System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) { 
		System::Windows::Forms::Button^ btn = ((System::Windows::Forms::Button^)sender);
		char* str1 = (char*)(void*)Marshal::StringToHGlobalAnsi(btn->Text);
		CustomCommand cmd = {};
		cmd.command  = "Click";
		cmd.btn = str1; 
		std::string mS = commandToString(&cmd);
		main2(mS);
	} 
	
	#define DEFAULT_BUFLEN 512
	#define DEFAULT_PORT "5555"
	int main2(std::string sendbuf)
	{
		WSADATA wsaData;
		SOCKET ConnectSocket = INVALID_SOCKET;
		struct addrinfo* result = NULL,
			* ptr = NULL,
			hints; 
		char recvbuf[DEFAULT_BUFLEN];
		int iResult;
		int recvbuflen = DEFAULT_BUFLEN;
		 

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			agregarMensaje("WSAStartup failed with error: %d\n", iResult);
			return 1;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
		if (iResult != 0) {
			agregarMensaje("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return 1;
		}

		// Attempt to connect to an address until one succeeds
		for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) {
				agregarMensaje("socket failed with error: %ld\n", WSAGetLastError());
				WSACleanup();
				return 1;
			}

			// Connect to server.
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			agregarMensaje("Unable to connect to server!");
			WSACleanup();
			return 1;
		}

		// Send an initial buffer
		iResult = send(ConnectSocket, sendbuf.c_str(), (int)strlen(sendbuf.c_str()), 0);
		if (iResult == SOCKET_ERROR) {
			agregarMensaje("send failed with error: ", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		agregarMensaje("Bytes Sent:", iResult);
		agregarMensaje("Msj Sent: ", sendbuf.c_str());

		// shutdown the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			agregarMensaje("shutdown failed with error: ", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// Receive until the peer closes the connection
		do {

			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0) { 
				agregarMensaje("Bytes received:", iResult);
				agregarMensaje("Message from Server: ", recvbuf);
			}
			else if (iResult == 0)
				agregarMensaje("Connection closed\n");
			else
				agregarMensaje("recv failed with error: ", WSAGetLastError());

		} while (iResult > 0);

		// cleanup
		closesocket(ConnectSocket);
		WSACleanup();

		return 0;
	}


	void agregarMensaje(const char* msj) {
		this->rtb_resumen->Text += "\n";
		this->rtb_resumen->Text += gcnew String(msj) ;
		this->rtb_resumen->Select(this->rtb_resumen->Text->Length - 1, 0);
		this->rtb_resumen->Refresh();
	}
	void agregarMensaje(const char* msj, int extra ) {
		std::string s = std::to_string(extra);
		char const* pchar = s.c_str();
		this->rtb_resumen->Text += "\n";
		this->rtb_resumen->Text += gcnew String(msj) + gcnew String(pchar);
		this->rtb_resumen->Select(this->rtb_resumen->Text->Length - 1, 0);
		this->rtb_resumen->Refresh();
	}
	void agregarMensaje(const char* msj, const char* extra) {
		this->rtb_resumen->Text += "\n";
		this->rtb_resumen->Text += gcnew String(msj) + gcnew String(extra);
		this->rtb_resumen->Select(this->rtb_resumen->Text->Length - 1, 0);
		this->rtb_resumen->Refresh();
	}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TableLayoutPanel^ tablero;
	protected:

	protected:

	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tablero = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->rtb_resumen = (gcnew System::Windows::Forms::RichTextBox());
			this->txtUsuario1 = (gcnew System::Windows::Forms::TextBox());
			this->txtUsuario2 = (gcnew System::Windows::Forms::TextBox());
			this->lblUsuario1 = (gcnew System::Windows::Forms::Label());
			this->lblUsuario2 = (gcnew System::Windows::Forms::Label());
			this->pnl_Usuarios = (gcnew System::Windows::Forms::Panel());
			this->turno_j2 = (gcnew System::Windows::Forms::PictureBox());
			this->turno_j1 = (gcnew System::Windows::Forms::PictureBox());
			this->lbl_pts_j2 = (gcnew System::Windows::Forms::Label());
			this->lbl_pts_j1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btnStart = (gcnew System::Windows::Forms::Button());
			this->pnl_game = (gcnew System::Windows::Forms::Panel());
			this->pnl_Usuarios->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->turno_j2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->turno_j1))->BeginInit();
			this->pnl_game->SuspendLayout();
			this->SuspendLayout();
			// 
			// tablero
			// 
			this->tablero->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Absolute, 538)));
			this->tablero->Location = System::Drawing::Point(6, 3);
			this->tablero->Name = L"tablero";
			this->tablero->Size = System::Drawing::Size(538, 360);
			this->tablero->TabIndex = 0;
			// 
			// rtb_resumen
			// 
			this->rtb_resumen->Location = System::Drawing::Point(2, 392);
			this->rtb_resumen->Name = L"rtb_resumen";
			this->rtb_resumen->ReadOnly = true;
			this->rtb_resumen->Size = System::Drawing::Size(349, 85);
			this->rtb_resumen->TabIndex = 1;
			this->rtb_resumen->Text = L"";
			// 
			// txtUsuario1
			// 
			this->txtUsuario1->Location = System::Drawing::Point(55, 28);
			this->txtUsuario1->Name = L"txtUsuario1";
			this->txtUsuario1->Size = System::Drawing::Size(100, 20);
			this->txtUsuario1->TabIndex = 2;
			// 
			// txtUsuario2
			// 
			this->txtUsuario2->Location = System::Drawing::Point(55, 54);
			this->txtUsuario2->Name = L"txtUsuario2";
			this->txtUsuario2->Size = System::Drawing::Size(100, 20);
			this->txtUsuario2->TabIndex = 3;
			// 
			// lblUsuario1
			// 
			this->lblUsuario1->AutoSize = true;
			this->lblUsuario1->Location = System::Drawing::Point(3, 34);
			this->lblUsuario1->Name = L"lblUsuario1";
			this->lblUsuario1->Size = System::Drawing::Size(52, 13);
			this->lblUsuario1->TabIndex = 4;
			this->lblUsuario1->Text = L"Usuario 1";
			// 
			// lblUsuario2
			// 
			this->lblUsuario2->AutoSize = true;
			this->lblUsuario2->Location = System::Drawing::Point(3, 60);
			this->lblUsuario2->Name = L"lblUsuario2";
			this->lblUsuario2->Size = System::Drawing::Size(49, 13);
			this->lblUsuario2->TabIndex = 5;
			this->lblUsuario2->Text = L"Usuario2";
			// 
			// pnl_Usuarios
			// 
			this->pnl_Usuarios->Controls->Add(this->turno_j2);
			this->pnl_Usuarios->Controls->Add(this->turno_j1);
			this->pnl_Usuarios->Controls->Add(this->lbl_pts_j2);
			this->pnl_Usuarios->Controls->Add(this->lbl_pts_j1);
			this->pnl_Usuarios->Controls->Add(this->label2);
			this->pnl_Usuarios->Controls->Add(this->label1);
			this->pnl_Usuarios->Controls->Add(this->btnStart);
			this->pnl_Usuarios->Controls->Add(this->lblUsuario1);
			this->pnl_Usuarios->Controls->Add(this->lblUsuario2);
			this->pnl_Usuarios->Controls->Add(this->txtUsuario1);
			this->pnl_Usuarios->Controls->Add(this->txtUsuario2);
			this->pnl_Usuarios->Location = System::Drawing::Point(567, 12);
			this->pnl_Usuarios->Name = L"pnl_Usuarios";
			this->pnl_Usuarios->Size = System::Drawing::Size(250, 151);
			this->pnl_Usuarios->TabIndex = 6;
			// 
			// turno_j2
			// 
			this->turno_j2->ImageLocation = L"./check2.png";
			this->turno_j2->Location = System::Drawing::Point(212, 53);
			this->turno_j2->Name = L"turno_j2";
			this->turno_j2->Size = System::Drawing::Size(32, 19);
			this->turno_j2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->turno_j2->TabIndex = 12;
			this->turno_j2->TabStop = false;
			this->turno_j2->Visible = false;
			// 
			// turno_j1
			// 
			this->turno_j1->ImageLocation = L"./check2.png";
			this->turno_j1->Location = System::Drawing::Point(212, 28);
			this->turno_j1->Name = L"turno_j1";
			this->turno_j1->Size = System::Drawing::Size(32, 19);
			this->turno_j1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->turno_j1->TabIndex = 11;
			this->turno_j1->TabStop = false;
			this->turno_j1->Visible = false;
			// 
			// lbl_pts_j2
			// 
			this->lbl_pts_j2->AutoSize = true;
			this->lbl_pts_j2->Location = System::Drawing::Point(165, 54);
			this->lbl_pts_j2->Name = L"lbl_pts_j2";
			this->lbl_pts_j2->Size = System::Drawing::Size(0, 13);
			this->lbl_pts_j2->TabIndex = 10;
			// 
			// lbl_pts_j1
			// 
			this->lbl_pts_j1->AutoSize = true;
			this->lbl_pts_j1->Location = System::Drawing::Point(165, 28);
			this->lbl_pts_j1->Name = L"lbl_pts_j1";
			this->lbl_pts_j1->Size = System::Drawing::Size(0, 13);
			this->lbl_pts_j1->TabIndex = 9;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(209, 8);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(35, 13);
			this->label2->TabIndex = 8;
			this->label2->Text = L"Turno";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(162, 8);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(40, 13);
			this->label1->TabIndex = 7;
			this->label1->Text = L"Puntos";
			// 
			// btnStart
			// 
			this->btnStart->Location = System::Drawing::Point(6, 77);
			this->btnStart->Name = L"btnStart";
			this->btnStart->Size = System::Drawing::Size(241, 45);
			this->btnStart->TabIndex = 6;
			this->btnStart->Text = L"Start";
			this->btnStart->UseVisualStyleBackColor = true;
			this->btnStart->Click += gcnew System::EventHandler(this, &Form1::btnStart_Click);
			// 
			// pnl_game
			// 
			this->pnl_game->Controls->Add(this->tablero);
			this->pnl_game->Location = System::Drawing::Point(11, 12);
			this->pnl_game->Name = L"pnl_game";
			this->pnl_game->Size = System::Drawing::Size(550, 374);
			this->pnl_game->TabIndex = 7;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(819, 483);
			this->Controls->Add(this->pnl_game);
			this->Controls->Add(this->rtb_resumen);
			this->Controls->Add(this->pnl_Usuarios);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->pnl_Usuarios->ResumeLayout(false);
			this->pnl_Usuarios->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->turno_j2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->turno_j1))->EndInit();
			this->pnl_game->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btnStart_Click(System::Object^ sender, System::EventArgs^ e) {
		bool iniciar = true;
		if (this->txtUsuario1->Text == "")
		{
			System::Windows::Forms::MessageBox::Show("Agregar Usuario 1");
			iniciar = false;
		}
		if (this->txtUsuario2->Text == "")
		{
			System::Windows::Forms::MessageBox::Show("Agregar Usuario 2");
			iniciar = false;
		}

		if (iniciar)
		{ 
			this->tablero->BringToFront();
			for each (Control ^ cntrl in this->tablero->Controls)
			{
				cntrl->Show();
			}
			this->pnl_game->Show();
			this->tablero->Show();

			this->txtUsuario1->ReadOnly = true;
			this->txtUsuario2->ReadOnly = true;
			this->btnStart->Visible = false; 
			this->pnl_game->Enabled = true;
			
			this->setPuntos(1, 1);
			this->setPuntos(2, 1);

			int jugador = 1 + (rand() % 2);
			this->setTurno(jugador); 
		}
	}

	private: void setTurno(int jugador) {
		if (jugador == 1)
		{ 
			this->turno_j1->Visible = true;
			this->turno_j2->Visible = false;
		}
		else { 
			this->turno_j1->Visible = false;
			this->turno_j2->Visible = true;
		}
	}


	private: void setPuntos(int jugador, bool mas) {
		if (jugador == 1)
		{
			if (mas)
			{
				this->pts_1++;
			}
			else {
				this->pts_1--;
			} 
			this->lbl_pts_j1->Text = this->pts_1.ToString();
		}
		else {
			if (mas)
			{
				this->pts_2++;
			}
			else {
				this->pts_2--;
			}
			this->lbl_pts_j2->Text = this->pts_2.ToString();
		}
	} 
};
}
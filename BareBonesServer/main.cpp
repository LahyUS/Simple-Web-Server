#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WS2tcpip.h>
#include <sstream>
#include <string>
#include <istream>
#include <fstream>
#include "Utils.h"

#pragma comment (lib,"ws2_32.lib")

#define MAX 100
#define MyPort 8080

void main()
{
	//Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(1, 1);

	int wsOK = WSAStartup(ver, &wsData);
	if (wsOK != 0)
	{
		std::cout << "Can't initialize winsock!\nQuitting\n";
		return;
	}

	//Create a socket 
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); // AF_INET : IPv4 version - SockStream : TCP
	if (listening == INVALID_SOCKET)
	{
		std::cout << "Can't create a socket!\nQuitting\n";
		return;
	}

	//Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET; // IPv4 version
	hint.sin_port = htons(MyPort);	//Host to network short - // MyPort = 8080
	hint.sin_addr.S_un.S_addr = INADDR_ANY;		//Connect socket to any client's interfaces 

	//Tell winsock the server's socket is for listening 
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Start listening
	listen(listening, SOMAXCONN); //SOMAXCONN : Number of connection = max

	SOCKET NewConnection;	//Create a socket connection to client
	sockaddr_in ConnectClient;
	int nSizeAddr;
	int nResult = listen(listening, SOMAXCONN);           //Check the connection

	if (nResult == -1) //Connection failed
	{
		WSAGetLastError();
		return;
	}

	std::cout << "Listening on IP : " << inet_ntoa(hint.sin_addr) << " - port : " << hint.sin_port << std::endl;
	// sin_port reverse 2 byte of port : 8080 -> 36895

	char lpBuffRevc[4096] = { 0 }; //Content received from CLIENT (REQUEST)

	while (1)
	{
		nSizeAddr = sizeof(sockaddr);        // Determine Client's sockaddr length

		NewConnection = accept(listening, (sockaddr*)&ConnectClient, &nSizeAddr);    // Accept 1 connection	from server to client

		// Connect failed
		if (NewConnection == -1) 
		{
			std::cout << "Connect failed\n";
			continue; 
		}

		std::cout << "Receive connection from : " << inet_ntoa(ConnectClient.sin_addr) << std::endl;
		std::cout << "************* Request content***********-\n";

		// Clear lastest request content
		memset(lpBuffRevc, 0, sizeof(lpBuffRevc));

		// Receive new request content
		recv(NewConnection, lpBuffRevc, sizeof(lpBuffRevc), 0);

		// Print request content
		std::cout << lpBuffRevc << "\n\n";

		/*			HANDLE REQUEST			*/

		std::string content; // Request's content
		std::string contentType; // HTML,CSS,PNG,JPG,....
		std::stringstream response; // Response/Reply

		std::vector<std::string> req = parse_Req(std::string(lpBuffRevc)); // Get all line of request -> Vector<string> req
		std::vector<std::string> header = split(req[0], " ");	// Get first line 

		if (header[0] == "POST")	// Get first element as method GET/PUT/POST
		{
			std::vector<std::string> Info = split(req[req.size()-1], "&");	// Last line of request content has username and password

			// Check user sign in information
			if (Info[0] == "user=admin" && Info[1] == "pass=admin")
			{	// HTTP Redirect to Infomation page
				// handle_Request(lpBuffRevc, content, contentType);
				response << "HTTP/1.1 301 Moved Permanently\r\n";
				response << "Location: info.html\r\n";
				response << "\r\n";
			}
			else
			{	//Redirect 404_Error
				response << "HTTP/1.1 404 Not Found\r\n";
				response << "Content-Type: text/html\r\n";
				response << "Content - Length: 169\r\n";
				response << "Connection : keep - alive\r\n";
				response << "\r\n";
				std::string content = readFileText("404/404.html");
				response << content;
			}
		}
		else if (header[0] == "GET")
		{
			handle_Request(lpBuffRevc, content, contentType);	// Handle request from client

			// Inlet the header to respone message
			response << "HTTP/1.1" << " 200 OK" << "\r\n";
			response << "Content-Type: " << contentType << "\r\n";
			response << "Content-Length: " << content.length() << "\r\n";
			response << "\r\n"; // Ket thuc header
			response << content;
		}

		// Response message
		std::string res = response.str();

		// Send respone message to client
		send(NewConnection, res.c_str(), res.length() + 1, 0);
		
		closesocket(NewConnection); // Đóng kết nối!
	}

	WSACleanup();
}

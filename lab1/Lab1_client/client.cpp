#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <windows.h>

#pragma comment(lib,"ws2_32.lib")   //socket库
using namespace std;

#define PORT 8000  //端口号
#define BufSize 1024  //缓冲区大小
SOCKET clientSocket; //定义客户端socket
SOCKADDR_IN servAddr; //定义服务器地址
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

#define _WINSOCK_DEPRECATED_NO_WARNINGS

DWORD WINAPI recvThread() //接收消息线程
{
	while (true)
	{
		char buffer[BufSize] = {};//接收数据缓冲区
		if (recv(clientSocket, buffer, sizeof(buffer), 0) > 0)
		{
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			cout << buffer << endl;
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			cout << ">>";

		}
		else if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0)
		{
			cout << "Connection lost!" << endl;
			break;
		}
	}
	Sleep(100);//延时100ms
	return 0;
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//绑定服务器地址
	servAddr.sin_family = AF_INET;//地址类型
	servAddr.sin_port = htons(PORT);//端口号
	if (inet_pton(AF_INET, "127.0.0.1", &(servAddr.sin_addr)) != 1) {
		cout << "Inet_pton error!" << endl;
		exit(EXIT_FAILURE);
	}
	//servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	//向服务器发起请求
	if (connect(clientSocket, (SOCKADDR*)&servAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "Connection failed on: " << WSAGetLastError() << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "Connection success!" << endl;
	}


	//创建消息线程
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)recvThread, NULL, 0, 0);

	char buf[BufSize] = {};
	cout << "Enter 'logout' to quit." << endl;
	
	//发送消息
	while (true)
	{
		SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		cout << ">>";
		cin.getline(buf, sizeof(buf));
		if (strcmp(buf, "logout") == 0) //输入exit退出
		{
			break;
		}
		send(clientSocket, buf, sizeof(buf), 0);//发送消息
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
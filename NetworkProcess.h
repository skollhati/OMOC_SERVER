
//UDP �������
//���� ���� Ȯ��
//�н� ���?
//UDP ������ Non-Block�ΰ�?


#ifndef __NETWORKPROCESS_H__
#define __NETWORKPROCESS_H__
#define BUFFER_SIZE 1024

#include "stdafx.h"
#include <queue>
#include <list>

typedef struct PLAY_GAME_PLAYER
{
	sockaddr user1_sock;
	sockaddr user2_sock;
}PlayingGame;

typedef struct SEDING_TASK
{
	sockaddr from_sock;
	TCHAR* send_message;
}MESSAGE;

class NetworkProcess
{

public:
	NetworkProcess();
	

	~NetworkProcess()
	{
		closesocket(ServerSocket);
		WSACleanup();
	}

public:
	void ConnectPlayer(sockaddr* sock);
	void PassCommand(TCHAR command[]);
	BOOL SearchPlayerList(sockaddr* sock);

public:
	WSADATA wsaData;
	SOCKET ServerSocket;
	SOCKADDR_IN ServerInfo;
	SOCKADDR_IN FromClient;
	

	int Recv_Size;
	int Send_Size;
	int FromClient_Size;
	int Count;

	TCHAR Buffer[BUFFER_SIZE];
	short ServerPort = 8800;

private:
	//static UINT WINAPI SendProc(LPVOID lParam);
	//static UINT WINAPI RecvProc(LPVOID lParam);

private:

	queue<sockaddr*> qWaitingPlayer;
	queue<MESSAGE> qMessage;
	list<sockaddr*> lPlayerList;
	list<PlayingGame> lGameList;
	
	HANDLE hSendThread;
	HANDLE hRecvThread;
};

#endif

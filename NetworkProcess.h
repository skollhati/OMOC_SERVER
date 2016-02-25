
//UDP 고려사항
//전송 순서 확인
//분실 고려?
//UDP 전송이 Non-Block인가?


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

UINT WINAPI SendProc(LPVOID lParam);
UINT WINAPI RecvProc(LPVOID lParam);

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

	char Buffer[BUFFER_SIZE];
	short ServerPort = 8800;

private:
	

private:
	queue<sockaddr*> qWaitingPlayer;
	list<sockaddr*> lPlayerList;
	list<PlayingGame> lGameList;
	
	HANDLE hSendThread;
	HANDLE hRecvThread;
};
#endif

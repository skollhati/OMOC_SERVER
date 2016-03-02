
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
#include "PacketSet.h"

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
	void ConnectPlayer(PSOCKET_OBJ p_sock,TCHAR* buf);
	void CommandProcess(PSOCKET_OBJ p_sock,TCHAR* buf);
	BOOL SendPacket(SOCKET ClientSocket, WORD com, TCHAR* buf);
	void PassCommand(PSOCKET_OBJ p_sock, TCHAR* buf);
	void Disconnect(PSOCKET_OBJ pSocktObj);
	void IniSocketObj();
	WORD CheckUserNum(char* ipAddr, int iPort);
	PSOCKET_OBJ InUserVector(char* ipAddr);
	UNPACK_DATA UDPReceive(WORD UserNum, TCHAR* buffer, WORD wSize);

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
	
	static UINT WINAPI RecvProc(LPVOID lParam);
	static UINT WINAPI CheckingHeartBeatThread(LPVOID lpParam);

private:

	queue<PLAY_GAME_DATA*> qWaiting;
	list<PLAY_GAME_DATA*> lGameList;
	queue<MESSAGE> qMessage;
	vector<PSOCKET_OBJ> lPlayerList;
	//list<PlayingGame> lGameList;
	PLAY_GAME_DATA pGame[50];
	HANDLE hHeartBeat;
	HANDLE hRecvThread;
	LARGE_INTEGER liDueTime;
	int offset_game=0;
	static PacketSet pPacket;
	
};

#endif

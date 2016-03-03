
//UDP 고려사항
//전송 순서 확인
//분실 고려?
//UDP 전송이 Non-Block인가?


#ifndef __NETWORKPROCESS_H__
#define __NETWORKPROCESS_H__
#define BUFFER_SIZE 1024
#include "PacketSet.h"
#include "stdafx.h"
#include <queue>
#include <list>

static int iHeartCount = 0;

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
	void StartServer();
	void CommandProcess(PSOCKET_OBJ p_sock,TCHAR* buf);
	BOOL SendPacket(PSOCKET_OBJ Client, WORD com, TCHAR* buf);
	void InitateHeartBeat();
	void CheckHeartBeat(PSOCKET_OBJ p_sock);
	void SendPassPacket(PSOCKET_OBJ Client, TCHAR* buf);
	void PassCommand(PSOCKET_OBJ p_sock, TCHAR* buf);
	void Disconnect(PSOCKET_OBJ pSocktObj);
	void IniSocketObj();
	PLAY_GAME_DATA* SearchGameOBJ(PSOCKET_OBJ p_sock);

	void RematchGame(PSOCKET_OBJ p_sock,TCHAR* buf);
	//WORD CheckUserNum(char* ipAddr, int iPort);
	PSOCKET_OBJ InUserVector(SOCKADDR_IN Client);
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
	vector<PLAY_GAME_DATA*> vReMatch;
	queue<PLAY_GAME_DATA*> qWaiting;
	list<PLAY_GAME_DATA*> lGameList;
	queue<MESSAGE> qMessage;
	vector<PSOCKET_OBJ> lPlayerList;
	
	HANDLE hHeartBeat;
	HANDLE hHeartTimer;
	HANDLE hRecvThread;
	
	LARGE_INTEGER liDueTime;
	

	
	PacketSet pPacket;
	
};

#endif

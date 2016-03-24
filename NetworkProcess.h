
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
	char* send_message;
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
	void StartServer();
	void CommandProcess(PSOCKET_OBJ p_sock, WORD com);
	PLAY_GAME_DATA * SearchGameOBJ(WORD roomNum);
	BOOL SendPacket(PSOCKET_OBJ Client, WORD com, char* buf,WORD buf_size);
	
	BOOL SendPacket(PSOCKET_OBJ Client);

	void InitateHeartBeat();
	void MakeGameRoom(PSOCKET_OBJ p_sock, char* title);
	void SendGameRoomList(PSOCKET_OBJ p_sock);
	void CheckHeartBeat(PSOCKET_OBJ p_sock);
	void Disconnect(PSOCKET_OBJ pSocktObj, char* buf);
	void IniSocketObj();
	void JoinGameRoom(PSOCKET_OBJ p_sock, WORD buf);
	
	PSOCKET_OBJ SearchingInUserList(char* ipaddr,short port);
	PSOCKET_OBJ InUserVector(char* ipAddr, short port, char* Uname);

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
	
	static UINT WINAPI RecvProc(LPVOID lParam);
	static UINT WINAPI CheckingHeartBeatThread(LPVOID lpParam);

private:
	vector<PLAY_GAME_DATA*> vReMatch;
	vector<PLAY_GAME_DATA*> lGameList;
	vector<PSOCKET_OBJ> lPlayerList;
	HANDLE hHeartBeat;
	HANDLE hHeartTimer;
	HANDLE hRecvThread;
	LARGE_INTEGER liDueTime;
	PacketSet pPacket;

	WORD RealRoom=0;
	
};

#endif

#include "NetworkProcess.h"
#pragma comment(lib,"ws2_32.lib")

#define BUFFER_SIZE 1024

using namespace std;


NetworkProcess::NetworkProcess()
{

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)	//문제발생 예외처리
	{
		cout << "SOCKET Initialize Error" << endl;
		WSACleanup();									//ws2_32.lib 사용 종료
	}

	memset(&ServerInfo, 0, sizeof(ServerInfo));
	memset(&FromClient, 0, sizeof(FromClient));
	memset(Buffer, 0, BUFFER_SIZE);

	ServerInfo.sin_family = AF_INET; //IPv4 주소
	ServerInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerInfo.sin_port = htons(ServerPort);

	ServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "socket creation fault!" << endl;
		closesocket(ServerSocket);
		WSACleanup();
		exit(0);
	}




}

void NetworkProcess::InitateHeartBeat()
{
	CancelWaitableTimer(hHeartTimer);
}

UINT WINAPI NetworkProcess::CheckingHeartBeatThread(LPVOID lpParam)
{
	NetworkProcess * pNet = (NetworkProcess*)lpParam;
	vector<PSOCKET_OBJ>::iterator itor;
	while (1)
	{

		WaitForSingleObject(pNet->hHeartTimer, INFINITE);

		for (itor = pNet->lPlayerList.begin(); itor != pNet->lPlayerList.end(); itor++)
		{
			if ((*itor)->bOnOff)
			{
				if (iHeartCount == 0)
				{
					if (!(*itor)->bHeartBeat[0])
					{
						pNet->Disconnect(*itor, NULL);
					}

					iHeartCount = 1;
				}
				else if (iHeartCount == 1)
				{
					if (!(*itor)->bHeartBeat[1])
					{
						pNet->Disconnect(*itor, NULL);
					}
					iHeartCount = 0;
				}
			}
		}
	}
	return 0;
}

void NetworkProcess::StartServer()
{
	if (bind(ServerSocket, (struct sockaddr*)&ServerInfo, sizeof(ServerInfo)) == SOCKET_ERROR)
	{
		cout << "bind fault" << endl;
		closesocket(ServerSocket);
		WSACleanup();
		exit(0);
	}

	liDueTime.QuadPart = -100000000;
	hHeartTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	SetWaitableTimer(hHeartTimer, &liDueTime, 10000, NULL, NULL, FALSE);
	IniSocketObj();

	//hHeartBeat = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::CheckingHeartBeatThread, (LPVOID)this, 0, NULL);
	hRecvThread = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::RecvProc, (LPVOID)this, 0, NULL);

	cout << "Server Start!" << endl;
	cout << "HeartBeatThread Start" << endl;
	cout << "Recv Thread Start" << endl;

}

void NetworkProcess::IniSocketObj()
{

	for (int i = 0; i < 50; i++)
	{
		PSOCKET_OBJ pSocketObj = new SOCKET_DATA();
		memset(pSocketObj, 0, sizeof(SOCKET_DATA));
		pSocketObj->wUserNum = i;
		lPlayerList.push_back(pSocketObj);
		if ((i + 1) % 2 == 0)
		{
			PLAY_GAME_DATA *pGame = new PLAY_GAME_DATA();
			pGame->game_number = i / 2;
			pGame->use_obj = false;
			lGameList.push_back(pGame);
		}
	}

	for (int i = 0; i < 12; i++)
	{
		lGameList.at(i)->use_obj = true;
		strcpy(lGameList.at(i)->title, "test");
		RealRoom++;
	}
}


//유저소켓 정보를 가진 벡터에서 같은 객체가 있는지 찾는 함수
PSOCKET_OBJ NetworkProcess::InUserVector(char* ipAddr, short port, char* Uname)
{

	vector<PSOCKET_OBJ>::iterator itor = lPlayerList.begin();
	PSOCKET_OBJ pEmptyObj = new SOCKET_DATA();

	for (itor; itor != lPlayerList.end(); itor++)
	{
		if (strcmp((*itor)->ipAddr, ipAddr) == 0)
		{
			return (*itor);
		}
		if ((*itor)->bOnOff == 0)
		{
			pEmptyObj = *itor;
		}

	}

	pEmptyObj->bOnOff = 1;
	strcpy(pEmptyObj->ipAddr, ipAddr);
	pEmptyObj->iPort = port;
	strcpy(pEmptyObj->UserName, Uname);
	//같은 객체가 없으면 입력후 반환 - USER_IN 일때만 해야하므로 고쳐야함

	return pEmptyObj;
}

PSOCKET_OBJ NetworkProcess::SearchingInUserList(char* ipaddr, short port)
{
	vector<PSOCKET_OBJ>::iterator itor = lPlayerList.begin();
	for (itor; itor != lPlayerList.end(); itor++)
	{

		if (strcmp((*itor)->ipAddr, ipaddr) == 0 && (*itor)->iPort == port)
		{
			return (*itor);
		}
		if (itor == lPlayerList.end())
			return NULL;
	}

}


UINT WINAPI NetworkProcess::RecvProc(LPVOID lpParam)
{

	NetworkProcess* pNproc = (NetworkProcess *)lpParam;

	while (1)
	{
		pNproc->FromClient_Size = sizeof(pNproc->FromClient);

		pNproc->Recv_Size = recvfrom(pNproc->ServerSocket, (char*)&(pNproc->Buffer), BUFFER_SIZE, 0, (struct sockaddr*)&(pNproc->FromClient), &(pNproc->FromClient_Size));

		if (pNproc->Recv_Size < 0)
		{
			printf("rcvfrom() error\n");
		}



		if (pNproc->Recv_Size > 0)
			printf("받은 데이터 : %s", pNproc->Buffer);

		pNproc->pPacket.GetInit(pNproc->Buffer);

		char * addr = inet_ntoa(pNproc->FromClient.sin_addr);

		WORD temp_com = pNproc->pPacket.GetWORD();
		if (temp_com == USER_IN)
		{
			PSOCKET_OBJ p_tempSock = pNproc->InUserVector(addr, pNproc->FromClient.sin_port, pNproc->pPacket.GetStr());
			//pNproc->CheckHeartBeat(p_tempSock);
			pNproc->SendGameRoomList(p_tempSock);
		}
		else
		{
			PSOCKET_OBJ p_tempSock = pNproc->SearchingInUserList(addr, pNproc->FromClient.sin_port);

			if (p_tempSock != NULL)
				pNproc->CommandProcess(p_tempSock, temp_com);
		}

	}
	return 0;
}


void NetworkProcess::Disconnect(PSOCKET_OBJ p_SockObj, char* buf)
{
	//게임 객체 찾아서 지우기 작업 해야함
	//이것도 새로 할것
	if (p_SockObj->bOnOff != 0)
	{
		memset(p_SockObj->ipAddr, 0, sizeof(p_SockObj->ipAddr));
		p_SockObj->bOnOff = 0;
	}
	WORD gRoomNum = *(WORD*)buf;

	if (gRoomNum != 0)
	{
		PLAY_GAME_DATA* pGameData = SearchGameOBJ(gRoomNum);
		if (pGameData != NULL)
		{
			if (strcmp(pGameData->player1->ipAddr, p_SockObj->ipAddr) == 0)
			{
				SendPacket(pGameData->player2, GAME_RETIRE, NULL, 0); //상대방이 기권했다고 알림
				memset(pGameData, 0, sizeof(PLAY_GAME_DATA));
			}
			else if (strcmp(pGameData->player2->ipAddr, p_SockObj->ipAddr) == 0)
			{
				SendPacket(pGameData->player1, GAME_RETIRE, NULL, 0); //상대방이 기권했다고 알림
				memset(pGameData, 0, sizeof(PLAY_GAME_DATA));
			}
		}
	}
}

void NetworkProcess::CommandProcess(PSOCKET_OBJ p_sock, WORD com)
{
	switch (com)
	{

	case USER_OUT: Disconnect(p_sock, pPacket.GetStr());//접속 종료 프로세스
		break;

	case GAME_ROOM_LIST:SendGameRoomList(p_sock);
		break;
	case JOIN_GAME: JoinGameRoom(p_sock, pPacket.GetWORD());
		break;
	case GAME_ROOM_MAKE:MakeGameRoom(p_sock, pPacket.GetStr());
		break;
	case HEARTBEAT: CheckHeartBeat(p_sock);
		break;
	}

}

void NetworkProcess::JoinGameRoom(PSOCKET_OBJ p_sock,WORD buf)
{
	
	PLAY_GAME_DATA* FindedGame = SearchGameOBJ(buf);
	
	pPacket.Init();
	pPacket.PutWORD(JOIN_GAME);
	
	if (FindedGame->game_master == 1)
	{
		pPacket.PutWORD(FindedGame->player1->iPort);
		pPacket.PutStr(FindedGame->player1->ipAddr,strlen(FindedGame->player1->ipAddr));

	}
	else
	{
		pPacket.PutWORD(FindedGame->player2->iPort);
		pPacket.PutStr(FindedGame->player2->ipAddr, strlen(FindedGame->player2->ipAddr));
	}
	
	pPacket.ClosePacket();
	SendPacket(p_sock);
}

void NetworkProcess::SendGameRoomList(PSOCKET_OBJ p_sock)
{

	pPacket.Init();
	pPacket.PutWORD(HEAD);
	pPacket.PutWORD(RealRoom);
	SendPacket(p_sock);
	
	
	//반복문을 이용해서 방을 갯수만큼 끊어서 보낼것 방 최대 갯수는 50개
	// 500> 방 하나 당 제목 60바이트 + 방넘버 2바이트 소모 5개 방을 보낼것
	int listsize = lGameList.size();
	for (int i = 0; i < listsize; i += 5)
	{
		pPacket.Init();
		pPacket.PutWORD(BODY);
		pPacket.GameListPacketInit();
		WORD bodyCount = 0;
	
		for (int j = 0; j < 5; j++)
		{
			if (lGameList.at(i + j)->use_obj == true)
			{
				pPacket.PutWORD(lGameList.at(i + j)->game_number);
				pPacket.PutStr(lGameList.at(i + j)->title, (strlen(lGameList.at(i + j)->title)+1));
				bodyCount++;
			}
		}

		if (bodyCount > 0)
		{
			pPacket.PutBodySize(bodyCount);
			SendPacket(p_sock);
		}


		/*for (int j = 0; j < 5; j++)
		{
			if (lGameList.at(i + j)->use_obj == true)
			{
				*(WORD *)(&szBuf[offset]) = lGameList.at(i + j)->game_number;
				offset += sizeof(WORD);
				strcpy(&szBuf[offset], lGameList.at(i + j)->title);
				offset += strlen(lGameList.at(i + j)->title);
				bodyCount++;
			}
		}
		if (bodyCount > 0)
		{
			*(WORD *)(&szBuf[2]) = bodyCount;

			SendPacket(p_sock, GAME_ROOM_LIST, szBuf, offset);
		}*/
	}

}



void NetworkProcess::MakeGameRoom(PSOCKET_OBJ p_sock, char* title)
{
	//PSOCKET_OBJ가 유저이름도 갖고 있어야한다.
	for (int i = 0; i < lGameList.size(); i++)
	{
		if (!lGameList.at(i)->use_obj)
		{
			strcpy(lGameList.at(i)->player1_name, p_sock->UserName);
			strcpy(lGameList.at(i)->title, title);

			lGameList.at(i)->player1 = p_sock;
			lGameList.at(i)->use_obj = true;
			break;
		}

	}
	//게임 리스트 여유분 없을때 상황도 고려해야한다.

}

PLAY_GAME_DATA* NetworkProcess::SearchGameOBJ(WORD roomNum)
{
	for (int i = 0; i < lGameList.size(); i++)
	{
		if (lGameList.at(i)->game_number == roomNum)
		{
			return lGameList.at(i);
			break;
		}
	}

	return NULL;
}

void NetworkProcess::CheckHeartBeat(PSOCKET_OBJ p_sock)
{
	if (iHeartCount == 0)
	{
		p_sock->bHeartBeat[0] = true;
		p_sock->bHeartBeat[1] = false;
	}
	else if (iHeartCount == 1)
	{
		p_sock->bHeartBeat[0] = false;
		p_sock->bHeartBeat[1] = true;
	}
}

BOOL NetworkProcess::SendPacket(PSOCKET_OBJ Client, WORD com, char* buf, WORD buf_size)
{

	SOCKADDR_IN ToClient;
	ToClient.sin_family = AF_INET;
	ToClient.sin_addr.s_addr = inet_addr(Client->ipAddr);
	ToClient.sin_port = Client->iPort;

	pPacket.Init();
	pPacket.PutWORD(com);
	pPacket.PutStr(buf, buf_size);
	

	Send_Size = sendto(ServerSocket, (const char*)pPacket.GetSendBuffer(), BUFFER_SIZE, 0, (struct sockaddr*)&ToClient, sizeof(ToClient));

	if (Send_Size == pPacket.m_iLen)
	{
		return true;
	}

	return false;
}

BOOL NetworkProcess::SendPacket(PSOCKET_OBJ Client)
{

	SOCKADDR_IN ToClient;
	ToClient.sin_family = AF_INET;
	ToClient.sin_addr.s_addr = inet_addr(Client->ipAddr);
	ToClient.sin_port = Client->iPort;

	

	Send_Size = sendto(ServerSocket, (const char*)pPacket.GetSendBuffer(), BUFFER_SIZE, 0, (struct sockaddr*)&ToClient, sizeof(ToClient));

	if (Send_Size == pPacket.m_iLen)
	{
		return true;
	}

	return false;
}
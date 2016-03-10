
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
						pNet->Disconnect(*itor);
					}

					iHeartCount = 1;
				}
				else if (iHeartCount == 1)
				{
					if (!(*itor)->bHeartBeat[1])
					{
						pNet->Disconnect(*itor);
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

	cout << "Server Start!" << endl;

	hHeartBeat = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::CheckingHeartBeatThread, (LPVOID)this, 0, NULL);
	hRecvThread = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::RecvProc, (LPVOID)this, 0, NULL);
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
			lGameList.push_back(pGame);
		}
	}
}

//WORD NetworkProcess::CheckUserNum(PSOCKET_OBJ p_sock)
//{
//	PSOCKET_OBJ pSocket = InUserVector(p_sock->ipAddr,p_sock->iPort);
//
//	if (pSocket->bOnOff == 0)
//	{
//		strcpy(pSocket->ipAddr, ipAddr);
//		pSocket->bOnOff = TRUE;
//		pSocket->iPort = iPort;
//
//		return pSocket->wUserNum;
//	}
//	else if (pSocket == NULL)
//	{
//		return -1; //빈 객체가 없을때- 접속허용 용량을 벗어날때
//	}
//
//
//	return pSocket->wUserNum;
//}

//유저소켓 정보를 가진 벡터에서 같은 객체가 있는지 찾는 함수
PSOCKET_OBJ NetworkProcess::InUserVector(char* ipAddr, short port, TCHAR* Uname)
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
	_tcscpy(pEmptyObj->UserName, Uname);
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
			cout << "recvfrom() error" << endl;
		}

		/*	cout << "packet arrival send client is" << inet_ntoa(FromClient.sin_addr) << endl;
			cout << "packet data" << Buffer << endl;*/

			//strcpy(t_SocketOBJ->ipAddr, inet_ntoa(pNproc->FromClient.sin_addr));
		//PSOCKET_OBJ temp_sock = pNproc->InUserVector(pNproc->FromClient);

		if (pNproc->Recv_Size > 0)
			cout << "받은 데이터 :" << pNproc->Buffer << endl;

		pNproc->pPacket.GetInit(pNproc->Buffer);

		char * addr = inet_ntoa(pNproc->FromClient.sin_addr);

		WORD temp_com = pNproc->pPacket.GetWORD();
		if (temp_com == USER_IN)
		{
			pNproc->CheckHeartBeat(pNproc->InUserVector(addr, pNproc->FromClient.sin_port, pNproc->pPacket.GetStr()));

		}
		else
		{
			PSOCKET_OBJ p_tempSock = pNproc->SearchingInUserList(addr, pNproc->FromClient.sin_port);

			if (p_tempSock != NULL)
				pNproc->CommandProcess(p_tempSock, temp_com, pNproc->pPacket.GetStr());
		}

	}
	return 0;
}


void NetworkProcess::Disconnect(PSOCKET_OBJ p_SockObj, TCHAR* buf)
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
				SendPacket(pGameData->player2, GAME_RETIRE, NULL); //상대방이 기권했다고 알림
				memset(pGameData, 0, sizeof(PLAY_GAME_DATA));
			}
			else if (strcmp(pGameData->player2->ipAddr, p_SockObj->ipAddr) == 0)
			{
				SendPacket(pGameData->player1, GAME_RETIRE, NULL); //상대방이 기권했다고 알림
				memset(pGameData, 0, sizeof(PLAY_GAME_DATA));
			}
		}
	}
}

void NetworkProcess::CommandProcess(PSOCKET_OBJ p_sock, WORD com, TCHAR* buf)
{
	switch (com)
	{

	case USER_OUT: Disconnect(p_sock,buf);//접속 종료 프로세스
		break;

	case GAME_ROOM_LIST:SendGameRoomList(p_sock);
		break;
	case JOIN_GAME:
		break;
	case GAME_ROOM_MAKE:
		break;
	case HEARTBEAT: CheckHeartBeat(p_sock);
		break;
	}

}

void NetworkProcess::SendGameRoomList(PSOCKET_OBJ p_sock)
{
	TCHAR buf[100];
	*(WORD *)(&buf[0]) = HEAD;
	*(WORD *)(&buf[(sizeof(WORD))]) = (WORD)0;
	SendPacket(p_sock, GAME_ROOM_LIST, buf);
	//반복문을 이용해서 방을 갯수만큼 끊어서 보낼것


}

void NetworkProcess::MakeGameRoom(PSOCKET_OBJ p_sock, TCHAR* title)
{
	//PSOCKET_OBJ가 유저이름도 갖고 있어야한다.
	list<PLAY_GAME_DATA*>::iterator itor;
	for (itor = lGameList.begin(); itor != lGameList.end(); itor++)
	{
		if (!(*itor)->use_obj)
		{
			_tcscpy((*itor)->player1_name, p_sock->UserName);
			_tcscpy((*itor)->title, title);

			(*itor)->player1 = p_sock;
			(*itor)->use_obj = true;
			break;
		}

	}
	//게임 리스트 여유분 없을때 상황도 고려해야한다.

}

PLAY_GAME_DATA* NetworkProcess::SearchGameOBJ(WORD roomNum)
{
	list<PLAY_GAME_DATA*>::iterator itor;

	for (itor = lGameList.begin(); itor != lGameList.end(); itor++)
	{
		if ((*itor)->game_number == roomNum)
		{
			return *itor;
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

BOOL NetworkProcess::SendPacket(PSOCKET_OBJ Client, WORD com, TCHAR* buf)
{

	SOCKADDR_IN ToClient;
	ToClient.sin_family = AF_INET;
	ToClient.sin_addr.s_addr = inet_addr(Client->ipAddr);
	ToClient.sin_port = Client->iPort;

	pPacket.Init();
	pPacket.PutWORD(com);
	pPacket.PutStr(buf);
	pPacket.PutSize();

	Send_Size = sendto(ServerSocket, (const char*)pPacket.GetSendBuffer(), BUFFER_SIZE, 0, (struct sockaddr*)&ToClient, sizeof(ToClient));

	if (Send_Size == pPacket.m_iLen)
	{
		return true;
	}

	return false;
}
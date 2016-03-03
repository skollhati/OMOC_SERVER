
#include "NetworkProcess.h"

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

	hHeartBeat = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::CheckingHeartBeatThread, (LPVOID)this, 0, NULL);
	hRecvThread = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::RecvProc, (LPVOID)this, 0, NULL);
}

void NetworkProcess::IniSocketObj()
{
	for (int i = 0; i < 10; i++)
	{
		PSOCKET_OBJ pSocketObj = new SOCKET_DATA();
		memset(pSocketObj, 0, sizeof(SOCKET_DATA));
		pSocketObj->wUserNum = i;
		lPlayerList.push_back(pSocketObj);
		if ((i + 1) % 2 == 0)
		{
			PLAY_GAME_DATA *pGame = new PLAY_GAME_DATA();
			lGameList.push_back(pGame);
		}
	}
}

WORD NetworkProcess::CheckUserNum(char* ipAddr, int iPort)
{
	PSOCKET_OBJ pSocket = InUserVector(ipAddr);

	if (pSocket->bOnOff == 0)
	{
		strcpy(pSocket->ipAddr, ipAddr);
		pSocket->bOnOff = TRUE;
		pSocket->iPort = iPort;

		return pSocket->wUserNum;
	}
	else if (pSocket == NULL)
	{
		return -1; //빈 객체가 없을때- 접속허용 용량을 벗어날때
	}


	return pSocket->wUserNum;
}

//유저소켓 정보를 가진 벡터에서 같은 객체가 있는지 찾는 함수
PSOCKET_OBJ NetworkProcess::InUserVector(char* ipAddr)
{

	if (lPlayerList.size() == 0)
		return NULL;

	vector<PSOCKET_OBJ>::iterator itor = lPlayerList.begin();
	PSOCKET_OBJ pEmptyObj = NULL;
	for (itor; itor != lPlayerList.end(); itor++)
	{
		if (!strcmp((*itor)->ipAddr, ipAddr))
		{
			return (*itor);
		}
		if ((*itor)->bOnOff == 0)
		{
			pEmptyObj = *itor;
		}

	}

	return pEmptyObj;
}

UINT WINAPI NetworkProcess::RecvProc(LPVOID lpParam)
{
	NetworkProcess* pNproc = (NetworkProcess *)lpParam;
	PSOCKET_OBJ t_SocketOBJ = new SOCKET_DATA;
	while (1)
	{
		pNproc->FromClient_Size = sizeof(pNproc->FromClient);

		pNproc->Recv_Size = recvfrom(pNproc->ServerSocket, (char*)pNproc->Buffer, BUFFER_SIZE, 0, (struct sockaddr*)&(pNproc->FromClient), &(pNproc->FromClient_Size));

		if (pNproc->Recv_Size < 0)
		{
			cout << "recvfrom() error" << endl;
		}

		/*	cout << "packet arrival send client is" << inet_ntoa(FromClient.sin_addr) << endl;
			cout << "packet data" << Buffer << endl;*/

			//strcpy(t_SocketOBJ->ipAddr, inet_ntoa(pNproc->FromClient.sin_addr));
		char* ipAddr = inet_ntoa(pNproc->FromClient.sin_addr);
		PSOCKET_OBJ temp_sock = pNproc->InUserVector(ipAddr);


		pNproc->CommandProcess(temp_sock, pNproc->Buffer);


	}
	return 0;
}

void NetworkProcess::ConnectPlayer(PSOCKET_OBJ p_sock, TCHAR* buf)
{
	if (qWaiting.size() != 0)
	{
		qWaiting.front()->player2 = p_sock;
		_tcscpy(qWaiting.front()->player2_name, buf);

		SendPacket(p_sock, MATCHING_GAME, qWaiting.front()->player1_name);//매칭 성공시 대전 상대의 플레이 명 전송
		SendPacket(qWaiting.front()->player1, MATCHING_GAME, buf);


		qWaiting.pop();

	}
	else
	{
		list<PLAY_GAME_DATA*>::iterator itor;
		for (itor = lGameList.begin(); itor != lGameList.end(); itor++)
		{
			if (!(*itor)->use_obj)
			{
				_tcscpy((*itor)->player1_name, buf);
				(*itor)->player1 = p_sock;
				(*itor)->use_obj = true;
				qWaiting.push(*itor);
				break;
			}
			SendPacket(p_sock, MATCHING_GAME, _T("0")); // 매칭 실패를 의미하는 0을 보냄

		}
	}
}

void NetworkProcess::Disconnect(PSOCKET_OBJ p_SockObj)
{
	//게임 객체 찾아서 지우기 작업 해야함
	if (p_SockObj->bOnOff != 0)
	{
		memset(p_SockObj->ipAddr, 0, sizeof(p_SockObj->ipAddr));
		p_SockObj->bOnOff = 0;
	}

	PLAY_GAME_DATA* pGameData = SearchGameOBJ(p_SockObj);
	if (pGameData != NULL)
	{
		if (strcmp(pGameData->player1->ipAddr, p_SockObj->ipAddr) == 0)
		{
			SendPacket(pGameData->player2, GAME_RETIRE, NULL); //상대방이 기권했다고 알림
			memset(pGameData, 0, sizeof(PLAY_GAME_DATA));
		}
		else if(strcmp(pGameData->player2->ipAddr, p_SockObj->ipAddr) == 0)
		{
			SendPacket(pGameData->player1, GAME_RETIRE, NULL); //상대방이 기권했다고 알림
			memset(pGameData, 0, sizeof(PLAY_GAME_DATA));
		}
	}
}

void NetworkProcess::PassCommand(PSOCKET_OBJ p_sock, TCHAR* buf)
{
	SOCKADDR_IN temp_sock;

	list<PLAY_GAME_DATA*>::iterator itor;
	for (itor = lGameList.begin(); itor != lGameList.end(); itor++)
	{
		if (strcmp((*itor)->player1->ipAddr, p_sock->ipAddr) == 0)
		{

			SendPassPacket((*itor)->player2, buf);
			break;
		}
		else if (strcmp((*itor)->player2->ipAddr, p_sock->ipAddr) == 0)
		{

			SendPassPacket((*itor)->player1, buf);
			break;
		}
	}
}

void NetworkProcess::CommandProcess(PSOCKET_OBJ p_sock, TCHAR* buf)
{
	pPacket.GetInit(buf);

	switch (pPacket.GetWORD())
	{

	case USER_IN: CheckHeartBeat(p_sock); ConnectPlayer(p_sock, pPacket.GetStr());//상대방의 정보를 서로에게 전달
		break;

	case USER_OUT: Disconnect(p_sock);//접속 종료 프로세스
		break;

	case GAME_REMATCH: RematchGame(p_sock, pPacket.GetStr());//rematch 벡터에 넣고 양측 정보 대기(클라이언트 측에서 10초 이내에 입력이 없을시 CANCEL 신호를 알림
		break;
	case GAME_RESULT: //게임 결과를 양측에서 받아서 옳은 결과가 도출되었는지 판단.
		break;
	case GAME_COMMAND: PassCommand(p_sock, buf);// 전달 프로세스
		break;
	case HEARTBEAT: CheckHeartBeat(p_sock);
		break;
	}
}

PLAY_GAME_DATA* NetworkProcess::SearchGameOBJ(PSOCKET_OBJ p_sock)
{

	list<PLAY_GAME_DATA*>::iterator itor;

	for (itor = lGameList.begin(); itor != lGameList.end(); itor++)
	{
		if (strcmp((*itor)->player1->ipAddr, p_sock->ipAddr) == 0 || strcmp((*itor)->player2->ipAddr, p_sock->ipAddr) == 0)
		{
			return *itor;
			break;
		}
	}

	return NULL;
}

void NetworkProcess::RematchGame(PSOCKET_OBJ p_sock, TCHAR* buf)
{
	PLAY_GAME_DATA* temp_obj = SearchGameOBJ(p_sock);
	if (_tcscmp(buf, _T("1")) == 0)
	{
		if (temp_obj != NULL)
		{
			if (strcmp(temp_obj->player1->ipAddr, p_sock->ipAddr) == 0)
			{
				temp_obj->rematch_1 = true;
			}
			else if (strcmp(temp_obj->player2->ipAddr, p_sock->ipAddr) == 0)
			{
				temp_obj->rematch_2 = true;
			}
		}

		if (temp_obj->rematch_1 && temp_obj->rematch_2)
		{
			SendPacket(temp_obj->player1, GAME_REMATCH, _T("1"));
			SendPacket(temp_obj->player2, GAME_REMATCH, _T("1"));

			temp_obj->rematch_1 = false;
			temp_obj->rematch_2 = false;

		}
		else
		{
			SendPacket(p_sock, GAME_REMATCH, _T("2")); //2는 대기하라는 뜻
		}
	}
	else
	{
		///재대결 거부 ->  매칭 실패 알림
		if (strcmp(temp_obj->player1->ipAddr, p_sock->ipAddr) == 0)
		{
			SendPacket(temp_obj->player1, GAME_REMATCH, _T("0"));
			memset(temp_obj, 0, sizeof(PLAY_GAME_DATA));

			ConnectPlayer(temp_obj->player1, temp_obj->player1_name);
		}
		else
		{
			SendPacket(temp_obj->player2, GAME_REMATCH, _T("0"));
			memset(temp_obj, 0, sizeof(PLAY_GAME_DATA));

			ConnectPlayer(temp_obj->player2, temp_obj->player2_name);
		}
	}
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

void NetworkProcess::SendPassPacket(PSOCKET_OBJ Client, TCHAR* buf)
{
	SOCKADDR_IN ToClient;
	ToClient.sin_family = AF_INET;
	ToClient.sin_addr.s_addr = inet_addr(Client->ipAddr);
	ToClient.sin_port = Client->iPort;
	Send_Size = sendto(ServerSocket, (const char*)buf, BUFFER_SIZE, 0, (struct sockaddr*)&ToClient, sizeof(ToClient));
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

//UNPACK_DATA NetworkProcess::UDPReceive(WORD UserNum, TCHAR* buffer, WORD wSize)
//{
//	pPacket.GetInit(buffer);
//	UNPACK_DATA m_Unpack;
//
//	//사이즈에 맞게 온 함수일 경우 차례대로 진행 아닐 경우 무시
//	if (pPacket.GetSize() == wSize)
//	{
//		switch (pPacket.GetWORD())
//		{
//		//case MATCHING_GAME:
//		//	//선공 후공(0,1) + 상대 아이디
//		//	MATCHING match_game = *(MATCHING *)pPacket.GetStr();
//		//	pGameProc->setGame(match_game);
//		//	SetEvent(pGameProc->hEvent);
//		//	break;
//
//		//case GAME_COMMAND:
//		//	XY temp_xy = strToXY(pPacket.GetStr());
//		//	pGameProc->RivalStoneInput(temp_xy.y, temp_xy.x);
//		//	break;
//
//		//case GAME_INFO:
//		//	//승패
//		//	//게임 결과 유저정보 갱신 (상대편 정보도 표시)
//		//	break;
//		}
//
//	}
//	else
//		m_Unpack = { 0, };
//
//	return m_Unpack;
//}
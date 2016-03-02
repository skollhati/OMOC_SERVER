
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

	if (bind(ServerSocket, (struct sockaddr*)&ServerInfo, sizeof(ServerInfo)) == SOCKET_ERROR)
	{
		cout << "bind fault" << endl;
		closesocket(ServerSocket);
		WSACleanup();
		exit(0);
	}
	hHeartBeat = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::CheckingHeartBeatThread, (LPVOID)this, 0, NULL);
	hRecvThread = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::RecvProc, (LPVOID)this, 0, NULL);
	
}


UINT WINAPI NetworkProcess::CheckingHeartBeatThread(LPVOID lpParam)
{
	NetworkProcess* pNproc = (NetworkProcess *)lpParam;

	while (1)
	{
		
	}

	return 0;
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
	offset_game = 0;
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

void NetworkProcess::ConnectPlayer(PSOCKET_OBJ p_sock,TCHAR* buf)
{
	if (qWaiting.size() != 0)
	{
		qWaiting.front()->plyaer2 = p_sock;
		_tcscpy(qWaiting.front()->player2_name,buf);
		qWaiting.pop();
	}
	else
	{
		list<PLAY_GAME_DATA*>::iterator itor;
		for (itor = lGameList.begin(); itor != lGameList.end(); itor++)
		{
			if (!(*itor)->use_obj)
			{
				_tcscpy((*itor)->player1_name,buf);
				(*itor)->player1 = p_sock;
				(*itor)->use_obj = true;
				qWaiting.push(*itor);
				break;
			}
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
}

void NetworkProcess::PassCommand(PSOCKET_OBJ p_sock,TCHAR* buf)
{
	
}

void NetworkProcess::CommandProcess(PSOCKET_OBJ p_sock,TCHAR* buf)
{
	pPacket.GetInit(buf);
	
	switch (pPacket.GetWORD())
	{

	case USER_IN: ConnectPlayer(p_sock,pPacket.GetStr());//처음 접속, 대기열 큐 등록
		break;

	case USER_OUT: Disconnect(p_sock);//접속 종료 프로세스
		break;

	case GAME_COMMAND: // 전달 프로세스
		break;
	}
}

BOOL NetworkProcess::SendPacket(SOCKET ClientSocket,WORD com, TCHAR* buf)
{
	pPacket.Init();
	pPacket.PutWORD(com);
	pPacket.PutStr(Buffer);
	pPacket.PutSize();


//	Send_Size = sendto(ClientSocket, (const char*)Buffer, BUFFER_SIZE, 0, (struct sockaddr*)&ToServer, sizeof(ToServer));

	if (Send_Size == pPacket.m_iLen)
	{
		return true;
	}

	return false;
}

UNPACK_DATA NetworkProcess::UDPReceive(WORD UserNum, TCHAR* buffer, WORD wSize)
{
	pPacket.GetInit(buffer);
	UNPACK_DATA m_Unpack;

	//사이즈에 맞게 온 함수일 경우 차례대로 진행 아닐 경우 무시
	if (pPacket.GetSize() == wSize)
	{
		switch (pPacket.GetWORD())
		{
		//case MATCHING_GAME:
		//	//선공 후공(0,1) + 상대 아이디
		//	MATCHING match_game = *(MATCHING *)pPacket.GetStr();
		//	pGameProc->setGame(match_game);
		//	SetEvent(pGameProc->hEvent);
		//	break;

		//case GAME_COMMAND:
		//	XY temp_xy = strToXY(pPacket.GetStr());
		//	pGameProc->RivalStoneInput(temp_xy.y, temp_xy.x);
		//	break;

		//case GAME_INFO:
		//	//승패
		//	//게임 결과 유저정보 갱신 (상대편 정보도 표시)
		//	break;
		}

	}
	else
		m_Unpack = { 0, };

	return m_Unpack;
}
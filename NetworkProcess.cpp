
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
	hSendThread = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::SendProc, NULL, 0, NULL);
	hRecvThread = (HANDLE)_beginthreadex(NULL, 0, NetworkProcess::RecvProc, NULL, 0, NULL);
	
}

BOOL NetworkProcess::SearchPlayerList(sockaddr* sock)
{
	list<sockaddr*>::iterator iter = lPlayerList.begin();
	for (iter; iter != lPlayerList.end(); iter++)
	{
		if (*iter == sock)
		{
			return false;
		}

	}

	//waiting queue 확인 절차( 있으면 매칭바로 없으면 기다리라고 전송하기)

	return true;
}

//UINT WINAPI NetworkProcess::SendProc()
//{
//	while (1)
//	{
//		
//	}
//
//	return 0;
//}
//
//UINT WINAPI NetworkProcess::RecvProc()
//{
//	while (1)
//	{
//		FromClient_Size = sizeof(FromClient);
//
//		Recv_Size = recvfrom(ServerSocket, Buffer, BUFFER_SIZE, 0, (struct sockaddr*)&FromClient, &FromClient_Size);
//
//		if (Recv_Size < 0)
//		{
//			cout << "recvfrom() error" << endl;
//		}
//
//		cout << "packet arrival send client is" << inet_ntoa(FromClient.sin_addr) << endl;
//		cout << "packet data" << Buffer << endl;
//
//		if (SearchPlayerList((struct sockaddr*)&FromClient))
//		{
//			// 대전상대 매칭까지 기다리라고 메시지 보내기 or 대전상대 매칭 됐다고 보내고 게임 진행
//		}
//		else
//		{
//			//커맨드 전달하기
//		}
//
//
//		//////////////test 할것//////////////////
//
//
//	}
//	return 0;
//}
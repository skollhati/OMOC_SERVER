
#ifndef __NETWORKPROCESS_H__
#define __NETWORKPROCESS_H__


#define BUFFER_SIZE 1024


#include "stdafx.h"


class NetworkProcess
{
public:
	NetworkProcess();
	

	~NetworkProcess()
	{}

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

	
	
};
#endif

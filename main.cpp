#include "stdafx.h"
#include "NetworkProcess.h"

UINT _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	NetworkProcess pNetworkProc = NetworkProcess();
	
	Sleep(1000);
	
	pNetworkProc.StartServer();
	while (1)
	{

	}

	return 0;
}
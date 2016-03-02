#include "stdafx.h"
#include "NetworkProcess.h"

UINT _tmain(int argc, TCHAR* argv[])
{
	NetworkProcess pNetworkProc = NetworkProcess();
	
	Sleep(1000);
	
	pNetworkProc.StartServer();

	return 0;
}
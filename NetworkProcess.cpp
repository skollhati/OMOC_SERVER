
#include "NetworkProcess.h"

#define BUFFER_SIZE 1024

using namespace std;


NetworkProcess::NetworkProcess()
{
	if(WSAStartup(0x202,&wsa))
}

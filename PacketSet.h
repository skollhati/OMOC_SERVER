#ifndef __PACKETSET_H__
#define __PACKETSET_H__

#include"stdafx.h"


#define MESSAGE_REN 100
#define USER_REN 50
#define BUFFER_SIZE 1024




typedef struct PACKET_STRUCT
{
	DWORD command;
	TCHAR message[1024];
}UnPacket;

class PacketSet
{

public:
	PacketSet()
	{}

	~PacketSet()
	{}

public:
	//TCHAR* PackedPacket(TCHAR message[], WORD command);
	//UnPacket UnpackPacket(TCHAR recvPacket[]);

	void	Init();
	void	PutBYTE(BYTE	bByte);
	void    PutWORD(WORD	wWORD);
	void    Putint(int      iint);
	void	PutStr(char* sStr,WORD buf_size);
	void	PutSize();
	void ClosePacket();
	char*	PrintBuffer();
	void	GameListPacketInit();
	void	PutBodySize(WORD wWORD);
	void    GetInit(char*    ib_Buffer);
	BYTE	GetBYTE(void);
	WORD	GetWORD(void);
	int		GetInt(void);
	char*	GetStr(void);
	WORD	GetSize(void);

	char* GetSendBuffer(void) const;

	int		m_iLen;


private:
	char	m_szBuffr[2000];
	char*  m_rzBuffr;

};
#endif
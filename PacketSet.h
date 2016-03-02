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
	void	PutStr(TCHAR* sStr);
	void	PutSize();

	void    GetInit(TCHAR*    ib_Buffer);
	BYTE	GetBYTE(void);
	WORD	GetWORD(void);
	int		GetInt(void);
	TCHAR*	GetStr(void);
	WORD	GetSize(void);

	TCHAR* GetSendBuffer(void) const;

	int		m_iLen;


private:
	TCHAR	m_szBuffr[2000];
	TCHAR*  m_rzBuffr;

};
#endif
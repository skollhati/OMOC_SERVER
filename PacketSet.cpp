
#include "PacketSet.h"

void PacketSet::Init()
{
	m_iLen = 2;
	memset(m_szBuffr, 0, sizeof(m_szBuffr));
}

void PacketSet::PutSize()
{
	*(WORD *)(&m_szBuffr[0]) = m_iLen;
}

void PacketSet::PutBYTE(BYTE bByte)
{
	*(BYTE *)(&m_szBuffr[m_iLen]) = bByte;
	m_iLen += sizeof(bByte);
}

void PacketSet::PutWORD(WORD wWORD)
{
	*(WORD *)(&m_szBuffr[m_iLen]) = wWORD;
	m_iLen += sizeof(WORD);
}

void PacketSet::Putint(int iint)
{
	*(int *)(&m_szBuffr[m_iLen]) = iint;
	m_iLen += sizeof(int);
}

void PacketSet::PutStr(TCHAR* sStr)
{
	_tcscpy(&m_szBuffr[m_iLen], sStr);

	m_iLen += _tcslen(sStr);
	*(WORD*)(&m_szBuffr[m_iLen]) = 0;
	m_iLen += 2;
}

void PacketSet::GetInit(TCHAR* ib_Buffer)
{
	m_rzBuffr = ib_Buffer;
	m_iLen = 2;
}

BYTE PacketSet::GetBYTE()
{
	BYTE bByte = *(BYTE *)&m_rzBuffr[m_iLen];
	m_iLen += sizeof(BYTE);

	return bByte;
}


WORD PacketSet::GetSize()
{
	WORD wSize = *(WORD *)&m_rzBuffr[0];

	return wSize;
}

WORD PacketSet::GetWORD()
{

	WORD wWORD = *(WORD *)&m_rzBuffr[m_iLen];
	m_iLen += sizeof(WORD);

	return wWORD;
}

int PacketSet::GetInt()
{
	int iINT = *(int *)&m_rzBuffr[m_iLen];
	m_iLen += sizeof(int);

	return iINT;
}

TCHAR* PacketSet::GetStr()
{
	TCHAR* sStr = &m_rzBuffr[m_iLen];
	m_iLen += _tcslen(sStr) + 2;

	return sStr;
}


TCHAR* PacketSet::GetSendBuffer() const
{
	return (TCHAR *)m_szBuffr;
}

//TCHAR* PacketSet::PackedPacket(TCHAR message[], WORD command)
//{
//	
//	TCHAR szBuffer[1024];
//	int offset = (sizeof(WORD) * 2);
//
//	*(WORD *)(&szBuffer[0]) = (WORD)(_tcslen(message) + 6); 
//	*(WORD *)(&szBuffer[2]) = command;						
//	_tcscpy(&szBuffer[4], message);							
//	
//	offset += _tcslen(message);
//	*(WORD *)(&szBuffer[offset]) = 00;					
//	
//	return szBuffer;
//}
//
//
//UnPacket PacketSet::UnpackPacket(TCHAR recvPacket[])
//{
//	UnPacket uPacket;
//	uPacket.command = *(DWORD *)&recvPacket[2];
//	_tcscpy(uPacket.message, (TCHAR *)(&recvPacket[4]));
//
//	return uPacket;
//}





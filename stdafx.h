#ifndef __STDAFX_H__
#define __STDAFX_H__



#include <WinSock2.h>

#include <stdio.h>
#include<Windows.h>
#include<process.h>
#include<CommCtrl.h>
#include<stdlib.h>
#include<mmsystem.h>
#include<time.h>
#include<tchar.h>
#include<iostream>
#include<vector>
#include<conio.h>
#include <queue>
#define USER_IN 1				//���� ���ӿ� ���
#define USER_OUT 4				//���� �α׾ƿ��� ���
#define MATCHING_GAME 7			//���� ���� ������ �Ѵٴ� ���� �˸�
#define GAME_COMMAND 100		//���� �������� �˸�
#define GAME_INFO 101			//���� ��� �˸�
#define HEARTBEAT 5				//���� ������ �ֱ������� �˸�

using namespace std;

typedef struct XY {
	int x;
	int y;
}xy;


typedef struct SOCKET_DATA
{
	WORD wUserNum;
	char ipAddr[30];
	int iPort;
	int time;
	bool bOnOff;
}*PSOCKET_OBJ;

typedef struct MATCHING
{
	bool fPlay;
	TCHAR rivalName[50];
};

typedef struct UNPACK_DATA
{
	WORD com;
	TCHAR *buf;
};
#include "PacketSet.h"



#endif
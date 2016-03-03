#ifndef __STDAFX_H__
#define __STDAFX_H__


#include "locale.h"
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
#define HEARTBEAT 5				//���� ������ �ֱ������� �˸�

#define MATCHING_GAME 11		//���� ��밡 �ְ� ������ �˸� ���˸� : 0 ��Ī���� : ��� ���̵�
#define GAME_RETIRE 12

#define GAME_COMMAND 100		//���� �������� �˸�
#define GAME_REMATCH 101	    //���� ���� �˸� ( ���� ��� TRUE) �� �����ؾ� ����ġ ����
#define GAME_RESULT 102			//���� ��� �˸�


using namespace std;

typedef struct XY {
	int x;
	int y;
}xy;


typedef struct SOCKET_DATA
{
	BOOL bHeartBeat[2] = { 1, };
	WORD wUserNum;
	char ipAddr[30];
	int iPort;
	int time;
	bool bOnOff;
}*PSOCKET_OBJ;

typedef struct PLAY_GAME_DATA
{
	BOOL use_obj = false;
	
	BOOL rematch_1 = false;
	BOOL rematch_2 = false;
	PSOCKET_OBJ player1;
	PSOCKET_OBJ player2;

	TCHAR player1_name[50];
	TCHAR player2_name[50];

};

typedef struct UNPACK_DATA
{
	WORD com;
	TCHAR *buf;
};



#endif
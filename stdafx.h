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


//JOIN_GAME packet [size][com][ROOM_NUM] -> ���� [size][com][ip][port]
//MAKE_GAME packet [size][com][title] -> ���� [size][MAKE_GAME][ROOM_NUM]
//GAME_LIST	packet [SIZE][com][NULL]  -> [size][GAME_ROOM_LIST][HEAD][totalcount][NULL]\
															   [BODY][transcount][ROOMARRAY(5�� ����)]
//

//Ŭ�� ���� ���� �ʿ�


#define USER_IN 1				//���� ���ӿ� ���
#define USER_OUT 4				//���� �α׾ƿ��� ���
#define HEARTBEAT 5				//���� ������ �ֱ������� �˸�

#define GAME_RETIRE 12
#define JOIN_GAME 13

#define GAME_ROOM_LIST 20
#define GAME_ROOM_MAKE 21

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
	BOOL bHeartBeat[2] = { true, true };
	TCHAR UserName[10];
	WORD wUserNum;
	char ipAddr[30];
	int iPort;
	int time;
	bool bOnOff;
}*PSOCKET_OBJ;

typedef struct PLAY_GAME_DATA
{
	BOOL use_obj = false;
	WORD game_number;
	BOOL rematch_1 = false;
	BOOL rematch_2 = false;
	PSOCKET_OBJ player1;
	PSOCKET_OBJ player2;

	TCHAR title[40];
	TCHAR player1_name[20];
	TCHAR player2_name[20];

};

typedef struct UNPACK_DATA
{
	WORD com;
	TCHAR *buf;
};



#endif
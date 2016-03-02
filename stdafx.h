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

#define USER_IN 1				//서버 접속에 사용
#define USER_OUT 4				//서버 로그아웃에 사용
#define MATCHING_GAME 7			//대전 상대와 게임을 한다는 것을 알림
#define GAME_COMMAND 100		//돌의 움직임을 알림
#define GAME_INFO 101			//게임 결과 알림
#define HEARTBEAT 5				//접속 중임을 주기적으로 알림

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

typedef struct PLAY_GAME_DATA
{
	BOOL use_obj = false;
	PSOCKET_OBJ player1;
	PSOCKET_OBJ plyaer2;

	TCHAR player1_name[50];
	TCHAR player2_name[50];
};

typedef struct UNPACK_DATA
{
	WORD com;
	TCHAR *buf;
};



#endif
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

#define USER_IN 1				//서버 접속에 사용
#define USER_OUT 4				//서버 로그아웃에 사용
#define HEARTBEAT 5				//접속 중임을 주기적으로 알림

#define MATCHING_GAME 11		//대전 상대가 있고 없음을 알림 대기알림 : 0 매칭성공 : 상대 아이디
#define GAME_RETIRE 12

#define GAME_COMMAND 100		//돌의 움직임을 알림
#define GAME_REMATCH 101	    //게임 재경기 알림 ( 양쪽 모드 TRUE) 를 전송해야 리매치 성사
#define GAME_RESULT 102			//게임 결과 알림


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
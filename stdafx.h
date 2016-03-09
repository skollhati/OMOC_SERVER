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


//JOIN_GAME packet [size][com][ROOM_NUM] -> 답장 [size][com][ip][port]
//MAKE_GAME packet [size][com][title] -> 답장 [size][MAKE_GAME][ROOM_NUM]
//GAME_LIST	packet [SIZE][com][NULL]  -> [size][GAME_ROOM_LIST][HEAD][totalcount][NULL]\
															   [BODY][transcount][ROOMARRAY(5개 이하)]
//

//클라간 접속 구분 필요


#define USER_IN 1				//서버 접속에 사용
#define USER_OUT 4				//서버 로그아웃에 사용
#define HEARTBEAT 5				//접속 중임을 주기적으로 알림

#define GAME_RETIRE 12
#define JOIN_GAME 13

#define GAME_ROOM_LIST 20
#define GAME_ROOM_MAKE 21

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
#include "stdafx.h"

#include "hoxy_Header.h"
#include "Start_Protocol.h"

/*
서버는 클라에게 순서대로 잘 보내준다.
클라는 서버가 보내준걸 잘 쪼개서 쓴다.
*/

// define
#define SERVERPORT 3000
#define WIDTH 80
#define HEIGHT 23
#define MAX_ARRNUM 50

// 서버 소켓
SOCKET g_serverSock;

// 스타 정보 저장 구조체 배열
st_StartInfo g_startInfoArr[MAX_ARRNUM];

// 내 ID
int g_id = -1;
int g_myIdx = -1;

// 사이즈만큼 받기
int recvn(SOCKET sock, char* buf, int size);

bool NetInit(void);
bool NetProc(void);
bool PacketProc(char* buf);
bool KeyProcess(void);
bool Render(void);

// 범위 체크
bool AreaCheck(int x, int y);

int main()
{
	CCmdStart CmdStart;

	HANDLE  hConsole;
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// 화면의 커서를 안보이게끔 설정한다.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;			// 커서 크기.
	// 이상하게도 0 이면 나온다. 1로하면 안나온다.

	//-------------------------------------------------------------
	// 콘솔화면 (스텐다드 아웃풋) 핸들을 구한다.
	//-------------------------------------------------------------
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);

	// 소켓 등등 초기화
	NetInit();

	while (1)
	{
		KeyProcess();

		// 네트워크 처리
		if (!NetProc())
		{
			break;
		}

		Render();
	}

	return 0;
}

// recvn
int recvn(SOCKET sock, char * buf, int size)
{
	int left = size;
	char* nowPtr = buf;

	while (left > 0)
	{
		int retval = recv(sock, nowPtr, left, 0);
		if (retval < 0)
		{
			CCmdStart::CmdDebugText(L"recvn()", false);
			return -1;
		}

		left -= retval;
		nowPtr += retval;
	}

	return size - left;
}

// 네트워크 초기화
bool NetInit(void)
{
	CSockUtill::WSAStart();

	g_serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_serverSock == SOCKET_ERROR)
	{
		CCmdStart::CmdDebugText(L"socket()", false);
		return false;
	}
	else
	{
		CCmdStart::CmdDebugText(L"socket()", true);
	}

	// IP 입력
	WCHAR szServerIP[30];// = { 0, };
	wcout << L"INPUT IP : ";
	wcin >> szServerIP;
	wcout << szServerIP << endl;

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVERPORT);
	InetPton(AF_INET, szServerIP, &serverAddr.sin_addr.s_addr);

	// connect
	int ret_con = connect(g_serverSock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR));
	if (ret_con == SOCKET_ERROR)
	{
		CCmdStart::CmdDebugText(L"connect()", false);
		return false;
	}
	else
	{
		CCmdStart::CmdDebugText(L"connect()", true);
	}

	return true;
}

bool NetProc(void)
{
	while (1)
	{
		FD_SET rset;
		FD_ZERO(&rset);
		FD_SET(g_serverSock, &rset);

		timeval sTime = { 0, 0 };
		int retval = select(0, &rset, NULL, NULL, &sTime);
		if (retval == SOCKET_ERROR)
		{
			CCmdStart::CmdDebugText(L"select", false);
			return false;
		}
		else if (retval == 0)
		{
			break;
		}

		char buf[16] = { 0, };
		int recvRet = recvn(g_serverSock, buf, 16);
		wcout << L"recved : " << recvRet << endl;

		if (recvRet < 0)
		{
			return false;
		}

		PacketProc(buf);
	}

	return true;
}

// 패킷 처리
bool PacketProc(char * buf)
{
	// local pBuf
	char* pBuf = buf;

	// 패킷 수신용 구조체로 형변환해서 패킷 타입만 얻어온다.
	int packetType = ((st_Packet*)buf)->Type;

	switch (packetType)
	{
	//ID할당(0)
	case e_Type::ID_ALLOC:
	{
		g_id = ((st_IdAlloc*)pBuf)->ID;
	}
	break;

	//별생성(1)
	case e_Type::STAR_MAKE:
	{
		for (int i = 0; i < MAX_ARRNUM; i++)
		{
			if (g_startInfoArr[i]._inUse == true)
			{
				continue;
			}

			g_startInfoArr[i]._inUse = true;
			g_startInfoArr[i]._ID = ((st_StarMake*)pBuf)->ID;
			g_startInfoArr[i]._X = ((st_StarMake*)pBuf)->X;
			g_startInfoArr[i]._Y = ((st_StarMake*)pBuf)->Y;

			// 생성패킷 아이디가 내 아이디 일 경우 해당 인덱스 저장
			if (g_id == g_startInfoArr[i]._ID)
			{
				g_myIdx = i;
			}

			break;
		}
	}
	break;

	//별삭제(2)
	case e_Type::STAR_DEL:
	{
		for (int i = 0; i < MAX_ARRNUM; i++)
		{
			if (g_startInfoArr[i]._ID != ((st_StartDelete*)pBuf)->ID)
			{
				continue;
			}
			g_startInfoArr[i]._inUse = false;

			break;
		}
	}
	break;

	//이동(3)
	case e_Type::STAR_MOVE:
	{
		if (!AreaCheck(((st_StarMove*)pBuf)->X, ((st_StarMove*)pBuf)->Y))
		{
			return false;
		}

		for (int i = 0; i < MAX_ARRNUM; i++)
		{
			if (g_startInfoArr[i]._ID != ((st_StarMove*)pBuf)->ID)
			{
				continue;
			}

			g_startInfoArr[i]._X = ((st_StarMove*)pBuf)->X;
			g_startInfoArr[i]._Y = ((st_StarMove*)pBuf)->Y;

			break;
		}
	}
	break;
	}

	return true;
}

bool KeyProcess(void)
{
	// 내 캐릭터 정보가 아직 없으면 리턴
	if (g_id == -1 || g_myIdx == -1)
	{
		return false;
	}

	st_StarMove sendBuf;

	int toX = 0;
	int toY = 0;
	bool isKeyDown = false;

	toX = g_startInfoArr[g_myIdx]._X;
	toY = g_startInfoArr[g_myIdx]._Y;

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		sendBuf.ID = g_id;
		sendBuf.Type = e_Type::STAR_MOVE;
		sendBuf.X = toX - 1;
		sendBuf.Y = toY;

		isKeyDown = true;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		sendBuf.ID = g_id;
		sendBuf.Type = e_Type::STAR_MOVE;
		sendBuf.X = toX + 1;
		sendBuf.Y = toY;

		isKeyDown = true;
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		sendBuf.ID = g_id;
		sendBuf.Type = e_Type::STAR_MOVE;
		sendBuf.X = toX;
		sendBuf.Y = toY - 1;

		isKeyDown = true;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		sendBuf.ID = g_id;
		sendBuf.Type = e_Type::STAR_MOVE;
		sendBuf.X = toX;
		sendBuf.Y = toY + 1;

		isKeyDown = true;
	}

	if (isKeyDown)
	{
		if (!AreaCheck(sendBuf.X, sendBuf.Y))
		{
			return false;
		}

		g_startInfoArr[g_myIdx]._X = sendBuf.X;
		g_startInfoArr[g_myIdx]._Y = sendBuf.Y;

		int retval = send(g_serverSock, (const char*)&sendBuf, sizeof(st_StarMove), 0);
		if (retval == SOCKET_ERROR)
		{
			CCmdStart::CmdDebugText(L"send()", false);
		}

		wcout << L"send : " << retval << endl;
		wcout << L"player : " << sendBuf.ID << ", X : " << sendBuf.X << L"Y : " << sendBuf.Y << endl;
	}

	return true;
}

bool Render(void)
{
	char groundArr[HEIGHT][WIDTH] = { 0, };

	memset(groundArr, ' ', sizeof(groundArr));

	for (int i = 0; i < HEIGHT; i++)
	{
		groundArr[i][WIDTH - 1] = '\0';
	}

	for (int i = 0; i < MAX_ARRNUM; i++)
	{
		if (g_startInfoArr[i]._inUse == false)
		{
			continue;
		}

		int x = g_startInfoArr[i]._X;
		int y = g_startInfoArr[i]._Y;

		groundArr[y][x] = '*';
	}

	system("cls");

	for (int i = 0; i < HEIGHT; i++)
	{
		printf("%s\n", groundArr[i]);
	}

	return true;
}

bool AreaCheck(int x, int y)
{
	if (x < 0 || x > WIDTH - 2 || y < 0 || y > HEIGHT - 1)
	{
		return false;
	}

	return true;
}
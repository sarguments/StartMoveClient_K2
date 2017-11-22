#pragma once

// 구버전 제외
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "ws2_32")

#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include <iostream>
#include <iomanip>
#include <locale>
//#include <locale.h>

using std::wcout;
using std::wcin;
using std::endl;

#include "hoxy_CmdStart.h"

//#include "hoxy_TcpSock.h"
#include "hoxy_SockUtill.h"
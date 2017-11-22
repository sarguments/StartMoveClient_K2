#include "stdafx.h"
#include "hoxy_Header.h"

BOOL CSockUtill::DomainToIP(WCHAR * szDomain, IN_ADDR * pAddr)
{
	ADDRINFOW* pAddrInfo;
	SOCKADDR_IN* pSockAddr;
	if (GetAddrInfo(szDomain, L"0", NULL, &pAddrInfo) != 0)
	{
		return FALSE;
	}
	pSockAddr = (SOCKADDR_IN*)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pAddrInfo);
	return TRUE;
}

bool CSockUtill::WSAStart()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		CCmdStart::CmdDebugText(L"WSAStartup", false);
		return false;
	}
	return true;
}

void CSockUtill::CleanUp()
{
	WSACleanup();
}

//CTcpSocket* CSockUtill::CreateTCPSocket(e_SocketAddressFamily inFamily)
//{
//	SOCKET tcpSocket = socket(inFamily, SOCK_STREAM, IPPROTO_TCP);
//
//	if (tcpSocket != INVALID_SOCKET)
//	{
//		return new CTcpSocket(tcpSocket);
//	}
//	else
//	{
//		CCmdStart::CmdDebugText(L"CSockUtill::CreateTCPSocket", false);
//		return nullptr;
//	}
//}
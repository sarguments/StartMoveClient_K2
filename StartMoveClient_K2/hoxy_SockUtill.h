#pragma once

enum e_SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};

// == MEMO ==
// 1. sin_addr.S_un.S_addr == sin_addr.s_addr

/*
ex)
	WCHAR serverDomain[] = L"procademyserver.iptime.org";
	IN_ADDR s_inAddr;
	ZeroMemory(&s_inAddr, sizeof(IN_ADDR));

	CSocketUtil::DomainToIP(serverDomain, &s_inAddr);
	WCHAR proIP[20] = { 0, };
	InetNtop(AF_INET, &s_inAddr, proIP, 20);

	wcout << L"SERVER IP : " << proIP << endl;
*/
class CSockUtill
{
public:
	static BOOL DomainToIP(WCHAR* szDomain, IN_ADDR* pAddr);

	static bool	WSAStart();
	static void	CleanUp();

	//static CTcpSocket* CreateTCPSocket(e_SocketAddressFamily inFamily);

private:
};
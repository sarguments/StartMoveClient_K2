#include "stdafx.h"
#include "hoxy_Header.h"
#include "hoxy_CmdStart.h"

CCmdStart::CCmdStart()
{
	//_wsetlocale(LC_ALL, L"");
	// 처음 한번만
	//std::locale::global(std::locale(""));
	//CCmdStart::CmdDebugText(L"Locale Init", true);

	//// 콘솔 창 띄우기
	//AllocConsole();
	//FILE *acStreamOut;
	//freopen_s(&acStreamOut, "CONOUT$", "wt", stdout);
	//wcout << L"Consol Window 초기화" << std::setw(30) << L"|OK|" << endl;
}

CCmdStart::~CCmdStart()
{
}

void CCmdStart::CmdDebugText(WCHAR* text, bool bParam)
{
	if (bParam)
	{
		wcout << std::setw(20) << text << std::setw(50) << L"|OK|" << endl;
	}
	else
	{
		wcout << std::setw(20) << text << std::setw(40) << L"|ERROR| : " << std::setw(10) << GetLastError() << endl;
	}
}
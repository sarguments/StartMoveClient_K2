#pragma once

struct st_StartInfo
{
	bool _inUse = false;
	int _ID;
	int _X;
	int _Y;
};

enum e_Type
{
	ID_ALLOC = 0,
	STAR_MAKE,
	STAR_DEL,
	STAR_MOVE,
	NONE
};

//패킷프로토콜 - 패킷은 16바이트 고정
struct st_Packet
{
	int Type;
	int ID;
	int X;
	int Y;
};

//ID할당(0)	Type(4Byte) | ID(4Byte) | 안씀(4Byte) | 안씀(4Byte)
struct st_IdAlloc
{
	int Type;
	int ID;
	int notUse1 = UINT_MAX;
	int notUse2 = UINT_MAX;
};


//별생성(1)	Type(4Byte) | ID(4Byte) | X(4Byte) | Y(4Byte)
struct st_StarMake
{
	int Type;
	int ID;
	int X;
	int Y;
};

//별삭제(2)	Type(4Byte) | ID(4Byte) | 안씀(4Byte) | 안씀(4Byte)
struct st_StartDelete
{
	int Type;
	int ID;
	int notUse1 = UINT_MAX;
	int notUse2 = UINT_MAX;
};

//이동(3)		Type(4Byte) | ID(4Byte) | X(4Byte) | Y(4Byte)
struct st_StarMove
{
	int Type;
	int ID;
	int X;
	int Y;
};

//0 ~2번 패킷은 서버->클라패킷 이며, 3번 패킷은  클라 <->서버  쌍방 패킷임.

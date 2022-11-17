#pragma once

enum PACKETLIST
{
	NONE = 0,
	IRC = 1,
	TEST,
	MaxPacket,
};

typedef struct PKIRC
{
	char	Key[64];
	char	Message[512];
}*LPPKIRC;

typedef struct PKTEST
{
	unsigned int	TestKey;

	char	Message[512];
}*LPPKTEST;

#pragma once

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#define SIZE_BODY		2
#define SIZE_PROTOCOL	2
#define SIZE_HEADER		SIZE_BODY + SIZE_PROTOCOL

struct	header_special
{
	unsigned	short	body_size;
	unsigned	short	command;
};

enum CS_AUTH_PROTOCOL
{
	CS_AUTH_START		= 0x1000,

	CS_AUTH_PLAYERKEY	= CS_AUTH_START + 1,
	
	CS_AUTH_LOGIN_REQ	= CS_AUTH_START + 2,
	CS_AUTH_LOGIN_ACK	= CS_AUTH_START + 3,
};

struct CS_AUTH_PLAYERKEY
{
	BYTE	wCryptKey;
};

struct	body_CS_AUTH_LOGIN_REQ
{
	int	id;
	int	pw;
};

#endif	// __PROTOCOL_H__
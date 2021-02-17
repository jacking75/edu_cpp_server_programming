#include "stdafx.h"
#include "Session.h"
#include "AttachServerMessage.h"
#include "fmt\format.h"
#include <iostream>

void CAttachServerMessage::SessionClose(CSession* session_, int reason_)
{
//	std::cout << fmt::format("session id: [{0}] close. by [{1}]\n", session_->GetSessionID(), reason_).c_str();

//	stringWriter temp;
//	temp << "session id: [" << session_->GetSessionID() << "] close. by [" << reason_ << "]\n";
//	std::cout << temp.c_str();

	printf("session id: [%lld] close. by [%d]\n", session_->GetSessionID(), reason_);	
}

void CAttachServerMessage::Accept(SESSION_ID id_, CSession* session_)
{
//	std::cout << fmt::format("session id: [{0}] accept.\n", session_->GetSessionID()).c_str();

//	stringWriter temp;
//	temp << "session id: [" << session_->GetSessionID() << "] accept.\n";
//	std::cout << temp.c_str();

	printf("someone accepted.\n");
}


void CAttachServerMessage::Recv(SESSION_ID id_, CSession* session_, int dwBytes_)
{
//	std::cout << fmt::format("session id: [{0}] recv. bytes [{1}]\n", id_, dwBytes_).c_str();

//	stringWriter temp;
//	temp << "session id: [" << session_->GetSessionID() << "] recv. bytes [" << dwBytes_ << "]\n";
//	std::cout << temp.c_str();

//	printf("session id: [%lld] recv. bytes [%d]\n", id_, dwBytes_);
	
}


void CAttachServerMessage::Send(SESSION_ID id_, CSession* session_, int dwBytes_)
{
//	std::cout << fmt::format("session id: [{0}] send. bytes [{1}]\n", session_->GetSessionID(), dwBytes_).c_str();

//	stringWriter temp;
//	temp << "session id: [" << session_->GetSessionID() << "] send. bytes [" << dwBytes_ << "]\n";
//	std::cout << temp.c_str();

//	printf("session id: [%lld] send. bytes [%d]\n", id_, dwBytes_);
}


CAttachServerMessage::CAttachServerMessage()
	: CAttachModule(CAttachModule::ModulerType::Module_ServerMessage)
{
}

CAttachServerMessage::~CAttachServerMessage()
{
}

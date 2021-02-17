#include "AttachModule.h"
#include "Session.h"

void CAttachModule::SessionClose(CSession* session_, int reason_)
{
	// Don't call this.
}


void CAttachModule::Accept(SESSION_ID id_, CSession* session_)
{
	// Don't call this.
}


void CAttachModule::Recv(SESSION_ID id_, CSession* session_, int dwBytes_)
{
	// Don't call this.
}


void CAttachModule::Send(SESSION_ID id_, CSession* session_, int dwBytes_)
{
	// Don't call this.
}


CAttachModule::CAttachModule(ModulerType moduleType_) :
	m_moduleType(moduleType_)
{
}


CAttachModule::~CAttachModule()
{
}


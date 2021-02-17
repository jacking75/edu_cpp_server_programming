#include "DBBaseTask.h"


void CDBBaseTask::Init(SConnectionInfo* connInfo_)
{
	m_connection_ptr = connInfo_->connection_ptr;
	m_record_ptr = connInfo_->record_ptr;
	m_command_ptr = connInfo_->command_ptr;
}


bool CDBBaseTask::Execute()
{
	// don't call this
	return false;
}


void CDBBaseTask::WriteLog()
{
	// don't call this
}


CDBBaseTask::CDBBaseTask()
	: m_userID(INVALID_USER_ID)
{
}


CDBBaseTask::~CDBBaseTask()
{
	m_connection_ptr = NULL;
	m_record_ptr = NULL;
	m_command_ptr = NULL;
}

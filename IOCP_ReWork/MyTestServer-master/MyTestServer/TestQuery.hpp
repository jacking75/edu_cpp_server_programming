#pragma once
#include "DBBaseTask.h"
#include "Define.h"
#include "fmt\format.h"


struct CTestQuery : CDBBaseTask
{
	USER_ID user_id;

	virtual bool Execute() override
	{
		m_command_ptr->CommandText = fmt::format("select user_id, user_name from tbl_user where user_id = {0}", user_id).c_str();

		try
		{
			m_record_ptr = m_command_ptr->Execute(NULL, NULL, adCmdText);
		}
		catch (...)
		{
			return false;
		}

		if (m_record_ptr->adoEOF)
		{
			// 로그(데이터 없음)
			m_record_ptr->Close();
			return false;
		}

		while (!m_record_ptr->adoEOF)
		{
			INT64 userID = m_record_ptr->Fields->GetItem("user_id")->GetValue();
			std::string  name = (_bstr_t)m_record_ptr->Fields->GetItem("user_name")->GetValue();

			m_record_ptr->MoveNext();
		}
		m_record_ptr->Close();

		return true;
	}

	virtual void WriteLog() override
	{
	}
};
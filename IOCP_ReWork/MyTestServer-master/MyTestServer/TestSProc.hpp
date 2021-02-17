#pragma once
#include "DBBaseTask.h"
#include "Define.h"
#include "fmt\format.h"


struct CTestSProc: CDBBaseTask
{
	std::string user_name;

	virtual bool Execute() override
	{
		m_command_ptr->CommandText = _bstr_t("test_sp");
		m_command_ptr->CommandType = adCmdStoredProc;
		m_command_ptr->ActiveConnection = m_connection_ptr;
		m_record_ptr->CursorLocation = adUseClient;

		m_command_ptr->Parameters->Append(m_command_ptr->CreateParameter(_bstr_t("i_name"), adVarChar, adParamInput, 30, user_name.c_str()));
		m_command_ptr->Parameters->Append(m_command_ptr->CreateParameter(_bstr_t("o_re"), adInteger, adParamOutput, sizeof(INT), vtMissing));

		try
		{
			m_record_ptr = m_command_ptr->Execute(NULL, NULL, adCmdStoredProc);
			m_command_ptr->Parameters->Refresh();
		}
		catch (...)
		{
			// ·Î±× 
			//m_command_ptr->Parameters->Refresh();
			return false;
		}

		if (m_record_ptr->adoEOF)
		{
			m_record_ptr->Close();
			return false;
		}

		
		while (!m_record_ptr->adoEOF)
		{			
			int re = m_record_ptr->Fields->GetItem("o_re")->GetValue();
			m_record_ptr->MoveNext();
		}
		m_record_ptr->Close();
		return true;
	}

	virtual void WriteLog() override
	{
	}
};
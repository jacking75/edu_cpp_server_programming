#include "stdafx.h"
#include "ado.h"

CAdo::CAdo(SAdoConfig& adoconfig)
	:m_tstrConnectingString(adoconfig.GetConnectionString()),
	m_tstrUserID(adoconfig.GetUserID()),
	m_tstrPassword(adoconfig.GetPassword()),
	m_tstrInitCatalog(adoconfig.GetInitCatalog()),
	m_tstrProvider(adoconfig.GetProvider()),
	m_tstrDSN(adoconfig.GetDSN()),
	m_nConnectionTimeout(adoconfig.GetConnectionTimeout()),
	m_nCommandTimeout(adoconfig.GetCommandTimeout()),
	m_bRetryConnection(adoconfig.GetRetryConnection()),
	m_bAutoCommit(false),

	m_pConnection(NULL), 
	m_pCommand(NULL),
	m_pRecordset(NULL),

	m_IsSuccess(TRUE),
	m_tstrParameterName(),
	m_tstrColumnName(),
	m_tstrQuery(),
	m_tstrCommand()
{
	if(FAILED(::CoInitialize(NULL))) {
		LOG(_T("::CoInitialize Fail!!"));
		return;
	}

	m_pConnection.CreateInstance(__uuidof(Connection));
	m_pCommand.CreateInstance(__uuidof(Command)); 
}

CAdo::~CAdo()
{
	Close();
}

void CAdo::Init()
{
	m_bAutoCommit = false;
	m_IsSuccess = TRUE;
	m_tstrParameterName.clear();
	m_tstrColumnName.clear();
	m_tstrQuery.clear();
	m_tstrCommand.clear();
}

void CAdo::dump_com_error(_com_error &e)
{
	m_IsSuccess = FALSE;
	if(0X80004005 == e.Error())
		Close();

	LOG(_T("Code = %08lX   Code meaning = %s"), e.Error(), e.ErrorMessage());
	LOG(_T("Source = %s"), (LPCTSTR)e.Source());
	LOG(_T("Desc = %s"), (LPCTSTR)e.Description());

}

void CAdo::dump_user_error()
{
	m_IsSuccess = FALSE;

	if(!m_tstrQuery.empty())
		LOG(_T("SQLQuery[%s]"), m_tstrQuery.c_str());
	if(!m_tstrCommand.empty())
		LOG(_T("Command[%s]"), m_tstrCommand.c_str());
	if(!m_tstrColumnName.empty())
		LOG(_T("Column[%s]"), m_tstrColumnName.c_str());
	if(!m_tstrParameterName.empty())
		LOG(_T("Paramter[%s]"), m_tstrParameterName.c_str());
}

BOOL CAdo::GetFieldCount(int& nValue)
{
	m_tstrCommand = _T("GetFieldCount()");
	try
	{
		nValue =  m_pRecordset->GetFields()->GetCount();;
	} catch(_com_error &e) {
		dump_com_error(e);
		return FALSE;	
	}
	return TRUE;
}

BOOL CAdo::RetryOpen()
{
	if(m_pConnection->GetState() != adStateClosed) return FALSE;

	return Open();
}

BOOL CAdo::Execute(CommandTypeEnum CommandType /*= adCmdStoredProc*/, ExecuteOptionEnum OptionType /*= adOptionUnspecified*/)
{
	if(!m_IsSuccess) return FALSE;

	try	{
		if(m_pConnection->GetState() == adStateClosed && true == m_bRetryConnection)
		{	
			m_tstrCommand = _T("RetryOpen()");  	//재연결 시도
			if(FALSE == RetryOpen())
				return FALSE;
			else
				m_IsSuccess = true;
		}
		m_tstrCommand = _T("Execute()");

		m_pCommand->CommandType = CommandType;
		m_pCommand->CommandText = m_tstrQuery.c_str();
		if(0 != m_nConnectionTimeout)
			m_pCommand->CommandTimeout = m_nConnectionTimeout;
		//			m_pConnection->CursorLocation = adUseClient;
		m_pRecordset = m_pCommand->Execute(NULL,NULL,OptionType); 	
	} catch(_com_error &e) {
		dump_com_error(e);
		return FALSE;	
	}
	return TRUE;
}

void CAdo::MoveNext()
{
	m_tstrCommand = _T("MoveNext()");
	try	{
		m_pRecordset->MoveNext();
	} catch (_com_error &e)	{
		dump_com_error(e);
		return;
	}
	return;
}

BOOL CAdo::GetEndOfFile()
{
	m_tstrCommand = _T("GetEndOfFile()");
	BOOL bEndOfFile = TRUE;

	try 
	{
		//bEndOfFile = m_pRecordset->GetEndOfFile();
		VARIANT_BOOL vbEnd = m_pRecordset->GetEndOfFile();
		if( 0 == vbEnd ) {
			bEndOfFile = FALSE;
		}
	}
	catch (_com_error &e) 
	{
		dump_com_error(e);
	}
	
	return bEndOfFile;
}

BOOL CAdo::NextRecordSet()
{
	m_tstrCommand = _T("NextRecordSet()");

	_variant_t variantRec;
	variantRec.intVal = 0;
	try	{
		m_pRecordset = m_pRecordset->NextRecordset((_variant_t*)&variantRec);
	} catch (_com_error &e) {
		dump_com_error(e);
		return FALSE;
	}
	return TRUE;
}

BOOL CAdo::GetFieldValue(IN TCHAR* tszName, OUT TCHAR* pszValue, IN unsigned int nSize)
{
	m_tstrCommand = _T("GetFieldValue(string)");

	m_tstrColumnName = tszName;
	try	{
		_variant_t vFieldValue = m_pRecordset->GetCollect(tszName);


		if(VT_NULL == vFieldValue.vt ||	VT_EMPTY ==  vFieldValue.vt) {
				m_tstrColumnName += _T(" null value");
				return FALSE;
		} else if(VT_BSTR != vFieldValue.vt) {
			m_tstrColumnName += _T(" nonbstr type");
			return FALSE;
		} if(nSize < _tcslen((TCHAR*)(_bstr_t(vFieldValue.bstrVal)))) {
			m_tstrColumnName += _T(" string size overflow");
			return FALSE;
		}

		//_tcscpy(pszValue, (TCHAR*)static_cast<_bstr_t>(vFieldValue.bstrVal));
		_tcscpy_s(pszValue, nSize, (TCHAR*)static_cast<_bstr_t>(vFieldValue.bstrVal));

	} catch (_com_error &e) {
		dump_com_error(e);
		return FALSE;
	}
	return TRUE;
}

BOOL CAdo::GetFieldValue(IN TCHAR* tszName, OUT BYTE* pbyBuffer, IN int inSize, OUT int& outSize)
{
	m_tstrCommand = _T("GetFieldValue(binary)");

	m_tstrColumnName = tszName;
	try
	{
		_variant_t vFieldValue = m_pRecordset->GetCollect(tszName);

		if(VT_NULL == vFieldValue.vt)
		{
			m_tstrColumnName += _T(" null value");
			return FALSE;
		}else if((VT_ARRAY|VT_UI1) != vFieldValue.vt) {
			m_tstrColumnName += _T(" nonbinary type");
			return FALSE;
		}

		FieldPtr pField = m_pRecordset->Fields->GetItem(tszName);

		if(inSize < pField->ActualSize || inSize > 8060)
		{
			m_tstrColumnName += _T(" binary size overflow");
			dump_user_error();
			return FALSE;
		}

		outSize = static_cast<int>(pField->ActualSize);

		BYTE * pData = NULL;
		SafeArrayAccessData(vFieldValue.parray, (void HUGEP* FAR*)&pData);
		memcpy(pbyBuffer, pData, static_cast<size_t>(pField->ActualSize));
		SafeArrayUnaccessData(vFieldValue.parray);
	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}
	return TRUE;
}

void CAdo::CreateNullParameter(IN TCHAR* tszName, IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction)
{
	if(!m_IsSuccess) return;

	m_tstrCommand = _T("CreateParameter(null)");
	m_tstrParameterName = tszName;
	try	{
		_ParameterPtr pParametor(m_pCommand->CreateParameter(tszName,Type,Direction, 0));
		m_pCommand->Parameters->Append(pParametor);
		_variant_t vNull;
		vNull.ChangeType(VT_NULL);
		pParametor->Value = vNull;
	} catch (_com_error &e) {
		dump_com_error(e);
	}

	return;
}

void CAdo::CreateParameter(IN TCHAR* tszName,IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction,
							  IN TCHAR* pValue, IN int nSize)
{
	if(!m_IsSuccess) return;

	m_tstrCommand = _T("CreateParameter(TCHAR)");
	m_tstrParameterName = tszName;

	_ASSERTE(nSize > 0 && "not allow 0 size!!");

	try	{
		_ParameterPtr pParametor(m_pCommand->CreateParameter(tszName,Type,Direction, nSize));
		m_pCommand->Parameters->Append(pParametor);

		if(NULL == pValue)
		{
			_variant_t vValue;
			vValue.vt = VT_NULL;
			pParametor->Value = vValue;
		}
		else
		{
			_variant_t vValue(pValue);
			pParametor->Value = vValue;
		}
	} catch (_com_error &e) {
		dump_com_error(e);
	}

	return;
}

void CAdo::CreateParameter(IN TCHAR* tszName,IN enum DataTypeEnum Type, IN enum ParameterDirectionEnum Direction,
							  IN BYTE* pValue, IN int nSize)
{
	if(!m_IsSuccess) return;

	_ASSERTE(nSize > 0 && "not allow 0 size!!");

	m_tstrCommand = _T("CreateParameter(binary)");
	m_tstrParameterName = tszName;

	try	{
		_ParameterPtr pParametor(m_pCommand->CreateParameter(tszName,Type,Direction, nSize));
		m_pCommand->Parameters->Append(pParametor);

		_variant_t vBinary;
		SAFEARRAY FAR *pArray = NULL;
		SAFEARRAYBOUND rarrayBound[1];

		if(NULL == pValue)		//명시적 null이거나 값이 null이라면
		{
			vBinary.vt = VT_NULL;
			pParametor->Value = vBinary;
		}
		else
		{
			vBinary.vt = VT_ARRAY|VT_UI1;
			rarrayBound[0].lLbound = 0;
			rarrayBound[0].cElements = nSize;
			pArray = SafeArrayCreate(VT_UI1, 1, rarrayBound);

			for (long n = 0; n < nSize; ++n )
			{
				SafeArrayPutElement(pArray, &n, &(pValue[n]) );
			}
			vBinary.parray = pArray;
			pParametor->AppendChunk(vBinary);
		}
	} catch (_com_error &e) {
		dump_com_error(e);
	}
	return;
}

void CAdo::UpdateNullParameter(IN TCHAR* tszName)
{
	if(!m_IsSuccess) return;

	m_tstrCommand = _T("UpdateNullParameter(null)");
	m_tstrParameterName = tszName;
	try	{
		m_pCommand->Parameters->GetItem(tszName)->Value.ChangeType(VT_NULL);
	} catch (_com_error &e) {
		dump_com_error(e);
	}

	return;
}

void CAdo::UpdateParameter(IN TCHAR* tszName, IN TCHAR* pValue, IN int nSize)
{
	if(!m_IsSuccess) return;

	_ASSERTE(nSize > 0 && "not allow 0 size!!");

	m_tstrCommand = _T("UpdateParameter(TCHAR)");
	m_tstrParameterName = tszName;
	try	{
		_variant_t vValue(pValue);

		if(NULL == pValue)
		{	
			vValue.vt = VT_NULL;
		}
		
		m_pCommand->Parameters->GetItem(tszName)->Size = nSize;
		m_pCommand->Parameters->GetItem(tszName)->Value = vValue;
	} catch (_com_error &e) {
		dump_com_error(e);
	}

	return;
}

void CAdo::UpdateParameter(IN TCHAR* tszName, IN BYTE* pValue, IN int nSize)
{
	if(!m_IsSuccess) return;

	_ASSERTE(nSize > 0 && "not allow 0 size!!");

	m_tstrCommand = _T("UpdateParameter(binary)");
	m_tstrParameterName = tszName;
	try	{
		_ParameterPtr pParametor(m_pCommand->Parameters->GetItem(tszName));
		pParametor->Size = nSize;

		_variant_t vBinary;
		SAFEARRAY FAR *pArray = NULL;
		SAFEARRAYBOUND rarrayBound[1];

		if(NULL == pValue)
		{
			vBinary.vt = VT_NULL;
			rarrayBound[0].lLbound = 0;
			rarrayBound[0].cElements = 0;
			pParametor->Value = vBinary;
		}
		else
		{
			vBinary.vt = VT_ARRAY|VT_UI1;
			rarrayBound[0].lLbound = 0;
			rarrayBound[0].cElements = nSize;
			pArray = SafeArrayCreate(VT_UI1, 1, rarrayBound);

			for (long n = 0; n < nSize; ++n )
			{
				SafeArrayPutElement(pArray, &n, &(pValue[n]) );
			}
			vBinary.parray = pArray;
			pParametor->AppendChunk(vBinary);
		}
	} catch (_com_error &e) {
		dump_com_error(e);
	}
	return;
}

BOOL CAdo::GetParameter(IN TCHAR* tszName, OUT TCHAR* pValue,IN unsigned int nSize)
{
	if(!m_IsSuccess) return FALSE;
	m_IsSuccess = FALSE;

	m_tstrCommand = _T("GetParameter(TCHAR*)");
	m_tstrParameterName = tszName;
	try	{
		_variant_t vFieldValue = m_pCommand->Parameters->GetItem(tszName)->Value;

		if(VT_NULL == vFieldValue.vt || VT_EMPTY == vFieldValue.vt) {
			m_tstrParameterName += _T(" null value");
			return FALSE;
		} else if(VT_BSTR != vFieldValue.vt) {
			m_tstrParameterName += _T(" nonString Type");
			return FALSE;
		} else if(nSize < _tcslen((TCHAR*)(_bstr_t(vFieldValue.bstrVal)))) {
			m_tstrParameterName += _T(" string size overflow");
			return FALSE;
		}

		//_tcscpy(pValue, (TCHAR*)(_bstr_t)vFieldValue);
		_tcscpy_s(pValue, nSize, (TCHAR*)(_bstr_t)vFieldValue);
	} catch (_com_error &e)	{
		dump_com_error(e);
		return FALSE;
	}
	return m_IsSuccess = TRUE;
}

BOOL CAdo::GetParameter(IN TCHAR* tszName, OUT BYTE* pBuffer, IN int inSize, OUT int& outSize)
{
	if(!m_IsSuccess) return FALSE;
	m_IsSuccess = FALSE;

	m_tstrCommand = _T("GetParameter(binary)");
	m_tstrParameterName = tszName;
	try	{
		_variant_t vFieldValue = m_pCommand->Parameters->GetItem(tszName)->Value;

		if(VT_NULL == vFieldValue.vt) {
			m_tstrParameterName += _T(" null value");
			return FALSE;
		} else if((VT_ARRAY|VT_UI1) != vFieldValue.vt) {
			m_tstrParameterName += _T(" nonbinary type");
			return FALSE;
		}

		int ElementSize = vFieldValue.parray->rgsabound[0].cElements;

		if(ElementSize > inSize || inSize > 8060)
		{
			m_tstrParameterName += _T(" size overflow");
			return FALSE;
		}

		BYTE * pData = NULL;
		SafeArrayAccessData(vFieldValue.parray, (void HUGEP* FAR*)&pData);
		memcpy(pBuffer, pData, ElementSize);
		SafeArrayUnaccessData(vFieldValue.parray);
		outSize = vFieldValue.parray->rgsabound[0].cElements;

	} catch (_com_error &e)	{
		dump_com_error(e);
		return FALSE;
	}
	return m_IsSuccess = TRUE;
}


void CAdo::Close()
{
	if(NULL == m_pConnection) return;

	try
	{
		if(m_pConnection->GetState() != adStateClosed)
			m_pConnection->Close();
	}
	catch(...)
	{

	}
}

BOOL CAdo::Open(CursorLocationEnum CursorLocation)
{
	m_tstrCommand = _T("Open()");
	try
	{
		if(m_nConnectionTimeout != 0)
			m_pConnection->PutConnectionTimeout(m_nConnectionTimeout);

		m_pConnection->CursorLocation = CursorLocation;

		if(!m_tstrProvider.empty())		//ip접속일 경우 Provider 사용
			m_pConnection->put_Provider((_bstr_t)m_tstrProvider.c_str());

		m_pConnection->Open((_bstr_t)m_tstrConnectingString.c_str(), (_bstr_t)m_tstrUserID.c_str(),
			(_bstr_t)m_tstrPassword.c_str(), NULL);

		if(m_pConnection->GetState() == adStateOpen)
			m_pConnection->DefaultDatabase = m_tstrInitCatalog.c_str();

		m_pCommand->ActiveConnection = m_pConnection;
		return TRUE;
	}
	catch(_com_error &e)
	{
		dump_com_error(e);
		return FALSE;
	}

}

void CAdo::Release()
{
	m_pCommand.Release();
	m_pCommand.CreateInstance(__uuidof(Command));
	if(m_pConnection->GetState() != adStateClosed)
		m_pCommand->ActiveConnection = m_pConnection;
}

void CAdo::LOG(TCHAR* format, ...)
{
	TCHAR tszBuffer[1024]={0,};
	va_list ap;
	va_start(ap, format);
	_vstprintf(tszBuffer, format, ap);
	va_end(ap);
	_tprintf(_T("%s\n"), tszBuffer);
}
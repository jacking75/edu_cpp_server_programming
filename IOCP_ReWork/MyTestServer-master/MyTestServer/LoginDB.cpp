#include "LoginDB.h"


bool CLoginDB::LoadDBTable()
{
	return false;
}


CLoginDB::CLoginDB()
	: m_dbType(CAdoOleDB::ACCOUNT_DB)
{
}


CLoginDB::~CLoginDB()
{
}
#pragma once
#include "AdoOleDB.h"

class CLoginDB :
	public CAdoOleDB
{
public:
	bool LoadDBTable() override;

public:
	CLoginDB();
	virtual ~CLoginDB();
};


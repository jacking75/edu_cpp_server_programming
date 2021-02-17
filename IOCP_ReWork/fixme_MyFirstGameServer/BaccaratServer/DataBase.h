// DataBase.h: interface for the CDataBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATABASE_H__C48F6BBB_4234_4F02_8A49_9525A59AB987__INCLUDED_)
#define AFX_DATABASE_H__C48F6BBB_4234_4F02_8A49_9525A59AB987__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define UNICODE
#define _UNICODE
#define DBINITCONSTANTS
#define INITGUID

#include <oledb.h>

#define	DBBUFEMPTY		NULL
#define	DBRINGBUFSIZE	8192
#define	MAXROW			10

const char IDBInitialize_Err[] = "Failed to get IDBInitialize Interface";
const char IDBProperties_Err[] = "Failed to get IDBProperties Interface";
const char Set_Initializetion_Err[] = "Failed to set Initializetion properties";
const char ConnectionErr[] = "Problem in establishing connection to the data source";
const char InitSessionAndCommand_Err[] = "Failed InitSessionAndCommand()";
const char IDBCreateSession_Err[] = "Failed to obtain IDBCreateSession Interface";
const char CreateSession_Err[] = "m_pIDBCreateSession->CreateSession Failed";
const char ICommand_Err[] = "Failed to access ICommand Interface";
const char strErrGetGameServerInfo1[] = "Failed GetGameServerInfo - SetCommandText";
const char strErrGetGameServerInfo2[] = "Failed to execute GetGameServerInfo command";
const char strErrGameServerInfoResultSet1[] = "GameServerInfoResultSet() - Failed to get IColumnsInfo Interface";
const char strErrGameServerInfoResultSet2[] = "GameServerInfoResultSet() - Failed to obtain IAccessor Interface";
const char strErrGetUserLevelBoundary1[] = "Failed GetUserLevelBoundary - SetCommandText";
const char strErrGetUserLevelBoundary2[] = "Failed to execute GetGameServerInfo command";
const char strErrUserLevelBoundaryResultSet1[] = "Failed to get IColumnsInfo Interface - UserLevelBoundaryResultSet";
const char strErrUserLevelBoundaryResultSet2[] = "Failed to obtain IAccessor Interface - UserLevelBoundaryResultSet";




class CDataBase  
{

private:
	
	//CRITICAL_SECTION		m_cs;
	//DATABASEDATA			m_dbDataRingBuf[ DBRINGBUFSIZE ];

	int						m_iBegin, m_iEnd, m_iRestCount;
	//HANDLE				m_hSemaphore;

private:
	inline void ReConnectionAndInitDB();
	int GetDBUserAvatarInfo(const char* szID, int UserIndex);
	int UserAvatarResult( const int UserIndex );
	inline void UserItemResultSet( int iIndex );
	inline void NotifyLogSuccess( int index );
	inline void NotifyLogFaild( char cResult, int index );
	int CheckPWDResultSet( const char* szID, int iIndex );
	inline int CheckPWD( int iIndex );
	inline int UserInfoResultSet( int iIndex );
	inline int GetDBUserInfo( const char* szID, int iIndex );
	int InitSessionAndCommand();
	void UserLevelBoundaryResultSet();
	void GameServerInfoResultSet();
	inline void SetDBBInding( ULONG lNumCols, ULONG& ConsumerBufColOffSel, DBBINDING* pBiding );
	void InitializeAndEstablishConnection();
	
	IDBInitialize*		m_pIDBInitialize;
	IDBProperties*		m_pIDBProperties;
	IDBCreateSession*	m_pIDBCreateSession;
	IDBCreateCommand*	m_pIDBCreateCommand;
	ICommandText*		m_pICommandText;
	IRowset*			m_pIRowset;
	IColumnsInfo*		m_pIColumnsInfo;
	DBCOLUMNINFO*		m_pDBColumnInfo;
	IAccessor*			m_pIAccessor;
	DBPROP				m_InitProperties[4];
	DBPROPSET           m_rgInitPropSet[1];
	WCHAR*				m_pStringsBuffer;
	BYTE*				m_pBuffer;
	DBBINDING*			m_pBinding;
	HACCESSOR			m_hAccessor;
	HROW				m_hRows[MAXROW];
	HROW*				m_pRows;
	
	
public:
	int ThreeiEncrypt( char* buf, const char* key, int len );
	inline void StroageCurUserNumInServer();
	inline void UserLevelChangeCheck( LPSOCKETCONTEXT lpSockContext );
	void GetDBUserItem( const char* szID, int iIndex );
	void SaveUserData( LPSOCKETCONTEXT lpSC, BOOL IsDisConnect = FALSE );

	int ThreeiDecrypt(char* buf, const char* key, int len);

	CDataBase();
	virtual ~CDataBase();

	void InitDataBase();
	void GetUserLevelBoundary();
	void GetGameServerInfo();	
	
	//void DataBaseBufDequeue( LPSOCKETCONTEXT* lpSockContext, char** cpPacket );
	//void DataBaseBufEnqueue( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	
	//friend unsigned int __stdcall DataBaseTProc( void* pParam );

	friend int OnRequestLoginDB( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	friend int OnRequestLogoutDB( LPSOCKETCONTEXT lpSockContext, char* cpPacket );
	
	
};

int InitDataBaseLayer();
//int DBQuery( char idLen, char* id, char pwdLen, char* pwd, USERINFO& userinfo, GAMESTATEMENT& gamest );

#endif // !defined(AFX_DATABASE_H__C48F6BBB_4234_4F02_8A49_9525A59AB987__INCLUDED_)

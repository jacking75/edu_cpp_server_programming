#pragma once

#include <string>

#include "../odbcLib.h"

const INT32 RESULT_OK = 0;

const INT32 MAX_USERID = 17;
const INT32 MAX_USERPW = 17;

const INT32 		MAX_SHORTQUERY = 300;
const INT32 		MAX_LONGQUERY = 1024;
char		g_szQueryShortStr[MAX_SHORTQUERY];	// 작은 크기의 쿼리문
char		g_szQueryLongStr[MAX_LONGQUERY];	// 큰 크기의 쿼리문


MyODBCLib::DBWorker* CreateDBWorker(std::string odbcName, std::string userID, std::string userPW )
{
	auto pDBWorker = new MyODBCLib::DBWorker;
	INT32  nRet = pDBWorker->Connect(3, odbcName.c_str(), userID.c_str(), userPW.c_str());

	if (false == nRet)
	{
		printf("SYSTEM | cLoginDBIocpServer::OnAccept() | cLoginQuery이 ODBC연결에 실패 ");
		return nullptr;
	}

	return pDBWorker;
}


/*
--------------- Dummy 조회 -----------------------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go

CREATE PROCEDURE SP_SELECT_DUMMY
AS
BEGIN
SELECT * FROM Dummy
END
GO
--------------------------------------------------------------
*/
bool selectDummy(MyODBCLib::DBWorker* dbWorker)
{
	// !!! 커서를 자동으로 Clear 하기 위해 SELECT를 하는 경우는 꼭 이것을 선언해야 된다.
	MyODBCLib::QueryCleaner cleaner(dbWorker);

	printf("SYSTEM | cGameServer::SelectDummy() | EXEC SP_SELECT_DUMMY 시작");

	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "EXEC SP_SELECT_DUMMY");
	if (dbWorker->Exec(g_szQueryShortStr) == false)
	{
		printf("SYSTEM | cGameServer::SelectDummy() | EXEC SP_SELECT_DUMMY 쿼리 실패");
		return false;
	}

	return true;
}



/*
-- 유저 인증 ------------------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go


ALTER PROCEDURE [dbo].[SP_USER_LOGIN_AUTHE]
@id varchar(16)
AS
BEGIN
SELECT UserKey, PassWord, JuminNumber, AdminLevel FROM Users WHERE ID=@id
END

Exec USER_LOGIN_AUTHE 'jacking'
-- -----------------------------------------
*/
char certifyLogin(MyODBCLib::DBWorker* dbWorker, const char* pcID, const char* pcPW)
{
	// !!! 커서를 자동으로 Clear 하기 위해 SELECT를 하는 경우는 꼭 이것을 선언해야 된다.
	MyODBCLib::QueryCleaner cleaner(dbWorker); 

	char szPW[MAX_USERPW];

	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "EXEC SP_USER_LOGIN_AUTHE '%s'", pcID);
	if (dbWorker->Exec(g_szQueryShortStr) == false)
	{
		printf("SYSTEM | cLoginServer::funcCertifyLogin() | EXEC spSelectAccountLoginInfo 쿼리 실패");
		return 11;
	}

	SQLRETURN retcode = dbWorker->Fetch();
	if (SQL_NO_DATA == retcode || -1 == retcode) {
		// ID가 존재하지 않을때
		return 12;
	}

	//UKey를 얻어온다.
	INT64 i64UKey = dbWorker->GetInt64(1);

	if (i64UKey <= 0)
	{
		printf("SYSTEM | cLoginQuery::funcCertifyLogin() | 잘못된 (%I64d)UKey 리턴 실패", i64UKey);
		return 14;
	}

	if (dbWorker->GetStr(2, szPW) == false)	// PassWord가 리턴되지 않았을 경우..
	{
		printf("SYSTEM | cLoginQuery::funcCertifyLogin() | 패스워드 리턴 실패");
		return 15;
	}

	// 패스워드 일치 확인
	if (strcmp(szPW, pcPW) != 0) {
		return 16;
	}

	//char	acUserID[MAX_USERID];				// ID
	char	acJuminNumber[18] = { 0, }; // 주민등록번호	
	if (dbWorker->GetStr(3, acJuminNumber) == false)	// 주민번호가 리턴되지 않았을 경우..
	{
		printf("SYSTEM | cLoginQuery::funcCertifyLogin() | 주민번호 리턴 실패");
		return 17;
	}

	char	cAdminLevel = dbWorker->GetInt(4);
	
	//아이디가 확인되었다면
	return RESULT_OK;
}


/*
-- - 스킬 로드------------------
CREATE PROCEDURE SP_LOAD_SKILL
AS
BEGIN
SELECT* FROM Skill
END
GO

Exec SP_LOAD_SKILL
---------------------------------- -
*/
bool funcLoadSkill()
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);


	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "EXEC SP_LOAD_SKILL");
	if (m_pDBConn->Exec(g_szQueryShortStr) == false)
	{
		printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcLoadSkill() | EXEC SP_LOAD_SKILL 쿼리 실패");
		return false;
	}


	DTG_PACKET_LOAD_SKILL_AQ tMDBPkt;
	int iCount = 0;

	while (true)
	{
		SQLRETURN retcode = m_pDBConn->Fetch();
		if (SQL_NO_DATA == retcode || -1 == retcode) {
			break;
		}

		// DB 데이터를 인코딩 한다.
		tMDBPkt.asSkillCd[iCount] = m_pDBConn->GetInt(SKILL_SKILLCD);
		tMDBPkt.cArrUseSex[iCount] = m_pDBConn->GetInt(SKILL_USESEX);
		tMDBPkt.aiUsePoint[iCount] = m_pDBConn->GetInt(SKILL_USEPOINT);
		tMDBPkt.cArrIsOneTarget[iCount] = m_pDBConn->GetInt(SKILL_ISONETARGET);
		tMDBPkt.asDistance[iCount] = m_pDBConn->GetInt(SKILL_DISTANCE);

		++iCount;

		// 지정된 개수가 되면 보낸다.
		if (MAX_LOAD_SKILL_COUNT == iCount) {
			tMDBPkt.iCount = iCount;
			SendBuffer((char*)& tMDBPkt, tMDBPkt.usLength);

			iCount = 0;
		}
	}

	if (iCount > 0)
	{
		tMDBPkt.iCount = iCount;
		SendBuffer((char*)& tMDBPkt, tMDBPkt.usLength);
	}

	return true;
}


/*
-- 캐릭터 정보 얻기 -----------------------
ALTER PROCEDURE [dbo].[SP_CHARACTER_INFO]
@id varchar(16)
AS
BEGIN
SELECT * FROM Characters WHERE ID=@id
END

Exec SP_CHARACTER_INFO 'jacking'
-------------------------------------------
*/
bool funcGetCharacterInfo(const GTD_PACKET_CHARACTER_INFO_RQ* ptDBReq)
{
	cMonitor::Owner lock(this);

	cQuery::Owner Clear(m_pDBConn);
	CHECK_START
		PREPARE_SENDPACKET(DTG_PACKET_CHARACTER_INFO_AQ, DTG_CHARACTER_INFO_AQ, ptDBRet);
	ptDBRet->cResult = RESULT_OK;
	ptDBRet->i64UKey = ptDBReq->i64UKey;
	ptDBRet->dwGameConnectKey = ptDBReq->dwGameConnectKey;
	ptDBRet->sChannelNum = ptDBReq->sChannelNum;


	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "EXEC SP_CHARACTER_INFO '%s'", ptDBReq->acID);
	if (m_pDBConn->Exec(g_szQueryShortStr) == false)
	{
		printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcCertifyLogin() | EXEC SP_CHARACTER_INFO 쿼리 실패");
		CHECK_ERROR(91)
	}

	SQLRETURN retcode = m_pDBConn->Fetch();
	if (SQL_NO_DATA == retcode || -1 == retcode) { // 캐릭터가 존재하지 않을때
		CHECK_ERROR(CHARACTER_INFO_RET_NONE_CHARACTER)
	}


	ptDBRet->i64CharCd = m_pDBConn->GetInt64(CHARACTER_CHARCD);
	m_pDBConn->GetStr(CHARACTER_NAME, ptDBRet->acName);
	ptDBRet->cSex = m_pDBConn->GetInt(CHARACTER_SEX);
	ptDBRet->sLv = m_pDBConn->GetInt(CHARACTER_LV);
	ptDBRet->iExp = m_pDBConn->GetInt(CHARACTER_EXP);
	ptDBRet->iPopularity = m_pDBConn->GetInt(CHARACTER_POPULARITY);
	ptDBRet->i64Cash = m_pDBConn->GetInt64(CHARACTER_CASH);
	ptDBRet->i64GameMoney = m_pDBConn->GetInt64(CHARACTER_GAMEMONEY);
	ptDBRet->cBloodType = m_pDBConn->GetInt(CHARACTER_BLOODTYPE);

	ptDBRet->sMaxWearItemCount = m_pDBConn->GetInt(CHARACTER_MAXWEARITEMCOUNT);
	ptDBRet->sMaxGameItemCount = m_pDBConn->GetInt(CHARACTER_MAXGAMEITEMCOUNT);
	ptDBRet->sMaxStageItemCount = m_pDBConn->GetInt(CHARACTER_MAXSTAGEITEMCOUNT);
	ptDBRet->sMaxArrangementItemCount = m_pDBConn->GetInt(CHARACTER_MAXARRANGEMENTITEMCOUNT);

	ptDBRet->i64CapCharItemCd = m_pDBConn->GetInt64(CHARACTER_CAP);
	ptDBRet->i64HeadCharItemCd = m_pDBConn->GetInt64(CHARACTER_HEAD);
	ptDBRet->i64FaceCharItemCd = m_pDBConn->GetInt64(CHARACTER_FACE);
	ptDBRet->i64CoatCharItemCd = m_pDBConn->GetInt64(CHARACTER_COAT);
	ptDBRet->i64ArmCharItemCd = m_pDBConn->GetInt64(CHARACTER_ARM);
	ptDBRet->i64HandCharItemCd = m_pDBConn->GetInt64(CHARACTER_HAND);
	ptDBRet->i64PantCharItemCd = m_pDBConn->GetInt64(CHARACTER_PANT);
	ptDBRet->i64LegCharItemCd = m_pDBConn->GetInt64(CHARACTER_LEG);
	ptDBRet->i64ShoeCharItemCd = m_pDBConn->GetInt64(CHARACTER_SHOE);
	ptDBRet->i64SetsCharItemCd = m_pDBConn->GetInt64(CHARACTER_SETs);
	ptDBRet->i64EyeCharItemCd = m_pDBConn->GetInt64(CHARACTER_EYE);
	ptDBRet->i64BackCharItemCd = m_pDBConn->GetInt64(CHARACTER_BACK);
	ptDBRet->i64CarriageCharItemCd = m_pDBConn->GetInt64(CHARACTER_CARRIAGE);
	ptDBRet->i64PetCharItemCd = m_pDBConn->GetInt64(CHARACTER_PET);

	ptDBRet->cGoodFeelCardRecvDay = m_pDBConn->GetInt(CHARACTER_GOODFEELCARDRECVDAY);
	ptDBRet->sGoodFeelCardCount = m_pDBConn->GetInt(CHARACTER_GOODFEELCARDCOUNT);

	ptDBRet->i64Eye2CharItemCd = m_pDBConn->GetInt64(CHARACTER_EYE2);
	ptDBRet->i64EarCharItemCd = m_pDBConn->GetInt64(CHARACTER_EAR);
	ptDBRet->i64RightHandCharItemCd = m_pDBConn->GetInt64(CHARACTER_RIGHTHAND);
	ptDBRet->i64LeftHandCharItemCd = m_pDBConn->GetInt64(CHARACTER_LEFTHAND);
	ptDBRet->i64WaistCharItemCd = m_pDBConn->GetInt64(CHARACTER_WAIST);

	ptDBRet->TotalConnectedTime = m_pDBConn->GetInt(CHARACTER_TOTAL_CONNECT_TIME);

	SendPost(ptDBRet->usLength);
	return true;

CHECK_ERR:
	ptDBRet->cResult = __cResult;
	SendPost(ptDBRet->usLength);
	return false;
}

/*
-------------------- 캐릭터 상세 정보 생성 -------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go

ALTER PROCEDURE [dbo].[SP_CREATE_CHARACTER_DETAIL_INFO]
@CharCd		bigint,
@District2		smallint,
@Personality		smallint,
@GoodPoint		smallint,
@WeekPoint		smallint,
@Charms		smallint,
@LikeColor		smallint,
@Alcoholic		smallint,
@Smoking		smallint,
@WeekendWork		smallint,
@AssuranceBodyPart	smallint,
@FirstLove		smallint,
@RespectPerson	smallint,
@MerriageAge		smallint,
@WantedAge		smallint,
@WantedSex		smallint,
@WantedBlood		smallint,
@WantedInterest	smallint,
@WantedDistrict2	smallint,
@WantedPersonality	smallint,
@WantedCharms		smallint,
@WantedAlcoholic	smallint,
@WantedSmoking	smallint,
@WantedWeekPoint	smallint,
@Interest			smallint
AS
BEGIN

DECLARE @ErrMsg			nvarchar( 255 ),
		@ErrCode		int,
		@RowCount		int

	--초기화
	SELECT @ErrCode = 0
	SET @RowCount = 0

	-- 이미있는지체크한다.
	EXEC sp_executesql	N'SELECT @b = COUNT(CharCd) FROM CharDetailInfo WHERE CharCd = @a',
						N'@a bigint,@b int output', @CharCd, @RowCount OUTPUT
	IF  @@ERROR <> 0
	BEGIN
		SELECT	@ErrMsg = 'Failed Execute : SELECT COUNT(CharCd) FROM CharDetailInfo WHERE CharCd ('+ CONVERT( nvarchar(10), @CharCd ) + ')',
				@ErrCode = 50801;
		RETURN @ErrCode;
	END

	-- 이미있다면
	IF( @RowCount > 0 )
		RETURN 99;


--캐릭터메인정보를넣는다.
EXEC sp_executesql N'INSERT INTO CharDetailInfo VALUES( @s_CharCd, @s_District2, @s_Personality, @s_GoodPoint, 
@s_WeekPoint, @s_Charms, @s_LikeColor, @s_Alcoholic, @s_Smoking,
@s_WeekendWork, @s_AssuranceBodyPart, @s_FirstLove, @s_RespectPerson,
@s_MerriageAge, @s_WantedAge, @s_WantedSex, @s_WantedBlood,
@s_WantedInterest, @s_WantedDistrict2, @s_WantedPersonality,
@s_WantedCharms, @s_WantedAlcoholic, @s_WantedSmoking, @s_WantedWeekPoint,
@s_Interest ); ',
N'@s_CharCd bigint, @s_District2 smallint, @s_Personality smallint, @s_GoodPoint smallint, 
@s_WeekPoint smallint, @s_Charms smallint, @s_LikeColor smallint, @s_Alcoholic smallint, @s_Smoking smallint,
@s_WeekendWork smallint, @s_AssuranceBodyPart smallint, @s_FirstLove smallint, @s_RespectPerson smallint,
@s_MerriageAge smallint, @s_WantedAge smallint, @s_WantedSex smallint, @s_WantedBlood smallint,
@s_WantedInterest smallint, @s_WantedDistrict2 smallint, @s_WantedPersonality smallint,
@s_WantedCharms smallint, @s_WantedAlcoholic smallint, @s_WantedSmoking smallint,
@s_WantedWeekPoint smallint, @s_Interest smallint', 
@CharCd, @District2, @Personality, @GoodPoint,
@WeekPoint, @Charms, @LikeColor, @Alcoholic, @Smoking,
@WeekendWork, @AssuranceBodyPart, @FirstLove, @RespectPerson,
@MerriageAge, @WantedAge, @WantedSex, @WantedBlood,
@WantedInterest, @WantedDistrict2, @WantedPersonality,
@WantedCharms, @WantedAlcoholic, @WantedSmoking, @WantedWeekPoint, @Interest
IF  @@ERROR < > 0
BEGIN
SELECT	@ErrMsg = 'Failed Execute : INSERT INTO CharDetailInfo CharCd :  (' + CONVERT(nvarchar(10), @CharCd) + ')',
	@ErrCode = 50501;
GOTO ERROR_HANDLER;
END


RETURN @ErrCode;

--에러통보
ERROR_HANDLER :
RAISERROR @ErrCode @ErrMsg
RETURN @ErrCode;

END


DECLARE @ret	int
EXEC @ret = SP_CREATE_CHARACTER_DETAIL_INFO 2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24
SELECT @ret
--------------------------------------------------------------------------
*/
bool cGameServer::funcCreateCharacterDetailInfo(const GTD_PACKET_CREATE_CHAR_DETAIL_RQ* ptMDBReq)
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);

	CHECK_START
		PREPARE_SENDPACKET(DTG_PACKET_CREATE_CHAR_DETAIL_AQ, DTG_CREATE_CHAR_DETAIL_AQ, ptMDBRet);
	ptMDBRet->i64CharCd = ptMDBReq->i64CharCd;
	ptMDBRet->iGameConnectKey = ptMDBReq->iGameConnectKey;
	ptMDBRet->cResult = RESULT_OK;


	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY,
		"{ ? = call SP_CREATE_CHARACTER_DETAIL_INFO( %I64d, %d, %d, %d, %d, %d, %d, %d, %d, \
		%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) }",
		ptMDBReq->i64CharCd,
		ptMDBReq->sDistrict2, ptMDBReq->cPersonality, ptMDBReq->cGoodPoint, ptMDBReq->cWeekPoint,
		ptMDBReq->cCharms, ptMDBReq->cLikeColor, ptMDBReq->cAlcoholic, ptMDBReq->cSmoking,
		ptMDBReq->cWeekendWork, ptMDBReq->cAssuranceBodyPart, ptMDBReq->cFirstLove, ptMDBReq->cRespectPerson,
		ptMDBReq->cMerriageAge, ptMDBReq->cWantedAge, ptMDBReq->cWantedSex, ptMDBReq->cWantedBlood,
		ptMDBReq->cWantedInterest, ptMDBReq->sWantedDistrict2, ptMDBReq->cWantedPersonality,
		ptMDBReq->cWantedCharms, ptMDBReq->cWantedAlcoholic, ptMDBReq->cWantedSmoking, ptMDBReq->cWantedWeekPoint,
		ptMDBReq->cInterest);

	int nRet = m_pDBConn->ExecGetReturn(g_szQueryShortStr);
	if (RESULT_OK != nRet)
	{
		if (CREATE_CHAR_DETAIL_ALREADY == nRet)
		{
			CHECK_ERROR(CREATE_CHAR_DETAIL_ALREADY)
		}
		else
		{
			printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcCreateCharacterDetailInfo() |  Result(%d), CharCd(%I64d), 쿼리실패",
				nRet, ptMDBReq->i64CharCd);
			CHECK_ERROR(97)
		}
	}


	ptMDBRet->sDistrict2 = ptMDBReq->sDistrict2;
	ptMDBRet->cPersonality = ptMDBReq->cPersonality;
	ptMDBRet->cGoodPoint = ptMDBReq->cGoodPoint;
	ptMDBRet->cWeekPoint = ptMDBReq->cWeekPoint;
	ptMDBRet->cCharms = ptMDBReq->cCharms;
	ptMDBRet->cLikeColor = ptMDBReq->cLikeColor;
	ptMDBRet->cAlcoholic = ptMDBReq->cAlcoholic;
	ptMDBRet->cSmoking = ptMDBReq->cSmoking;
	ptMDBRet->cWeekendWork = ptMDBReq->cWeekendWork;
	ptMDBRet->cAssuranceBodyPart = ptMDBReq->cAssuranceBodyPart;
	ptMDBRet->cFirstLove = ptMDBReq->cFirstLove;
	ptMDBRet->cRespectPerson = ptMDBReq->cRespectPerson;
	ptMDBRet->cMerriageAge = ptMDBReq->cMerriageAge;
	ptMDBRet->cWantedAge = ptMDBReq->cWantedAge;
	ptMDBRet->cWantedSex = ptMDBReq->cWantedSex;
	ptMDBRet->cWantedBlood = ptMDBReq->cWantedBlood;
	ptMDBRet->cWantedInterest = ptMDBReq->cWantedInterest;
	ptMDBRet->sWantedDistrict2 = ptMDBReq->sWantedDistrict2;
	ptMDBRet->cWantedPersonality = ptMDBReq->cWantedPersonality;
	ptMDBRet->cWantedCharms = ptMDBReq->cWantedCharms;
	ptMDBRet->cWantedAlcoholic = ptMDBReq->cWantedAlcoholic;
	ptMDBRet->cWantedSmoking = ptMDBReq->cWantedSmoking;
	ptMDBRet->cWantedWeekPoint = ptMDBReq->cWantedWeekPoint;
	ptMDBRet->cInterest = ptMDBReq->cInterest;

	SendPost(ptMDBRet->usLength);

	return true;
CHECK_ERR:
	ptMDBRet->cResult = __cResult;
	SendPost(ptMDBRet->usLength);
	return false;
}

/*
------------------------------- 캐릭터 상세 정보 변경 ----------------------------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go




ALTER PROCEDURE [dbo].[SP_CHANGE_CHARACTER_DETAIL_INFO]
@CharCd		bigint,
@District2		smallint,
@Personality		smallint,
@GoodPoint		smallint,
@WeekPoint		smallint,
@Charms		smallint,
@LikeColor		smallint,
@Alcoholic		smallint,
@Smoking		smallint,
@WeekendWork	smallint,
@AssuranceBodyPart	smallint,
@FirstLove		smallint,
@RespectPerson	smallint,
@MerriageAge		smallint,
@WantedAge		smallint,
@WantedSex		smallint,
@WantedBlood		smallint,
@WantedInterest	smallint,
@WantedDistrict2	smallint,
@WantedPersonality	smallint,
@WantedCharms	smallint,
@WantedAlcoholic	smallint,
@WantedSmoking	smallint,
@WantedWeekPoint	smallint,
@Interest		smallint
AS
BEGIN

DECLARE @ErrMsg			nvarchar( 255 ),
		@ErrCode		int,
		@RowCount		int

	--초기화
	SELECT @ErrCode = 0
	SET @RowCount = 0


--상세정보를변경한다.
EXEC sp_executesql N'UPDATE CharDetailInfo SET District2=@s_District2, Personality=@s_Personality, GoodPoint=@s_GoodPoint, 
WeekPoint = @s_WeekPoint, Charms = @s_Charms, LikeColor = @s_LikeColor, Alcoholic = @s_Alcoholic, Smoking = @s_Smoking,
WeekendWork = @s_WeekendWork, AssuranceBodyPart = @s_AssuranceBodyPart, FirstLove = @s_FirstLove,
RespectPerson = @s_RespectPerson, MerriageAge = @s_MerriageAge, WantedAge = @s_WantedAge, WantedSex = @s_WantedSex,
WantedBlood = @s_WantedBlood, WantedInterest = @s_WantedInterest, WantedDistrict2 = @s_WantedDistrict2,
WantedPersonality = @s_WantedPersonality, WantedCharms = @s_WantedCharms, WantedAlcoholic = @s_WantedAlcoholic,
WantedSmoking = @s_WantedSmoking, WantedWeekPoint = @s_WantedWeekPoint, Interest = @s_Interest
WHERE CharCd = @s_CharCd; SET @RowCount=@@ROWCOUNT',
N'@s_District2 smallint, @s_Personality smallint, @s_GoodPoint smallint, 
@s_WeekPoint smallint, @s_Charms smallint, @s_LikeColor smallint, @s_Alcoholic smallint, @s_Smoking smallint,
@s_WeekendWork smallint, @s_AssuranceBodyPart smallint, @s_FirstLove smallint, @s_RespectPerson smallint,
@s_MerriageAge smallint, @s_WantedAge smallint, @s_WantedSex smallint, @s_WantedBlood smallint,
@s_WantedInterest smallint, @s_WantedDistrict2 smallint, @s_WantedPersonality smallint,
@s_WantedCharms smallint, @s_WantedAlcoholic smallint, @s_WantedSmoking smallint,
@s_WantedWeekPoint smallint, @s_Interest smallint, @s_CharCd bigint, @RowCount int OUTPUT', 
@District2, @Personality, @GoodPoint,
@WeekPoint, @Charms, @LikeColor, @Alcoholic, @Smoking,
@WeekendWork, @AssuranceBodyPart, @FirstLove, @RespectPerson,
@MerriageAge, @WantedAge, @WantedSex, @WantedBlood,
@WantedInterest, @WantedDistrict2, @WantedPersonality,
@WantedCharms, @WantedAlcoholic, @WantedSmoking, @WantedWeekPoint, @Interest,
@CharCd, @RowCount OUTPUT
IF  @@ERROR < > 0
BEGIN
SELECT	@ErrMsg = 'Failed Execute : UPDATE SET CharDetailInfo CharCd :  (' + CONVERT(nvarchar(10), @CharCd) + ')',
	@ErrCode = 50501;
GOTO ERROR_HANDLER;
END
ELSE IF  @RowCount = 0
BEGIN
SELECT	@ErrMsg = 'Failed Execute : CharDetailInfo Not Exist @CahrCd (' + CONVERT(nvarchar(10), @CharCd) + ')',
	@ErrCode = 91;
GOTO ERROR_HANDLER;
END


RETURN @ErrCode;

--에러통보
ERROR_HANDLER :
RAISERROR @ErrCode @ErrMsg
RETURN @ErrCode;

END



DECLARE @ret	int
EXEC @ret = SP_CHANGE_CHARACTER_DETAIL_INFO 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24
SELECT @ret
------------------------------------------------------------------------------------------------------------------------
*/
bool cGameServer::funcChangeCharacterDetailInfo(const GTD_PACKET_CHANGE_CHAR_DETAIL_RQ* ptMDBReq)
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);

	CHECK_START
		PREPARE_SENDPACKET(DTG_PACKET_OWN_CHAR_DETAIL_AQ, DTG_CHANGE_CHAR_DETAIL_AQ, ptMDBRet);
	ptMDBRet->i64CharCd = ptMDBReq->i64CharCd;
	ptMDBRet->iGameConnectKey = ptMDBReq->iGameConnectKey;
	ptMDBRet->cIsCreated = RESULT_OK;


	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY,
		"{ ? = call SP_CHANGE_CHARACTER_DETAIL_INFO( %I64d, %d, %d, %d, %d, %d, %d, %d, %d, \
		%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d) }",
		ptMDBReq->i64CharCd,
		ptMDBReq->sDistrict2, ptMDBReq->cPersonality, ptMDBReq->cGoodPoint, ptMDBReq->cWeekPoint,
		ptMDBReq->cCharms, ptMDBReq->cLikeColor, ptMDBReq->cAlcoholic, ptMDBReq->cSmoking,
		ptMDBReq->cWeekendWork, ptMDBReq->cAssuranceBodyPart, ptMDBReq->cFirstLove, ptMDBReq->cRespectPerson,
		ptMDBReq->cMerriageAge, ptMDBReq->cWantedAge, ptMDBReq->cWantedSex, ptMDBReq->cWantedBlood,
		ptMDBReq->cWantedInterest, ptMDBReq->sWantedDistrict2, ptMDBReq->cWantedPersonality,
		ptMDBReq->cWantedCharms, ptMDBReq->cWantedAlcoholic, ptMDBReq->cWantedSmoking, ptMDBReq->cWantedWeekPoint,
		ptMDBReq->cInterest);

	int nRet = m_pDBConn->ExecGetReturn(g_szQueryShortStr);
	if (RESULT_OK != nRet)
	{
		if (CHANGE_CHAR_DETAIL_NONE == nRet)
		{
			CHECK_ERROR(CHANGE_CHAR_DETAIL_NONE)
		}
		else
		{
			printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcChangeCharacterDetailInfo() |  Result(%d), CharCd(%I64d), 쿼리실패",
				nRet, ptMDBReq->i64CharCd);
			CHECK_ERROR(97)
		}
	}


	ptMDBRet->sDistrict2 = ptMDBReq->sDistrict2;
	ptMDBRet->cPersonality = ptMDBReq->cPersonality;
	ptMDBRet->cGoodPoint = ptMDBReq->cGoodPoint;
	ptMDBRet->cWeekPoint = ptMDBReq->cWeekPoint;
	ptMDBRet->cCharms = ptMDBReq->cCharms;
	ptMDBRet->cLikeColor = ptMDBReq->cLikeColor;
	ptMDBRet->cAlcoholic = ptMDBReq->cAlcoholic;
	ptMDBRet->cSmoking = ptMDBReq->cSmoking;
	ptMDBRet->cWeekendWork = ptMDBReq->cWeekendWork;
	ptMDBRet->cAssuranceBodyPart = ptMDBReq->cAssuranceBodyPart;
	ptMDBRet->cFirstLove = ptMDBReq->cFirstLove;
	ptMDBRet->cRespectPerson = ptMDBReq->cRespectPerson;
	ptMDBRet->cMerriageAge = ptMDBReq->cMerriageAge;
	ptMDBRet->cWantedAge = ptMDBReq->cWantedAge;
	ptMDBRet->cWantedSex = ptMDBReq->cWantedSex;
	ptMDBRet->cWantedBlood = ptMDBReq->cWantedBlood;
	ptMDBRet->cWantedInterest = ptMDBReq->cWantedInterest;
	ptMDBRet->sWantedDistrict2 = ptMDBReq->sWantedDistrict2;
	ptMDBRet->cWantedPersonality = ptMDBReq->cWantedPersonality;
	ptMDBRet->cWantedCharms = ptMDBReq->cWantedCharms;
	ptMDBRet->cWantedAlcoholic = ptMDBReq->cWantedAlcoholic;
	ptMDBRet->cWantedSmoking = ptMDBReq->cWantedSmoking;
	ptMDBRet->cWantedWeekPoint = ptMDBReq->cWantedWeekPoint;
	ptMDBRet->cInterest = ptMDBReq->cInterest;

	SendPost(ptMDBRet->usLength);

	return true;
CHECK_ERR:
	ptMDBRet->cIsCreated = __cResult;
	SendPost(ptMDBRet->usLength);
	return false;
}


/*
------ 캐릭터 생성 -------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go



ALTER PROCEDURE [dbo].[SP_CREATE_CHARACTER]
@CharCd		bigint OUTPUT,
@ID			varchar(16),
@Name		varchar(20),
@Sex		smallint,
@BloodType	smallint,
@MaxWearItemCount	smallint,
@MaxGameItemCount	smallint,
@MaxStageItemCount	smallint,
@MaxArrangementItemCount	smallint
AS
BEGIN

DECLARE @ErrMsg			nvarchar( 255 ),
		@ErrCode		int,
		@RowCount		int
	--초기화
	SELECT @ErrCode = 0
	SET @RowCount = 0

	--같은 이름의 캐릭터가 있는지 체크 한다.
	EXEC sp_executesql	N'SELECT @b = COUNT(Name) FROM Characters WHERE Name = @a',
						N'@a varchar(20),@b int output', @Name, @RowCount OUTPUT
	IF  @@ERROR <> 0
	BEGIN
		SELECT	@ErrMsg = 'Failed Execute : SELECT COUNT(Name) FROM Characters WHERE Name(' + @Name +')',
				@ErrCode = 50801;
		RETURN @ErrCode;
	END

	--유저 메인 정보가 없다면
	IF( @RowCount > 0 )
		RETURN 99;


	-- 캐릭터 생성 시작
	BEGIN TRAN

--캐릭터 메인 정보를 넣는다.
EXEC sp_executesql N'INSERT INTO Characters VALUES( @b,@c,@d,1,0,0,0,0,@f,@Count1,
@Count2, @Count3, @Count4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ); SET @a = SCOPE_IDENTITY(); ',
N'@a bigint output, @b varchar(16), @c varchar(20), 
@d smallint, @f smallint,
@Count1 smallint, @Count2 smallint, @Count3 smallint, @Count4 smallint',
@CharCd OUTPUT, @ID, @Name, @Sex, @BloodType,
@MaxWearItemCount, @MaxGameItemCount, @MaxStageItemCount, @MaxArrangementItemCount

IF  @@ERROR < > 0
BEGIN
SELECT	@ErrMsg = 'Failed Execute : INSERT INTO Characters UserID : (' + @ID + ')',
@ErrCode = 50501;
GOTO ERROR_HANDLER;
END


COMMIT TRAN
RETURN @ErrCode;

--에러 통보
ERROR_HANDLER :
RAISERROR @ErrCode @ErrMsg
IF @@TRANCOUNT < > 0 ROLLBACK TRAN
RETURN @ErrCode;

END


EXEC @ret = SP_CREATE_CHARACTER 1, 'Test5', '테스트', 1, 2
--------------------------------------------------------------------------

Description : 캐릭터 생성. 캐릭터의 복장 부분은 DB의 기본값을 사용하도록 지정하지 않는다.
*/
bool cGameServer::funcCreateCharacter(GTD_PACKET_CREATE_CHARACTER_RQ* ptDBReq)
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);

	CHECK_START
		PREPARE_SENDPACKET(DTG_PACKET_CREATE_CHARACTER_AQ, DTG_CREATE_CHARACTER_AQ, ptDBRet);
	ptDBRet->i64UKey = ptDBReq->i64UKey;
	ptDBRet->dwGameConnectKey = ptDBReq->dwGameConnectKey;
	ptDBRet->acName[MAX_NICKNAME - 1] = '\0';	strncpy(ptDBRet->acName, ptDBReq->acName, MAX_NICKNAME - 1);

	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "{ ? = call SP_CREATE_CHARACTER( ?, '%s', '%s', %d, %d, %d, %d, %d, %d ) }",
		ptDBReq->acID, ptDBReq->acName, ptDBReq->cSex, ptDBReq->cBloodType,
		MAX_FIRST_WEAR_CHARITEM_COUNT, MAX_FIRST_GAME_CHARITEM_COUNT, MAX_FIRST_STAGE_CHARITEM_COUNT,
		MAX_FIRST_ARRANGE_CHARITEM_COUNT);

	__int64 i64CharCd = 0;
	int nRet = m_pDBConn->ExecGetReturnAndOutput(g_szQueryShortStr, i64CharCd);
	if (RESULT_OK != nRet || 0 == i64CharCd)
	{
		if (GTC_CREATE_CHARACTER_AQ_RESULT_DUPLICATION == nRet)
		{
			CHECK_ERROR(GTC_CREATE_CHARACTER_AQ_RESULT_DUPLICATION)
		}
		else
		{
			printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcCreateCharacter() |  ID(%s), Result(%d), CharCd(%I64d), 쿼리실패",
				ptDBReq->acID, nRet, i64CharCd);
			CHECK_ERROR(97)
		}
	}

	ptDBRet->cResult = RESULT_OK;
	ptDBRet->i64CharCd = i64CharCd;
	ptDBRet->cSex = ptDBReq->cSex;
	ptDBRet->sLv = 1;
	ptDBRet->iExp = 0;
	ptDBRet->iPopularity = 0;
	ptDBRet->i64Cash = 0;
	ptDBRet->i64GameMoney = 0;
	ptDBRet->cBloodType = ptDBReq->cBloodType;

	SendPost(ptDBRet->usLength);
	return true;
CHECK_ERR:
	ptDBRet->cResult = __cResult;
	SendPost(ptDBRet->usLength);
	return false;
}


/*
-------------------------------------- 캐릭터 아이템 삭제 ----------------------------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go

ALTER PROCEDURE [dbo].[SP_DELETE_CHARITEM]
@ItemKind		int,
@CharItemCd		bigint
AS
BEGIN
	SET NOCOUNT ON;

	DECLARE @ErrMsg 		nvarchar( 255 ),
			@ErrCode		int

	SELECT @ErrCode = 0


	IF @ItemKind = 1
	BEGIN
		EXEC sp_executesql	N'DELETE CharItemWear WHERE CharItemCd=@a1',
				N'@a1 bigint', @CharItemCd;
	END
	ELSE IF @ItemKind = 2
	BEGIN
		EXEC sp_executesql	N'DELETE CharItemGame WHERE CharItemCd=@a1',
				N'@a1 bigint', @CharItemCd;
	END
	ELSE IF @ItemKind = 3
	BEGIN
		EXEC sp_executesql	N'DELETE CharItemStage WHERE CharItemCd=@a1',
				N'@a1 bigint', @CharItemCd;
	END
	ELSE
	BEGIN
		EXEC sp_executesql	N'DELETE CharItemArrangement WHERE CharItemCd=@a1',
				N'@a1 bigint', @CharItemCd;
	END


	-- 에러처리
	IF  @@ERROR <> 0
	BEGIN
		SELECT	@ErrMsg = 'Failed Execute DELETE CharItemCd: @CharItemCd(' + CONVERT( nvarchar(10) , @CharItemCd ) +')',
				@ErrCode = 57102;
		GOTO ERROR_HANDLER;
	END

	RETURN @ErrCode;
ERROR_HANDLER:
	RAISERROR @ErrCode @ErrMsg
	RETURN @ErrCode;
END
-----------------------------------------------------------------------------


Description : 캐릭터 아이템 삭제.
*/
bool cGameServer::funcDELETE_CHARITEM_RQ(GTD_PACKET_DELETE_CHARITEM_RQ* ptDBReq)
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);


	// 패킷을 인코딩한다.
	PREPARE_SENDPACKET(DTG_PACKET_DELETE_CHARITEM_AQ, DTG_DELETE_CHARITEM_AQ, ptDBRet);
	ptDBRet->cResult = RESULT_OK;
	ptDBRet->i64CharCd = ptDBReq->i64CharCd;
	ptDBRet->dwGameConnectKey = ptDBReq->dwGameConnectKey;
	ptDBRet->cItemKind = ptDBReq->cItemKind;
	ptDBRet->sItemInvenArrayNum = ptDBReq->sItemInvenArrayNum;
	ptDBRet->i64CharItemCd = ptDBReq->i64CharItemCd;

	int iItemKind = ptDBReq->cItemKind;
	// 캐릭터 아이템을 변경한다.
	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "{ ? = call SP_DELETE_CHARITEM( %d, %I64d ) }", iItemKind, ptDBRet->i64CharItemCd);

	int nRet = m_pDBConn->ExecGetReturn(g_szQueryShortStr);
	if (RESULT_OK != nRet)
	{
		printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcDELETE_CHARITEM_RQ() | CharCd(%I64d), ItemKind(%d), CharItemCd(%I64d)",
			ptDBReq->i64CharCd, iItemKind, ptDBReq->i64CharItemCd);
		ptDBRet->cResult = 97;
	}

	SendPost(ptDBRet->usLength);

	return true;
}


/*
---------------------------------- 캐릭터 아이템 추가 --------------------------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go



ALTER PROCEDURE [dbo].[SP_ADD_CHARITEM]
@CharItemCd		bigint OUTPUT,
@ItemKind		int,
@CharCd			bigint,
@ItemCd			int,
@CurSecondTime	int
AS
BEGIN

DECLARE @ErrMsg			nvarchar( 255 ),
		@ErrCode		int,
		@RowCount		int
	--초기화
	SELECT @ErrCode = 0
	SET @RowCount = 0


	IF @ItemKind = 1
	BEGIN
		EXEC sp_executesql N'INSERT INTO CharItemWear VALUES( @_charcd,@_itemcd,@_gettime, 0, 0 ); SET @a = SCOPE_IDENTITY();',
							N'@a bigint output, @_charcd bigint, @_itemcd int, @_gettime int',
							@CharItemCd OUTPUT, @CharCd, @ItemCd, @CurSecondTime;
	END
	ELSE IF @ItemKind = 2
	BEGIN
		EXEC sp_executesql N'INSERT INTO CharItemGame VALUES( @_charcd,@_itemcd,@_gettime, 0, 0, 0 ); SET @a = SCOPE_IDENTITY();',
							N'@a bigint output, @_charcd bigint, @_itemcd int, @_gettime int',
							@CharItemCd OUTPUT, @CharCd, @ItemCd, @CurSecondTime;
	END
	ELSE IF @ItemKind = 3
	BEGIN
		EXEC sp_executesql N'INSERT INTO CharItemStage VALUES( @_charcd,@_itemcd,@_gettime, 0, 0, 0 ); SET @a = SCOPE_IDENTITY();',
							N'@a bigint output, @_charcd bigint, @_itemcd int, @_gettime int',
							@CharItemCd OUTPUT, @CharCd, @ItemCd, @CurSecondTime;
	END
	ELSE
	BEGIN
		EXEC sp_executesql N'INSERT INTO CharItemArrangement VALUES( @_charcd,@_itemcd,@_gettime, 0, 0, 0, 0, 0, 0 ); SET @a = SCOPE_IDENTITY();',
							N'@a bigint output, @_charcd bigint, @_itemcd int, @_gettime int',
							@CharItemCd OUTPUT, @CharCd, @ItemCd, @CurSecondTime;
	END


	IF  @@ERROR <> 0
	BEGIN
		SELECT	@ErrMsg = 'Failed Execute : INSERT INTO CharItemWear CharCd : ('+ @CharCd + ')',
				@ErrCode = 50507;
		GOTO ERROR_HANDLER;
	END


	RETURN @ErrCode;

-- 에러통보
ERROR_HANDLER:
	RAISERROR @ErrCode @ErrMsg
	RETURN @ErrCode;
END

EXEC SP_ADD_CHARITEM_WEAR 1, 1,1,1,77777
---------------------------------------------------------------------------------------

Description : 아이템 구매.
*/
bool cGameServer::funcBuy_ITEM_RQ(GTD_PACKET_BUY_ITEM_RQ* ptMDBReq)
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);

	int iCurSecondTime = static_cast<int>(g_pGameDBIocpServer->GetCurSecondTime());

	// 패킷을 인코딩한다.
	PREPARE_SENDPACKET(DTG_PACKET_BUY_ITEM_AQ, DTG_BUY_ITEM_AQ, ptMDBRet);
	ptMDBRet->cResult = RESULT_OK;
	ptMDBRet->i64CharCd = ptMDBReq->i64CharCd;
	ptMDBRet->dwGameConnectKey = ptMDBReq->dwGameConnectKey;
	ptMDBRet->cItemKind = ptMDBReq->cItemKind;
	ptMDBRet->iBuyItemCd = ptMDBReq->iBuyItemCd;
	ptMDBRet->iBuyGameMoney = ptMDBReq->iBuyGameMoney;
	ptMDBRet->sItemInvenArrayNum = ptMDBReq->sItemInvenArrayNum;
	ptMDBRet->i64BuyCharItemCd = 0;
	ptMDBRet->iCurSecondTime = iCurSecondTime;

	int iItemKind = ptMDBReq->cItemKind;
	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "{ ? = call SP_ADD_CHARITEM( ?, %d, %I64d, %d, %d ) }",
		iItemKind, ptMDBReq->i64CharCd, ptMDBReq->iBuyItemCd, iCurSecondTime);

	__int64 i64CharItemCd = 0;
	int nRet = m_pDBConn->ExecGetReturnAndOutput(g_szQueryShortStr, i64CharItemCd);
	if (RESULT_OK != nRet || 0 == i64CharItemCd)
	{
		printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcBuy_ITEM_RQ() | CharCd(%I64d), ItemKind(%d), ItemCd(%d) 쿼리실패",
			ptMDBReq->i64CharCd, iItemKind, ptMDBReq->iBuyItemCd);
		ptMDBRet->cResult = 97;
	}
	else
	{
		ptMDBRet->i64BuyCharItemCd = i64CharItemCd;
	}

	SendPost(ptMDBRet->usLength);
	return true;
}


/*
------------------------------------ 호감도 추천 ----------------------------------------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go


ALTER PROCEDURE [dbo].[SP_VOTE_GOODFEEL]
@VoteCharCd			bigint,
@GoodFeelCardCount	int,
@ReceiveCharCd		bigint,
@Popularity 		int,
@CurDay				int
AS
BEGIN
	SET NOCOUNT ON;

	DECLARE @ErrMsg 		nvarchar( 255 ),
			@ErrCode		int

	SELECT @ErrCode = 0

	-- 캐릭터의호감추천표를변경한다.
	EXEC sp_executesql	N'UPDATE Characters SET GoodFeelCardRecvDay=@s_CurDay,
						GoodFeelCardCount=@s_GoodFeelCardCount WHERE CharCd=@s_CharCd',
				N'@s_CurDay int, @s_GoodFeelCardCount int, @s_CharCd bigint',
				@CurDay, @GoodFeelCardCount, @VoteCharCd

	IF  @@ERROR <> 0
		BEGIN
			SELECT @ErrCode = 91;
			GOTO ERROR_HANDLER;
		END

	-- 캐릭터의호감도를변경한다.
	EXEC sp_executesql	N'UPDATE Characters SET Popularity=@s_Popularity WHERE CharCd=@s_CharCd',
				N'@s_Popularity int, @s_CharCd bigint',
				@Popularity, @ReceiveCharCd

	IF  @@ERROR <> 0
		BEGIN
			SELECT @ErrCode = 92;
			GOTO ERROR_HANDLER;
		END


	RETURN @ErrCode;
ERROR_HANDLER:
	RAISERROR @ErrCode @ErrMsg
	RETURN @ErrCode;
END
---------------------------------------------------------------------------------------------------
*/
bool cGameServer::funcVoteGoodFeel_RQ(GTD_PACKET_VOTE_GOODFEEL_RQ* ptMDBReq)
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);


	PREPARE_SENDPACKET(DTG_PACKET_VOTE_GOODFEEL_AQ, DTG_VOTE_GOODFEEL_AQ, ptMDBRet);
	ptMDBRet->cResult = RESULT_OK;
	ptMDBRet->i64VoteCharCd = ptMDBReq->i64VoteCharCd;
	ptMDBRet->dwVoteCharGameConnectKey = ptMDBReq->dwVoteCharGameConnectKey;
	ptMDBRet->sPrevGoodFeelCardCount = ptMDBReq->sPrevGoodFeelCardCount;
	ptMDBRet->sCurGoodFeelCardCount = ptMDBReq->sCurGoodFeelCardCount;
	ptMDBRet->i64ReceiveCharCd = ptMDBReq->i64ReceiveCharCd;
	ptMDBRet->dwReceiveCharGameConnectKey = ptMDBReq->dwReceiveCharGameConnectKey;
	ptMDBRet->iPrevPopularity = ptMDBReq->iPrevPopularity;
	ptMDBRet->iCurPopularity = ptMDBReq->iCurPopularity;
	ptMDBRet->acGiverNickName[MAX_NICKNAME - 1] = '\0'; strncpy(ptMDBRet->acGiverNickName, ptMDBReq->acGiverNickName, MAX_NICKNAME - 1);


	_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "{ ? = call SP_VOTE_GOODFEEL( %I64d, %d, %I64d, %d, %d ) }",
		ptMDBReq->i64VoteCharCd, ptMDBReq->sCurGoodFeelCardCount, ptMDBReq->i64ReceiveCharCd, ptMDBReq->iCurPopularity,
		ptMDBReq->cCurDay);

	int nRet = m_pDBConn->ExecGetReturn(g_szQueryShortStr);
	if (RESULT_OK != nRet)
	{
		printf(LOG_ERROR_NORMAL, "SYSTEM | cGameServer::funcVoteGoodFeel_RQ() | Result(%d), VoteCharCd(%I64d), ReceiveCharCd(%I64d)",
			nRet, ptMDBReq->i64VoteCharCd, ptMDBReq->i64ReceiveCharCd);
		ptMDBRet->cResult = nRet;
	}

	SendPost(ptMDBRet->usLength);

	return true;
}


/*
------------------ 캐릭터 스킬 슬롯 변경 --------------------------------
set ANSI_NULLS ON
set QUOTED_IDENTIFIER ON
go

ALTER PROCEDURE [dbo].[SP_UPDATE_CHAR_SKILL_SLOT]
@CharSkillSlotCd	int,
@SkillCd		int,
@SlotNum		int
AS
BEGIN
	DECLARE 	@ErrCode		int

	SELECT @ErrCode = 0

	EXEC sp_executesql	N'UPDATE CharSkillSlot SET SkillCd=@_SkillCd SlotNum=@_SlotNum WHERE CharSkillSlotCd=@_CharSkillSlotCd',
				N'@_SkillCd int, @_SlotNum int, @_CharSkillSlotCd',
				@SkillCd, @SlotNum, @CharSkillSlotCd

	IF  @@ERROR <> 0
		BEGIN
			SELECT @ErrCode = 95;
		END


	RETURN @ErrCode;
END
----------------------------------------------------------------------
*/
bool cGameServer::funcSaveCharacterSkillSlot(GTD_PACKET_SAVE_CHAR_SKILL_SLOT_RQ* ptMDBReq)
{
	cMonitor::Owner lock(this);
	cQuery::Owner Clear(m_pDBConn);


	int nRet = 0;
	for (int i = 0; i < MAX_CHAR_SKILL_SLOT_COUNT; ++i)
	{
		if (SAVE_CHAR_SKILL_SLOT_DB_OPERATE_NONE == ptMDBReq->cArrDBOperateKind[i]) {
			continue;
		}

		if (SAVE_CHAR_SKILL_SLOT_DB_OPERATE_INSERT == ptMDBReq->cArrDBOperateKind[i])
		{
			_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "{ ? = call SP_INSERT_CHAR_SKILL_SLOT( %I64d, %d, %d ) }",
				ptMDBReq->i64CharCd, ptMDBReq->asSkillCd[i], i);

			nRet = m_pDBConn->ExecGetReturn(g_szQueryShortStr);


		}
		else if (SAVE_CHAR_SKILL_SLOT_DB_OPERATE_UPDATE == ptMDBReq->cArrDBOperateKind[i])
		{
			_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "{ ? = call SP_UPDATE_CHAR_SKILL_SLOT( %d, %d, %d ) }",
				ptMDBReq->aiCharSkillSlotCd[i], ptMDBReq->asSkillCd[i], i);

			nRet = m_pDBConn->ExecGetReturn(g_szQueryShortStr);
		}
		else if (SAVE_CHAR_SKILL_SLOT_DB_OPERATE_DELETE == ptMDBReq->cArrDBOperateKind[i])
		{
			_snprintf(g_szQueryShortStr, MAX_SHORTQUERY, "{ ? = call SP_DELETE_CHAR_SKILL_SLOT( %d ) }",
				ptMDBReq->aiCharSkillSlotCd[i]);

			nRet = m_pDBConn->ExecGetReturn(g_szQueryShortStr);
		}


		if (RESULT_OK != nRet)
		{
			printf(LOG_ERROR_HIGH, "SAVE GAME DATA | cGameServer::funcSaveCharacterSkillSlot() | OperateKind(%d), CharCd(%I64d), " \
				"CharSkillSlotCd(%d), SkillCd(%D), SlotNum(%d)",
				ptMDBReq->cArrDBOperateKind[i], ptMDBReq->i64CharCd, ptMDBReq->aiCharSkillSlotCd[i],
				ptMDBReq->asSkillCd[i], i);
		}
	}

	return true;
}
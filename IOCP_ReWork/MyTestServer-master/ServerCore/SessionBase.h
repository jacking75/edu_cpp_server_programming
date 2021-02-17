#pragma once


///////////////////////////////////////////////////////////
// CSessionBase Class
// Session class는 반드시 해당 클래스를 상속받아야 함
///////////////////////////////////////////////////////////
class CSessionBase
{
public:
	/// 메시지 해석 가능 여부
	virtual void  SetTranslateMessageState(bool state_) = 0;    
};
#pragma once


///////////////////////////////////////////////////////////
// 캐시 오브젝트
///////////////////////////////////////////////////////////
class CCacheObject
{
private:
	__int64 m_key;           ///< 캐시 키
	__int64 m_expireTimer;   ///< 캐시 만료 시간   
	bool    m_bIsCacheWait;      ///< 캐시 여부

public:
	/// 캐시 키를 가지고 온다.
	inline __int64 GetCacheKey() const { return m_key; }

	/// 캐시 유무
	inline bool IsWaitCache() const { return m_bIsCacheWait; }

	/// 캐시할 오브젝트의 초기화
	void Init(__int64 key_, __int64 after_);

	/// 캐시 만료 시간을 가지고 온다
	inline __int64 GetExpireTimer() const { return m_expireTimer; }

	/// 캐시 유무에 대한 상태 세팅
	virtual void SetCacheWaitState(bool is_);

	/// 시간이 만료되어 캐시에서 삭제될 수 있는지에 대한 유무 판단
	virtual bool CheckEnableReleaseCache() const;

public:
	CCacheObject();
	virtual ~CCacheObject();
};
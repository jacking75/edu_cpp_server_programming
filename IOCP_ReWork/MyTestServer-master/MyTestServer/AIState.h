#pragma once
#include <map>
#include <list>

enum E_STATE
{
	IDLE,
	FOLLOW,
	ATTACK,
	RETURN,
	HEAL
};


enum E_STATE_CONDITION
{
	HIT,
	ENCOUNTER,
	ARRIVE,
	DYING,
	WANDER,
	PATROL,
	DEFAULT,
	END,
};


class CAIState;
class CActor;
class CAIStateMachine;
typedef std::list<CAIState*> LIST_FSM_CONDITON;
typedef std::map<E_STATE, LIST_FSM_CONDITON*> MAP_FSM;

///////////////////////////////////////////////////////////
// AIState BaseClass
// (다음 상태)
///////////////////////////////////////////////////////////
class CAIState
{
protected:
	CAIStateMachine* m_pFSM;                           ///< FSM 클래스
	E_STATE m_nextState;                               ///< 다음 상태
	E_STATE_CONDITION m_currentConState;               ///< 현재 조건
	LIST_FSM_CONDITON* m_pCurrentStateConditions;      ///< 현재 상태에 대한 조건별 aiState 맵
	bool m_bIsPlaying;                                 ///< 현재 상태의 AI가 플레이 중인지 여부

public:
	/// FSM 객체 포인터 세팅
	void SetFSM(CAIStateMachine* fsm_);

	/// 현재 AI 플레이 유무 세팅
	void SetPlayState(bool bIs_);

	/// 해당 객체의 조건을 세팅
	void SetCurrentCondition(E_STATE_CONDITION condition_);

	/// 현재의 AI 플레이 유무 가져오기
	inline bool IsPlaying() { return m_bIsPlaying; }

	/// 다음 상태를 가져온다
	inline E_STATE GetNextState() { return m_nextState; }

	/// 해당 스테이트의 조건을 가져온다
	inline E_STATE_CONDITION GetStateCondition() { return m_currentConState; }

	/// 현재 상태에 대한 조건별 aiState 맵을 가져온다
	LIST_FSM_CONDITON const* GetConditions();
	
	/// AI 진입 조건에 맞는지 체크
	virtual bool CheckCondition(CActor const* actor_, CAIState* state_);

	/// AI 진입
	virtual void Enter(CActor* actor_, LIST_FSM_CONDITON const* conditions_);

	/// AI 수행
	virtual bool Execute(CActor* actor_);

	/// AI 수행 후 퇴장
	virtual void Leave(CActor* actor_);

public:
	CAIState(E_STATE nextState_);
	virtual ~CAIState();
};
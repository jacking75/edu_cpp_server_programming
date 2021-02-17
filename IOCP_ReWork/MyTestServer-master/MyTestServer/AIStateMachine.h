#pragma once
#include "AIState.h"
#include <queue>


class CActor;
class CAIState;

///////////////////////////////////////////////////////////
// FSM 클래스
// (수행할 액터)
///////////////////////////////////////////////////////////
class CAIStateMachine
{
public:
	struct _StAIStateEvent {
		E_STATE           state;
		E_STATE_CONDITION condition;
	};
	typedef std::queue<_StAIStateEvent> QUEUE_AI_EVENT;

private:
	MAP_FSM m_stateMachine;        ///< 상태별 조건들에 대한 맵
	CAIState* m_pCurrentState;     ///< 현재 조건의 AIState
	CActor* m_pActor;              ///< 수행 actor
	QUEUE_AI_EVENT m_eventQueue;   ///< 외부에서의 현재 상태에서의 AI 조건 수행 큐

public:
	/// AI 상태 및 조건 등록(상태, 조건, AIState)
	bool RegistState(E_STATE state_, E_STATE_CONDITION condiiton_, CAIState* stateMachine_);

	/// 등록된 내용 중에서 지정한 AIState를 현재 상태로 세팅한다(상태, 조건)
	bool SetCurrentState(E_STATE state_, E_STATE_CONDITION condition_);

	/// 현재 상태를 지정한 AIState의 다음 상태로 세팅
	bool SetNextState(CAIState* state_);

	/// 등록된 내용 중에서 해당 AIState를 삭제한다(상태, 조건)
	bool RemoveState(E_STATE state_, E_STATE_CONDITION condition_);

	/// 현재 AI의 플레이 유무를 세팅
	void SetCurrentPlayState(bool bIs_);

	/// 외부에서 해당 상태의 특정 조건을 강제로 수행하기 위해 이벤트를 삽입
	void PushEvent(_StAIStateEvent const& condition_);

	/// 업데이트
	void Update();

public:
	CAIStateMachine(CActor* actor_);
	~CAIStateMachine();
};
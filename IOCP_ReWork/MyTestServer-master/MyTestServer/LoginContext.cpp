#include "LoginContext.h"
#include "Session.h"
#include "sample.pb.h"
#include "types.pb.h"
#include "ServerBase.h"
#include "ZoneManager.h"
#include "Zone.h"
#include "PC.h"
#include "PCManager.h"
#include "ServerTaskZoneMove.h"

using namespace google;
using namespace packetdef;


bool CLoginContext::MessageProc(CSession* session_, packetdef::PacketID paketID_, protobuf::io::ArrayInputStream& io_)
{
#define DESERIALIZE(p, o) \
	packets::##p req; \
	if (!Deserialize<packets::##p>(req, o)) { \
		return false; \
	}\

	switch (paketID_)
	{
		case PacketID::LoginReq:
		{
			DESERIALIZE(LoginReq, io_)
			SESSION_ID sessionId = m_pServer->RegistSession(session_, req.serialid(), req.id(), req.pass());
			if (sessionId == INVALID_SESSION_ID)
			{
				// 로그
				packets::LoginAck ack;
				ack.set_result(packets::RESULT_FAILED);
				session_->SendImmediately(PacketID::LoginAck, &ack);
				return false;
			}

			CPC* pc = session_->CreatePlayer(static_cast<SERIAL_ID>(sessionId));
			if (pc == NULL)
			{
				packets::LoginAck ack;
				ack.set_result(packets::RESULT_FAILED);
				session_->SendImmediately(PacketID::LoginAck, &ack);
				return false;
			}

			packets::LoginAck ack;
			ack.set_result(packets::RESULT_SUCCESS);
			ack.set_userid(pc->GetUserID());
			ack.set_actorid(pc->GetActorID());
			session_->SendImmediately(PacketID::LoginAck, &ack);
			break;
		}

		case PacketID::CharSelectReq:
		{
			DESERIALIZE(CharSelectReq, io_)

			packets::CharSelectAck ack;
			ack.set_actorid(session_->GetActorID());
			ack.set_userid(session_->GetUserID());
			session_->SendImmediately(PacketID::CharSelectAck, &ack);
			break;
		}

		case PacketID::EnterWorld:
		{
			DESERIALIZE(EnterWorld, io_)
			SERIAL_ID serialId = static_cast<SERIAL_ID>(req.actorid());
			CPC* pc = CPCManager::GetInstance().FindPC(serialId);
			if (pc == NULL)
			{
				// 로그
				return false;
			}

			CZoneManager::GetInstance().EnterZone(pc->GetZoneID(), pc);
			break;
		}

		case PacketID::Move:
		{
			DESERIALIZE(Move, io_)
			USER_ID actorID = req.actorid();
			float x = req.position().x();
			float y = req.position().y();
			float z = req.position().z();

			CPC* pc = session_->GetPlayer();
			if (pc == NULL)
			{
				// 로그
				return false;
			}

			pc->SetPosition(req.position());
			packets::Move nty;
			nty.set_actorid(session_->GetSessionID());
			nty.set_allocated_position(pc->GetPos());
			CZone* zone = pc->GetZone();
			if (zone)
			{
				zone->SendZone(PacketID::Move, &nty);
			}
			break;
		}

		case PacketID::ZoneMoveReq:
		{
			DESERIALIZE(ZoneMoveReq, io_)

			ZONE_ID zoneid = req.zoneid();

			if (session_->GetZoneID() == zoneid)
			{
				// 로그
				packets::ZoneMoveAck ack;
				ack.set_result(packets::RESULT_FAILED);
				session_->SendImmediately(PacketID::ZoneMoveAck, &ack);
				break;
			}

			CZone* zone = CZoneManager::GetInstance().FindZone(zoneid);
			if (zone == NULL)
			{
				// 로그
				packets::ZoneMoveAck ack;
				ack.set_result(packets::RESULT_FAILED);
				session_->SendImmediately(PacketID::ZoneMoveAck, &ack);
				break;
			}

			packets::ZoneMoveAck ack;
			ack.set_result(packets::RESULT_SUCCESS);
			session_->SendImmediately(PacketID::ZoneMoveAck, &ack);

			CServerTaskZoneMove* task = ALLOCATE_POOL_MALLOC(CServerTaskZoneMove, sizeof(CServerTaskZoneMove));
			new(task) CServerTaskZoneMove(session_, zoneid);

			m_pServer->PushTask(task);
			break;
		}

	}

	return true;
}


CLoginContext::CLoginContext(CServerBase * server_)
	: m_pServer(server_)
{
}


CLoginContext::~CLoginContext()
{
}

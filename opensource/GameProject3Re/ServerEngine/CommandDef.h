#pragma once

namespace hbServerEngine
{

#define BEGIN_PROCESS_MESSAGE(ClassName) \
bool ClassName##::DispatchPacket(NetPacket *pNetPacket) \
{ \
	PacketHeader *pPacketHeader = (PacketHeader *)pNetPacket->m_pDataBuffer->GetBuffer();\
	ERROR_RETURN_true(pPacketHeader != NULL);\
	switch(pPacketHeader->dwMsgID) \
	{

#define PROCESS_MESSAGE_ITEM(dwMsgID, Func) \
		case dwMsgID:{\
		CLog::GetInstancePtr()->LogInfo("---Receive Message:[%s]----", #dwMsgID);\
		if(Func(pNetPacket)){return true;}}break;

#define PROCESS_MESSAGE_ITEMEX(dwMsgID, Func) \
		case dwMsgID:{\
		if(Func(pNetPacket)){return true;}}break;

#define END_PROCESS_MESSAGE \
		default: \
			{ return false;} \
			break;\
	}\
	return true;\
}

}



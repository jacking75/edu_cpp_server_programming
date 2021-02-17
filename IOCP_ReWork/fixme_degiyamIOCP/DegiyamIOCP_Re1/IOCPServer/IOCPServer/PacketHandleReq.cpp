
#include "ServiceManager.h"
#include "../../Common/Protocol.h"

using namespace NETWORKLIB;

const BOOL ServiceManager::RecvCS_AUTH_LOGIN_REQ(LIB_SESSIONDATA* pSession)
{
	TCHAR	Buffer[MAX_BUFFER];
	static UINT snCount = 0;	// 이 static 변수도 테스트 때문에 남긴거라서 삭제해도 됩니다

	body_CS_AUTH_LOGIN_REQ* body = (body_CS_AUTH_LOGIN_REQ*)(Buffer + SIZE_HEADER);

	memcpy(&Buffer, &pSession->m_SocketCtx.recvContext->Buffer, sizeof(body_CS_AUTH_LOGIN_REQ) + SIZE_HEADER);

	int id = body->id;
	int pw = body->pw;

	printf("클라이언트로 전송받은 값 id : %d, pw : %d - Count : %d\n", id, pw, ++snCount);

	this->RecvCS_AUTH_LOGIN_ACK(pSession);

	return 1;
}
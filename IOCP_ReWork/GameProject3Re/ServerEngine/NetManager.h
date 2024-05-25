#pragma once

#include "IBufferHandler.h"
#include "Connection.h"
#include "CommonSocket.h"
#include "DataBuffer.h"
#include "PacketHeader.h"

#include <string>
#include <vector>
#include <thread>


namespace hbServerEngine
{
    class CNetManager
    {
        CNetManager(void)
        {
        }

        virtual ~CNetManager(void)
        {
            m_pBufferHandler = nullptr;
        }

    public:
        static CNetManager* GetInstancePtr()
        {
            static CNetManager NetManager;

            return &NetManager;
        }


    public:
        bool Start(uint16_t nPortNum, int32_t nMaxConn, IDataHandler* pBufferHandler, std::string strIpAddr)
        {
            ERROR_RETURN_false(pBufferHandler != NULL);

            m_pBufferHandler = pBufferHandler;

            CConnectionMgr::GetInstancePtr()->InitConnectionList(nMaxConn);

            if (!InitNetwork())
            {
                CLog::GetInstancePtr()->LogError("네트워크를 초기화하지 못했다！！");
                return false;
            }

            if (!CreateCompletePort())
            {
                CLog::GetInstancePtr()->LogError("완료 포트 또는 Epoll을 만들지 못했다！！");
                return false;
            }

            if (!CreateEventThread(0))
            {
                CLog::GetInstancePtr()->LogError("네트워크 이벤트 처리 스레드를 만들지 못했다！！");
                return false;
            }

            if (!StartNetListen(nPortNum, strIpAddr))
            {
                CLog::GetInstancePtr()->LogError("Listen 하지 못함！！");
                return false;
            }

            return true;
        }

        bool Stop()
        {
            StopListen();

            CConnectionMgr::GetInstancePtr()->CloseAllConnection();

            CloseEventThread();

            DestroyCompletePort();

            CConnectionMgr::GetInstancePtr()->DestroyAllConnection();

            UninitNetwork();

            return true;
        }

        bool SendMessageData(int32_t nConnID, int32_t MsgID, uint64_t u64TargetID, uint32_t dwUserData, const char* pData, uint32_t dwLen)
        {
            if (nConnID <= 0)
            {
                return false;
            }

            CConnection* pConn = CConnectionMgr::GetInstancePtr()->GetConnectionByID(nConnID);
            if (pConn == NULL)
            {
                //연결이 실패하여 연결이 끊어졌으며 연결 ID를 사용할 수 없음을 나타낸다
                return false;
            }

            if (pConn->GetConnectStatus() != NET_ST_CONN)
            {
                CLog::GetInstancePtr()->LogError("CNetManager::SendMessageData FAILED, 연결이 끊어졌다, MsgID:%d, nConnID:%d", MsgID, nConnID);
                return false;
            }

            IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(dwLen + sizeof(PacketHeader));
            ERROR_RETURN_false(pDataBuffer != NULL);

            PacketHeader* pHeader = (PacketHeader*)pDataBuffer->GetBuffer();
            pHeader->TotalSize = dwLen + sizeof(PacketHeader);
            pHeader->MsgID = MsgID;
            
            memcpy(pDataBuffer->GetBuffer() + sizeof(PacketHeader), pData, dwLen);

            pDataBuffer->SetTotalLenth(pHeader->TotalSize);

            if (pConn->SendBuffer(pDataBuffer))
            {
                PostSendOperation(pConn);
                return true;
            }

            return false;
        }

        bool SendMessageBuff(int32_t nConnID, IDataBuffer* pBuffer)
        {
            ERROR_RETURN_false(nConnID != 0);
            ERROR_RETURN_false(pBuffer != 0);
            CConnection* pConn = CConnectionMgr::GetInstancePtr()->GetConnectionByID(nConnID);
            if (pConn == nullptr)
            {
                // 연결이 실패하여 연결이 끊어졌으며 연결 ID를 사용할 수 없음을 나타낸다
                return false;
            }

            if (pConn->GetConnectStatus() != NET_ST_CONN)
            {
                CLog::GetInstancePtr()->LogError("CNetManager::SendMessageBuff FAILED, 연결이 끊어졌다, ConnID:%d", nConnID);
                return false;
            }

            pBuffer->AddRef();
            if (pConn->SendBuffer(pBuffer))
            {
                PostSendOperation(pConn);
                return true;
            }

            return false;
        }


        bool InitNetwork()
        {
            return InitSocket();
        }

        bool UninitNetwork()
        {
            return UninitSocket();
        }

        bool StartNetListen(uint16_t nPortNum, std::string strIpAddr)
        {
            sockaddr_in SvrAddr;
            SvrAddr.sin_family = AF_INET;
            SvrAddr.sin_port = htons(nPortNum);

            if (strIpAddr.size() <= 1)
            {
                SvrAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 다중 IP 주소 수신 지원
            }
            else
            {
                SvrAddr.sin_addr.s_addr = IpAddrStrToInt(strIpAddr.c_str());
            }

            m_hListenSocket = CreateSocket(AF_INET, SOCK_STREAM, 0);
            if (m_hListenSocket == INVALID_SOCKET)
            {
                CLog::GetInstancePtr()->LogError("수신 소켓을 만들지 못한 이유:%s!", CommonFunc::GetLastErrorStr(GetSocketLastError()).c_str());
                return false;
            }

            SetSocketBlock(m_hListenSocket, false);

            SetSocketReuseable(m_hListenSocket);

            if (!BindSocket(m_hListenSocket, (sockaddr*)&SvrAddr, sizeof(SvrAddr)))
            {
                CLog::GetInstancePtr()->LogError("Bind 실패 원이:%s!", CommonFunc::GetLastErrorStr(GetSocketLastError()).c_str());
                return false;
            }

            if (!ListenSocket(m_hListenSocket, 20))
            {
                CLog::GetInstancePtr()->LogError("스레드 소켓을 수신하지 못함:%s!", CommonFunc::GetLastErrorStr(GetSocketLastError()).c_str());
                return false;
            }

            if (!WaitConnect())
            {
                CLog::GetInstancePtr()->LogError("연결 실패를 수락하기 위해 대기 중:%s!", CommonFunc::GetLastErrorStr(GetSocketLastError()).c_str());
                return false;
            }

            return true;
        }

        bool StopListen()
        {
            CloseSocket(m_hListenSocket);

            CloseSocket(m_hCurAcceptSocket);

            return true;
        }


        //IOCP 섹션은 다음과 같다.
    public:
        bool CreateCompletePort()
        {
#ifdef WIN32
            m_hCompletePort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, -1);
            ERROR_RETURN_false(m_hCompletePort != NULL);
#else
            m_hCompletePort = epoll_create(10000);
            ERROR_RETURN_false(m_hCompletePort != -1);
#endif

            return true;
        }


        bool DestroyCompletePort()
        {
#ifdef WIN32
            CloseHandle(m_hCompletePort);
#else
            close(m_hCompletePort);
#endif

            return true;
        }


        bool    CreateEventThread(int32_t nNum)
        {
            if (nNum == 0)
            {
                nNum = CommonFunc::GetProcessorNum();
            }

            ERROR_RETURN_false(nNum > 0);

            m_bCloseEvent = false;

            //현재 리눅스는 단일 스레드를 사용한다.
#ifndef WIN32
            nNum = 1;
#endif

            for (int32_t i = 0; i < nNum; ++i)
            {
                std::thread* pThread = new std::thread(&CNetManager::WorkThread_ProcessEvent, this, nNum);
                m_vtEventThread.push_back(pThread);
            }

            return true;
        }


        bool CloseEventThread()
        {
            m_bCloseEvent = true;

            for (std::vector<std::thread*>::iterator itor = m_vtEventThread.begin(); itor != m_vtEventThread.end(); ++itor)
            {
                std::thread* pThread = *itor;
                if (pThread == NULL)
                {
                    continue;
                }

                pThread->join();

                delete pThread;
            }

            m_vtEventThread.clear();

            return true;
        }


#ifdef WIN32
        bool WorkThread_ProcessEvent(int32_t nParam)
        {
            ERROR_RETURN_false(m_hCompletePort != INVALID_HANDLE_VALUE);

            DWORD nNumOfByte = 0;
            ULONG_PTR CompleteKey = 0;
            LPOVERLAPPED lpOverlapped = nullptr;
            DWORD nWaitTime = 1000;
            bool  bRetValue = false;

            while (!m_bCloseEvent)
            {
                bRetValue = GetQueuedCompletionStatus(m_hCompletePort, &nNumOfByte, &CompleteKey, &lpOverlapped, nWaitTime);
                if (!bRetValue)
                {
                    if (lpOverlapped == nullptr)
                    {
                        if (ERROR_ABANDONED_WAIT_0 == GetSocketLastError())
                        {
                            CLog::GetInstancePtr()->LogError("완료 포트 외부 폐쇄!");
                            return false;
                        }

                        if (GetSocketLastError() == WAIT_TIMEOUT)
                        {
                            continue;
                        }

                        if (GetSocketLastError() == ERROR_OPERATION_ABORTED)
                        {
                            return false;
                        }
                    }
                }

                NetIoOperatorData* pIoPeratorData = (NetIoOperatorData*)lpOverlapped;
                switch (pIoPeratorData->nOpType)
                {
                case NET_OP_RECV:
                {
                    CConnection* pConnection = (CConnection*)CompleteKey;
                    if (pConnection == nullptr)
                    {
                        CLog::GetInstancePtr()->LogError("NET_MSG_RECV1, pConnection == NULL");
                        break;
                    }

                    if (nNumOfByte == 0)
                    {
                        // 상대방이 종료되었음을 의미한다
                        if (pConnection->GetConnectionID() != pIoPeratorData->nConnID)
                        {
                            CLog::GetInstancePtr()->LogError("NET_MSG_RECV2, 상대방이 연결을 닫았지만 우리가 더 빠를 수도 있고 연결이 재사용되었을 수도 있다. NewID:%d, OrgID:%d", pConnection->GetConnectionID(), pIoPeratorData->nConnID);
                            break;
                        }
                        pConnection->Close();
                    }
                    else
                    {
                        if (pConnection->GetConnectionID() != pIoPeratorData->nConnID)
                        {
                            CLog::GetInstancePtr()->LogError("NET_MSG_RECV3，데이터가 있지만 연결이 재사용되었다. NewID:%d, OrgID:%d", pConnection->GetConnectionID(), pIoPeratorData->nConnID);
                            break;
                        }

                        if (pConnection->GetConnectStatus() == NET_ST_CONN || pConnection->GetConnectStatus() == NET_ST_WAIT)
                        {
                            if (!pConnection->HandleRecvEvent(nNumOfByte))
                            {
                                pConnection->Close();
                            }
                        }
                        else
                        {
                            CLog::GetInstancePtr()->LogError("심각한 오류, 연결되지 않은 데이터 수신! ConnID:%d", pConnection->GetConnectionID());
                        }
                    }
                }
                break;
                case NET_OP_SEND:
                {
                    pIoPeratorData->pDataBuffer->Release();
                    CConnection* pConnection = (CConnection*)CompleteKey;
                    if (pConnection == NULL)
                    {
                        CLog::GetInstancePtr()->LogError("NET_MSG_SEND, pConnection == NULL。");
                        break;
                    }

                    if (pConnection->GetConnectionID() != pIoPeratorData->nConnID)
                    {
                        CLog::GetInstancePtr()->LogError("NET_MSG_SEND, 하지만 재사용을 위해 연결이 종료되었다.");
                        break;
                    }

                    pConnection->DoSend();
                }
                break;
                case NET_OP_POST:
                {
                    CConnection* pConnection = (CConnection*)CompleteKey;
                    if (pConnection == NULL)
                    {
                        CLog::GetInstancePtr()->LogError("NET_MSG_POST1, pConnection == NULL。");
                        break;
                    }

                    if (pConnection->GetConnectionID() != pIoPeratorData->nConnID)
                    {
                        CLog::GetInstancePtr()->LogError("NET_MSG_POST2, 하지만 재사용을 위해 연결이 종료되었다.");
                        break;
                    }

                    pConnection->DoSend();
                }
                break;
                case NET_OP_CONNECT:
                {
                    CConnection* pConnection = (CConnection*)CompleteKey;
                    if (pConnection == NULL)
                    {
                        CLog::GetInstancePtr()->LogError("NET_MSG_CONNECT, pConnection == NULL。");
                        break;
                    }

                    if (pConnection->GetConnectionID() != pIoPeratorData->nConnID)
                    {
                        CLog::GetInstancePtr()->LogError("NET_MSG_CONNECT, 이벤트 ID와 연결 ID가 일치하지 않는다.");
                        break;
                    }

                    if (bRetValue)
                    {
                        pConnection->SetConnectStatus(NET_ST_CONN);
                        m_pBufferHandler->OnNewConnect(pConnection->GetConnectionID());

                        if (!pConnection->DoReceive())
                        {
                            pConnection->Close();
                        }
                    }
                    else
                    {
                        pConnection->SetConnectStatus(NET_ST_INIT);

                        pConnection->Close();
                    }
                }
                break;
                case NET_OP_ACCEPT:
                {
                    if (m_hCurAcceptSocket == INVALID_SOCKET)
                    {
                        break;
                    }

                    if (!bRetValue && GetSocketLastError() == ERROR_OPERATION_ABORTED)
                    {
                        return false;
                    }

                    sockaddr_in* addrClient = NULL, * addrLocal = NULL;
                    if (!GetSocketAddress(m_hListenSocket, m_AddressBuf, addrClient, addrLocal))
                    {
                        CLog::GetInstancePtr()->LogError("새 연결을 수락하지 못함 원인: GetSocketAddress false Reason:%s!", CommonFunc::GetLastErrorStr(GetSocketLastError()).c_str());
                        break;
                    }

                    SetSocketBlock(m_hCurAcceptSocket, false);
                    //SetSocketNoDelay(m_hCurAcceptSocket);
                    CConnection* pConnection = AssociateCompletePort(m_hCurAcceptSocket, false);
                    if (pConnection != NULL)
                    {
                        pConnection->m_dwIpAddr = addrClient->sin_addr.s_addr;

                        pConnection->SetConnectStatus(NET_ST_CONN);

                        m_pBufferHandler->OnNewConnect(pConnection->GetConnectionID());

                        if (!pConnection->DoReceive())
                        {
                            pConnection->Close();
                        }
                    }
                    else
                    {
                        CLog::GetInstancePtr()->LogError("새 연결을 수락하지 못했습니다 원인: AssociateCompletePort가 최대 연결 수에 도달했거나 바인딩에 실패했다!");
                    }

                    m_IoOverlapAccept.Reset();
                    m_IoOverlapAccept.nOpType = NET_OP_ACCEPT;
                    m_hCurAcceptSocket = CreateSocket();
                    AcceptSocketEx(m_hListenSocket, m_hCurAcceptSocket, (CHAR*)m_AddressBuf, (LPOVERLAPPED)&m_IoOverlapAccept);
                }
                break;
                }
            }

            return true;
}
#else
        bool WorkThread_ProcessEvent(int32_t nParam)
        {
            struct epoll_event vtEvents[1024];
            int nFd = 0;
            while (!m_bCloseEvent)
            {
                nFd = epoll_wait(m_hCompletePort, vtEvents, 1024, 500);
                if (nFd == -1)
                {
                    if (errno != EINTR)
                    {
                        CLog::GetInstancePtr()->LogError("epoll_wait 실패 원인:%s", CommonFunc::GetLastErrorStr(errno).c_str());
                        return false;
                    }

                    continue;
                }

                for (int i = 0; i < nFd; ++i)
                {
                    if (vtEvents[i].data.fd == m_hListenSocket)
                    {
                        sockaddr_in client_addr;
                        socklen_t nLen = sizeof(client_addr);
                        memset(&client_addr, 0, sizeof(client_addr));
                        while (true)
                        {
                            SOCKET hClientSocket = accept(m_hListenSocket, (sockaddr*)&client_addr, &nLen);
                            if (hClientSocket == INVALID_SOCKET)
                            {
                                if (errno != EAGAIN && errno != EINTR)
                                {
                                    CLog::GetInstancePtr()->LogError("새 연결을 수락하지 못함 원인:%s", CommonFunc::GetLastErrorStr(errno).c_str());
                                }

                                break;
                            }

                            SetSocketBlock(hClientSocket, false);
                            CConnection* pConnection = AssociateCompletePort(hClientSocket, false);
                            if (pConnection != NULL)
                            {
                                pConnection->m_dwIpAddr = Con_Addr.sin_addr.s_addr;

                                pConnection->SetConnectStatus(NET_ST_CONN);

                                m_pBufferHandler->OnNewConnect(pConnection->GetConnectionID());
                            }
                            else
                            {
                                CLog::GetInstancePtr()->LogError("새 연결을 수락하지 못했다 원인: 최대 연결 수에 도달했거나 바인딩에 실패했다!");
                                break;
                            }
                        }

                        continue;
                    }
                    CConnection* pConnection = (CConnection*)vtEvents[i].data.ptr;
                    if (pConnection == NULL)
                    {
                        CLog::GetInstancePtr()->LogError("---Invalid pConnection Ptr--!");
                        continue;
                    }

                    INT32 nNeedEvent = EPOLLIN;
                    if (vtEvents[i].events & EPOLLIN)
                    {
                        if (pConnection->GetConnectStatus() == NET_ST_INIT)
                        {
                            pConnection->SetConnectStatus(NET_ST_CONN);
                            m_pBufferHandler->OnNewConnect(pConnection->GetConnectionID());
                        }

                        if (!pConnection->HandleRecvEvent(0))
                        {
                            // 이는 기본적으로 연결이 끊어졌으며 닫을 수 있음을 나타낸다
                            EventDelete(pConnection);
                            pConnection->Close();
                            continue;
                        }
                    }

                    if (vtEvents[i].events & EPOLLOUT)
                    {
                        if (pConnection->GetConnectStatus() == NET_ST_INIT)
                        {
                            pConnection->SetConnectStatus(NET_ST_CONN);
                            m_pBufferHandler->OnNewConnect(pConnection->GetConnectionID());
                        }

                        pConnection->m_IsSending = true;
                        INT32 nRet = pConnection->DoSend();
                        pConnection->m_IsSending = false;
                        if (nRet == E_SEND_ERROR)
                        {
                            EventDelete(pConnection);
                            pConnection->Close();
                            continue;
                        }

                        if (nRet == E_SEND_UNDONE)
                        {
                            nNeedEvent = nNeedEvent | EPOLLOUT;
                        }
                    }

                    if (nNeedEvent != 0)
                    {
                        struct epoll_event EpollEvent;
                        EpollEvent.data.ptr = pConnection;
                        EpollEvent.events = nNeedEvent;
                        if (0 != epoll_ctl(m_hCompletePort, EPOLL_CTL_MOD, pConnection->GetSocket(), &EpollEvent))
                        {
                            CLog::GetInstancePtr()->LogError("socket 이벤트 설정 실패 원인: %s", CommonFunc::GetLastErrorStr(errno).c_str());
                        }
                    }
                }
            }

            return true;
        }
#endif

        bool WorkThread_Listen()
        {
            sockaddr_in Con_Addr;
            socklen_t nLen = sizeof(Con_Addr);

            while (true)
            {
                memset(&Con_Addr, 0, sizeof(Con_Addr));
                SOCKET hClientSocket = accept(m_hListenSocket, (sockaddr*)&Con_Addr, &nLen);
                if (hClientSocket == INVALID_SOCKET)
                {
                    CLog::GetInstancePtr()->LogError("accept 오류 원인:%s!", CommonFunc::GetLastErrorStr(GetSocketLastError()).c_str());
                    return false;
                }
                
                SetSocketBlock(hClientSocket, false);
                //SetSocketNoDelay(hClientSocket);
                
                CConnection* pConnection = AssociateCompletePort(hClientSocket, false);
                if (pConnection != nullptr)
                {
                    pConnection->m_dwIpAddr = Con_Addr.sin_addr.s_addr;

                    pConnection->SetConnectStatus(NET_ST_CONN);

                    m_pBufferHandler->OnNewConnect(pConnection->GetConnectionID());

                    // Windows IOCP 모델에서는 먼저 새 연결을 수신하도록 조정해야 하는 반면, EPOLL 모델에서는 읽기 이벤트만 있으면 이벤트가 도착할 때까지 기다린 다음 읽기 작업을 전송할 수 있다                    
#ifdef WIN32
                    if (!pConnection->DoReceive())
                    {
                        pConnection->Close();
                    }
#endif
                }
                else
                {
                    CLog::GetInstancePtr()->LogError("새 연결을 수락하지 못했다 원인: 최대 연결 수에 도달했거나 바인딩에 실패했다!");
                }
            }

            return true;
        }


#ifdef WIN32
        bool EventDelete(CConnection* pConnection)
        {
            return true;
        }
#else
        bool EventDelete(CConnection* pConnection)
        {
            struct epoll_event delEpv = { 0, { 0 } };
            delEpv.data.ptr = pConnection;
            if (-1 == epoll_ctl(m_hCompletePort, EPOLL_CTL_DEL, pConnection->GetSocket(), &delEpv))
            {
                CLog::GetInstancePtr()->LogError("---EventDelete::epoll_ctl 실패 원인: %s!", CommonFunc::GetLastErrorStr(errno).c_str());
                return false;
            }

            return true;
        }
#endif

        bool PostSendOperation(CConnection* pConnection)
        {
            ERROR_RETURN_false(pConnection != nullptr);

            if (!pConnection->m_IsSending)
            {
#ifdef WIN32
                pConnection->m_IsSending = true;
                pConnection->m_IoOverLapPost.Reset();
                pConnection->m_IoOverLapPost.nOpType = NET_OP_POST;
                pConnection->m_IoOverLapPost.nConnID = pConnection->GetConnectionID();
                PostQueuedCompletionStatus(m_hCompletePort, pConnection->GetConnectionID(), (ULONG_PTR)pConnection, (LPOVERLAPPED)&pConnection->m_IoOverLapPost);
#else
                struct epoll_event EpollEvent;
                EpollEvent.data.ptr = pConnection;
                EpollEvent.events = EPOLLOUT | EPOLLET | EPOLLIN;
                epoll_ctl(m_hCompletePort, EPOLL_CTL_MOD, pConnection->GetSocket(), &EpollEvent);
#endif
            }
            return true;
        }

#ifdef WIN32
        CConnection* AssociateCompletePort(SOCKET hSocket, bool bConnect)
        {
            ERROR_RETURN_NULL(hSocket != INVALID_SOCKET && hSocket != 0);
            CConnection* pConnection = CConnectionMgr::GetInstancePtr()->CreateConnection();
            if (pConnection == nullptr)
            {
                CloseSocket(hSocket);
                return nullptr;
            }
            pConnection->SetSocket(hSocket);
            pConnection->SetDataHandler(m_pBufferHandler);
            if (NULL == CreateIoCompletionPort((HANDLE)hSocket, m_hCompletePort, (ULONG_PTR)pConnection, 0))
            {
                CLog::GetInstancePtr()->LogError("AssociateCompletePort 실패!");
                pConnection->Close();
            }

            return pConnection;
}
#else
        CConnection* AssociateCompletePort(SOCKET hSocket, bool bConnect)
        {
            ERROR_RETURN_NULL(hSocket != INVALID_SOCKET && hSocket != 0);

            CConnection* pConnection = CConnectionMgr::GetInstancePtr()->CreateConnection();
            if (pConnection == NULL)
            {
                CloseSocket(hSocket);
                return NULL;
            }

            pConnection->SetSocket(hSocket);

            pConnection->SetDataHandler(m_pBufferHandler);

            struct epoll_event EpollEvent;
            EpollEvent.data.ptr = pConnection;
            if (bConnect)
            {
                EpollEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
            }
            else
            {
                EpollEvent.events = EPOLLIN | EPOLLET;
            }

            if (-1 == epoll_ctl(m_hCompletePort, EPOLL_CTL_ADD, hSocket, &EpollEvent))
            {
                CLog::GetInstancePtr()->LogError("AssociateCompletePort 실패!");
                pConnection->Close();
            }

            return pConnection;
        }
#endif

        CConnection* ConnectTo_Sync(std::string strIpAddr, UINT16 sPort)
        {
            SOCKET hSocket = CreateSocket(AF_INET, SOCK_STREAM, 0);
            if (hSocket == INVALID_SOCKET)
            {
                CloseSocket(hSocket);
                CLog::GetInstancePtr()->LogError("소켓을 만들지 못했다!!");
                return NULL;
            }

            SetSocketBlock(hSocket, true);

            //SetSocketNoDelay(hSocket);

            if (!ConnectSocket(hSocket, strIpAddr.c_str(), sPort))
            {
                CloseSocket(hSocket);
                return NULL;
            }

            CConnection* pConnection = AssociateCompletePort(hSocket, true);
            if (pConnection == NULL)
            {
                CLog::GetInstancePtr()->LogError("완료 포트에 소켓을 본딩하지 못했다!!");
                return NULL;
            }

            SetSocketBlock(hSocket, false);

            pConnection->SetConnectStatus(NET_ST_CONN);

            m_pBufferHandler->OnNewConnect(pConnection->GetConnectionID());

            if (!pConnection->DoReceive())
            {
                pConnection->Close();
            }

            return pConnection;
        }

        CConnection* ConnectTo_Async(std::string strIpAddr, UINT16 sPort)
        {
            SOCKET hSocket = CreateSocket(AF_INET, SOCK_STREAM, 0);
            if (hSocket == INVALID_SOCKET || hSocket == 0)
            {
                CloseSocket(hSocket);
                CLog::GetInstancePtr()->LogError("소켓을 만들지 못했다!!");
                return NULL;
            }

            SetSocketBlock(hSocket, false);

            //SetSocketNoDelay(hSocket);

#ifdef WIN32
            CConnection* pConnection = AssociateCompletePort(hSocket, true);
            if (pConnection == NULL)
            {
                CLog::GetInstancePtr()->LogError("완료 포트에 소켓을 본딩하지 못했다!!");

                return NULL;
            }

            pConnection->m_IoOverlapRecv.Reset();

            pConnection->m_IoOverlapRecv.nOpType = NET_OP_CONNECT;

            pConnection->m_IoOverlapRecv.nConnID = pConnection->GetConnectionID();

            pConnection->m_dwIpAddr = IpAddrStrToInt((CHAR*)strIpAddr.c_str());

            bool bRet = ConnectSocketEx(hSocket, strIpAddr.c_str(), sPort, (LPOVERLAPPED)&pConnection->m_IoOverlapRecv);

            if (!bRet)
            {
                CLog::GetInstancePtr()->LogError("ConnectTo_Async 대상 서버에 연결하지 못했다,IP:%s--Port:%d!!", strIpAddr.c_str(), sPort);
                pConnection->Close();
            }

            return pConnection;
#else
            bool bRet = ConnectSocket(hSocket, strIpAddr.c_str(), sPort);
            if (!bRet)
            {
                CLog::GetInstancePtr()->LogError("ConnectTo_Async 연결 실패,IP:%s,Port:%d!", strIpAddr.c_str(), sPort);
                CloseSocket(hSocket);
                return NULL;
            }

            CConnection* pConnection = AssociateCompletePort(hSocket, true);
            if (pConnection == NULL)
            {
                CLog::GetInstancePtr()->LogError("완료 포트에 소켓을 본딩하지 못했다!!");

                return NULL;
            }

            return pConnection;
#endif
        }

        bool WaitConnect()
        {
#ifdef WIN32
            if (NULL == CreateIoCompletionPort((HANDLE)m_hListenSocket, m_hCompletePort, (ULONG_PTR)NULL, 0))
            {
                CLog::GetInstancePtr()->LogError("WaitConnect가 Listen 소켓을 명시하지 못했다: %s!", CommonFunc::GetLastErrorStr(GetSocketLastError()).c_str());
                return false;
            }
            m_IoOverlapAccept.Reset();
            m_IoOverlapAccept.nOpType = NET_OP_ACCEPT;
            m_hCurAcceptSocket = CreateSocket();
            return AcceptSocketEx(m_hListenSocket, m_hCurAcceptSocket, (CHAR*)m_AddressBuf, (LPOVERLAPPED)&m_IoOverlapAccept);
#else
            struct epoll_event epollev;
            epollev.data.fd = m_hListenSocket;
            epollev.events = EPOLLIN | EPOLLET;
            return (-1 < epoll_ctl(m_hCompletePort, EPOLL_CTL_ADD, m_hListenSocket, &epollev));
#endif
        }


    public:
        SOCKET              m_hListenSocket = NULL;
        NetIoOperatorData   m_IoOverlapAccept;
        SOCKET              m_hCurAcceptSocket;
        HANDLE              m_hCompletePort = NULL;
        char                m_AddressBuf[128];
        bool                m_bCloseEvent = false; //이벤트 처리 스레드 종료 여부

        IDataHandler* m_pBufferHandler = nullptr;
        std::vector<std::thread*> m_vtEventThread;

    };

}

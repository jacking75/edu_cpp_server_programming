#pragma once

#include "IBufferHandler.h"
#include "ReaderWriterQueue.h"
#include "DataBuffer.h"
#include "CommonSocket.h"
#include "PacketHeader.h"
#include "Log.h"

#include <vector>
#include <mutex>

namespace hbServerEngine
{

#define  NET_OP_RECV                1
#define  NET_OP_SEND                2
#define  NET_OP_CONNECT             3
#define  NET_OP_ACCEPT              4
#define  NET_OP_POST                5
#define  NET_OP_UDP_RECV            6

#define RECV_BUF_SIZE               8192
#define MAX_BUFF_SIZE               32768

#define E_SEND_SUCCESS              1
#define E_SEND_UNDONE               2
#define E_SEND_ERROR                3

#define NET_ST_INIT                 1
#define NET_ST_CONN                 2
#define NET_ST_WAIT                 3

    struct NetIoOperatorData
    {
#ifdef WIN32
        OVERLAPPED      Overlap;
#endif
        int32_t           nOpType;
        int32_t           nConnID;

        IDataBuffer* pDataBuffer;

        void Reset()
        {
#ifdef WIN32
            memset(&Overlap, 0, sizeof(Overlap));
#endif

            nOpType = 0;
            nConnID = 0;

            pDataBuffer = nullptr;
        }
    };

    class CConnection
    {
    public:
        CConnection() 
        {
        }
        
        virtual ~CConnection()
        {
            Reset();
            m_nConnID = 0;
            m_pDataHandler = nullptr;
        }


    public:
        bool HandleRecvEvent(int32_t nBytes)
        {
#ifdef WIN32
            m_nDataLen += nBytes;

            if (!ExtractBuffer())
            {
                return false;
            }

            if (!DoReceive())
            {
                return false;
            }
#else
            if (!DoReceive())
            {
                return false;
            }

            //   if(!ExtractBuffer())
            //   {
            //       return false;
            //   }
#endif
            return true;
        }

        int32_t GetConnectionID()
        {
            return m_nConnID;
        }

        uint64_t GetConnectionData()
        {
            return m_uConnData;
        }

        void SetConnectionID(int32_t nConnID)
        {
            ERROR_RETURN_NONE(nConnID != 0);

            ERROR_RETURN_NONE(m_nConnStatus == NET_ST_INIT);

            m_nConnID = nConnID;

            return;
        }

        void SetConnectionData(uint64_t uData)
        {
            ERROR_RETURN_NONE(m_nConnID != 0);

            m_uConnData = uData;

            return;
        }

        bool Shutdown()
        {
            m_nConnStatus = NET_ST_WAIT;

            m_uLastRecvTick = CommonFunc::GetTickCount();

            _ShutdownSocket(m_hSocket);

            return true;
        }

        bool Close()
        {
            CloseSocket(m_hSocket);

            m_hSocket = INVALID_SOCKET;

            m_nDataLen = 0;

            m_IsSending = false;

            if (m_pDataHandler != NULL && !m_bNotified)
            {
                m_bNotified = true;
                m_pDataHandler->OnCloseConnect(GetConnectionID());
            }

            m_nConnStatus = NET_ST_INIT;

            return true;
        }

        bool SetSocket(SOCKET hSocket)
        {
            m_hSocket = hSocket;

            return true;
        }

        SOCKET GetSocket()
        {
            return m_hSocket;
        }

        bool SetDataHandler(IDataHandler* pHandler)
        {
            ERROR_RETURN_false(pHandler != NULL);

            m_pDataHandler = pHandler;

            return true;
        }

        bool ExtractBuffer()
        {
            // 이 메서드에서 false를 반환한다.
            // 이 연결이 외부에서 닫히게 된다.
            if (m_nDataLen == 0)
            {
                return true;
            }

            while (true)
            {
                if (m_pCurRecvBuffer != nullptr)
                {
                    if ((m_pCurRecvBuffer->GetTotalLenth() + m_nDataLen) < m_nCurBufferSize)
                    {
                        memcpy(m_pCurRecvBuffer->GetBuffer() + m_pCurRecvBuffer->GetTotalLenth(), m_pBufPos, m_nDataLen);
                        m_pBufPos = m_pRecvBuf;
                        m_pCurRecvBuffer->SetTotalLenth(m_pCurRecvBuffer->GetTotalLenth() + m_nDataLen);
                        m_nDataLen = 0;
                        break;
                    }
                    else
                    {
                        memcpy(m_pCurRecvBuffer->GetBuffer() + m_pCurRecvBuffer->GetTotalLenth(), m_pBufPos, m_nCurBufferSize - m_pCurRecvBuffer->GetTotalLenth());
                        m_nDataLen -= m_nCurBufferSize - m_pCurRecvBuffer->GetTotalLenth();
                        m_pBufPos += m_nCurBufferSize - m_pCurRecvBuffer->GetTotalLenth();
                        m_pCurRecvBuffer->SetTotalLenth(m_nCurBufferSize);
                        m_uLastRecvTick = CommonFunc::GetTickCount();
                        m_pDataHandler->OnDataHandle(m_pCurRecvBuffer, GetConnectionID());
                        m_pCurRecvBuffer = nullptr;
                    }
                }

                if (m_nDataLen < sizeof(PacketHeader))
                {
                    if (m_nDataLen >= 1 && *(BYTE*)m_pBufPos != 0x88)
                    {
                        CLog::GetInstancePtr()->LogWarn("첫 바이트 누락 확인!, m_nDataLen:%d--ConnID:%d", m_nDataLen, m_nConnID);
                        return false;
                    }

                    break;
                }

                PacketHeader* pHeader = (PacketHeader*)m_pBufPos;
                
                //여기서 패킷 헤더를 확인하고 합법적이지 않은 경우 false를 반환한다
                if (!CheckHeader(m_pBufPos))
                {
                    //return false;
                }

                int32_t nPacketSize = pHeader->TotalSize;

                
                if ((nPacketSize > m_nDataLen) && (nPacketSize < RECV_BUF_SIZE))
                {
                    break;
                }

                if (nPacketSize <= m_nDataLen)
                {
                    IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(nPacketSize);

                    memcpy(pDataBuffer->GetBuffer(), m_pBufPos, nPacketSize);

                    m_nDataLen -= nPacketSize;

                    m_pBufPos += nPacketSize;

                    pDataBuffer->SetTotalLenth(nPacketSize);

                    m_uLastRecvTick = CommonFunc::GetTickCount();
                    m_pDataHandler->OnDataHandle(pDataBuffer, GetConnectionID());
                }
                else
                {
                    IDataBuffer* pDataBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(nPacketSize);
                    memcpy(pDataBuffer->GetBuffer(), m_pBufPos, m_nDataLen);

                    pDataBuffer->SetTotalLenth(m_nDataLen);
                    m_nDataLen = 0;
                    m_pBufPos = m_pRecvBuf;
                    m_pCurRecvBuffer = pDataBuffer;
                    m_nCurBufferSize = nPacketSize;
                }
            }

            if (m_nDataLen > 0)
            {
                memmove(m_pRecvBuf, m_pBufPos, m_nDataLen);
            }

            m_pBufPos = m_pRecvBuf;

            return true;
        }

#ifdef WIN32
        bool DoReceive()
        {
            WSABUF  DataBuf;

            DataBuf.len = RECV_BUF_SIZE - m_nDataLen;
            DataBuf.buf = m_pRecvBuf + m_nDataLen;

            DWORD nRecvBytes = 0, nFlags = 0;

            m_IoOverlapRecv.Reset();
            m_IoOverlapRecv.nOpType = NET_OP_RECV;
            m_IoOverlapRecv.nConnID = m_nConnID;

            int nRet = WSARecv(m_hSocket, &DataBuf, 1, &nRecvBytes, &nFlags, (LPOVERLAPPED)&m_IoOverlapRecv, NULL);
            if (nRet != 0)
            {
                //반환 값이 0이면 오류가 발생하지 않았음을 의미한다.
                // 반환 값이 ERROR_IO_PENDING이면 읽기 요청이 성공적으로 전송되었음을 의미하며, 다른 모든 반환 값은 오류이다.
                int nError = GetSocketLastError();
                if (nError != ERROR_IO_PENDING)
                {
                    CLog::GetInstancePtr()->LogWarn("데이터를 받는 동안 오류가 발생했기 때문에 연결을 닫는다:%s!", CommonFunc::GetLastErrorStr(nError).c_str());

                    return false;
                }
            }

            // WSARecv의 경우 0을 반환하면 오류가 발생하지 않았다는 의미이다

            return true;
    }
#else
        bool    DoReceive()
        {
            while (true)
            {
                int nBytes = recv(m_hSocket, m_pRecvBuf + m_nDataLen, RECV_BUF_SIZE - m_nDataLen, 0);
                if (nBytes < 0)
                {
                    int nErr = CommonSocket::GetSocketLastError();
                    if (nErr == EAGAIN)
                    {
                        return true;
                    }

                    CLog::GetInstancePtr()->LogWarn("읽기 실패, 연결이 끊어졌을 수 있음 원인:%s!!", CommonFunc::GetLastErrorStr(nErr).c_str());
                    return false;
                }

                if (nBytes == 0)
                {
                    return false;
                }

                m_nDataLen += nBytes;

                if (!ExtractBuffer())
                {
                    return false;
                }
            }

            return true;
        }
#endif

        int32_t   GetConnectStatus()
        {
            return m_nConnStatus;
        }

        bool SetConnectStatus(int32_t nConnStatus)
        {
            m_nConnStatus = nConnStatus;

            m_uLastRecvTick = CommonFunc::GetTickCount();

            return true;
        }

        bool Reset()
        {
            m_hSocket = INVALID_SOCKET;

            m_nConnStatus = NET_ST_INIT;

            m_uConnData = 0;

            m_nDataLen = 0;

            m_dwIpAddr = 0;

            m_pBufPos = m_pRecvBuf;

            m_bNotified = false;

            m_uLastRecvTick = 0;

            memset(&m_UdpAddr, 0, sizeof(m_UdpAddr));

            if (m_pCurRecvBuffer != nullptr)
            {
                m_pCurRecvBuffer->Release();
                m_pCurRecvBuffer = nullptr;
            }

            m_nCheckNo = 0;

            m_IsSending = false;

            IDataBuffer* pBuff = nullptr;
            while (m_SendBuffList.try_dequeue(pBuff))
            {
                pBuff->Release();
            }

            return true;
        }

        bool SendBuffer(IDataBuffer* pBuff)
        {
            return m_SendBuffList.enqueue(pBuff);
        }

#ifdef WIN32
        bool DoSend()
        {
            IDataBuffer* pFirstBuff = NULL;
            IDataBuffer* pSendingBuffer = NULL;
            int nSendSize = 0;
            int nCurPos = 0;

            IDataBuffer* pBuffer = NULL;
            while (m_SendBuffList.try_dequeue(pBuffer))
            {
                nSendSize += pBuffer->GetTotalLenth();

                if (pFirstBuff == NULL && pSendingBuffer == NULL)
                {
                    pFirstBuff = pBuffer;

                    pBuffer = NULL;
                }
                else
                {
                    if (pSendingBuffer == NULL)
                    {
                        pSendingBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(RECV_BUF_SIZE);
                        pFirstBuff->CopyTo(pSendingBuffer->GetBuffer() + nCurPos, pFirstBuff->GetTotalLenth());
                        pSendingBuffer->SetTotalLenth(pSendingBuffer->GetTotalLenth() + pFirstBuff->GetTotalLenth());
                        nCurPos += pFirstBuff->GetTotalLenth();
                        pFirstBuff->Release();
                        pFirstBuff = NULL;
                    }

                    pBuffer->CopyTo(pSendingBuffer->GetBuffer() + nCurPos, pBuffer->GetTotalLenth());
                    pSendingBuffer->SetTotalLenth(pSendingBuffer->GetTotalLenth() + pBuffer->GetTotalLenth());
                    nCurPos += pBuffer->GetTotalLenth();
                    pBuffer->Release();
                    pBuffer = NULL;
                }

                IDataBuffer** pPeekBuff = m_SendBuffList.peek();
                if (pPeekBuff == NULL)
                {
                    break;
                }

                pBuffer = *pPeekBuff;
                if (nSendSize + pBuffer->GetTotalLenth() >= RECV_BUF_SIZE)
                {
                    break;
                }

                pBuffer = NULL;
            }

            if (pSendingBuffer == NULL)
            {
                pSendingBuffer = pFirstBuff;
            }

            if (pSendingBuffer == NULL)
            {
                m_IsSending = false;
                return true;
            }

            WSABUF  DataBuf;
            DataBuf.len = pSendingBuffer->GetTotalLenth();
            DataBuf.buf = pSendingBuffer->GetBuffer();
            m_IoOverlapSend.Reset();
            m_IoOverlapSend.nOpType = NET_OP_SEND;
            m_IoOverlapSend.pDataBuffer = pSendingBuffer;
            m_IoOverlapSend.nConnID = m_nConnID;

            DWORD nSendBytes = 0;
            int nRet = WSASend(m_hSocket, &DataBuf, 1, &nSendBytes, 0, (LPOVERLAPPED)&m_IoOverlapSend, NULL);
            if (nRet == 0) // 보내기 성공
            {
                //if(nSendBytes < DataBuf.len)
                //{
                //  CLog::GetInstancePtr()->LogError("发送线程:直接发送数据成功send:%d--Len:%d!", nSendBytes, DataBuf.len);
                //}
            }
            else if (nRet == -1) // 보내기 오류
            {
                int32_t errCode = GetSocketLastError();
                if (errCode != ERROR_IO_PENDING)
                {
                    Close();
                    CLog::GetInstancePtr()->LogError("스레드 보내기:보내지 못함, 연결이 닫힘 원인:%s!", CommonFunc::GetLastErrorStr(errCode).c_str());
                }
            }

            return true;
}
#else
        bool DoSend()
        {
            //반환 값이 양수이며, 완전히 전송된 경우와 부분적으로 전송된 경우로 나뉘며, 부분적으로 전송된 경우 다른 버퍼를 사용하여 계속 전송한다.
            // 반환 값이 음수이다 오류 코드이다:
            //
            //if (errno != EAGAIN)
            //{
            //  //ERROR("TcpConnection sendInLoop");
            //  if (errno == EPIPE || errno == ECONNRESET)
            //  {
            //      faultError = true;//이건 실수이다
            //  }
            //}
            // #define E_SEND_SUCCESS               1
            // #define E_SEND_UNDONE                2
            // #define E_SEND_ERROR                 3

            if (m_pSendingBuffer != NULL)
            {
                INT32 nDataLen = m_pSendingBuffer->GetTotalLenth() - m_nSendingPos;
                INT32 nRet = send(m_hSocket, m_pSendingBuffer->GetBuffer() + m_nSendingPos, nDataLen, 0);
                if (nRet < 0)
                {
                    if (errno != EAGAIN)
                    {
                        m_pSendingBuffer->Release();
                        m_pSendingBuffer = NULL;
                        m_nSendingPos = 0;
                        CLog::GetInstancePtr()->LogWarn("스레드 보내기:보내지 못함, 연결이 닫힘 원인:%s!", GetLastErrorStr(errno).c_str());
                        return E_SEND_ERROR;
                    }

                    return E_SEND_SUCCESS;
                }

                if (nRet < nDataLen)
                {
                    // 이는 데이터의 절반이 전송되었음을 의미한다
                    m_nSendingPos += nRet;
                    return E_SEND_UNDONE;
                }
                else
                {
                    m_pSendingBuffer->Release();
                    m_pSendingBuffer = NULL;
                    m_nSendingPos = 0;
                }
            }

            IDataBuffer* pBuffer = NULL;
            while (m_SendBuffList.try_dequeue(pBuffer))
            {
                if (pBuffer == NULL)
                {
                    continue;
                }

                INT32 nRet = send(m_hSocket, pBuffer->GetBuffer(), pBuffer->GetTotalLenth(), 0);
                if (nRet < 0)
                {
                    if (errno != EAGAIN)
                    {
                        m_pSendingBuffer->Release();
                        m_pSendingBuffer = NULL;
                        m_nSendingPos = 0;
                        CLog::GetInstancePtr()->LogWarn("스레드 보내기:전송 실패, 연결이 닫힘 원인 2:%s!", GetLastErrorStr(errno).c_str());
                        return E_SEND_ERROR;
                    }

                    return E_SEND_SUCCESS;
                }

                if (nRet < pBuffer->GetTotalLenth())
                {
                    // 이는 데이터의 절반이 전송되었음을 의미한다
                    m_pSendingBuffer = pBuffer;
                    m_nSendingPos = nRet;
                    return E_SEND_UNDONE;
                }

                pBuffer->Release();
            }

            return E_SEND_SUCCESS;
        }
#endif

        bool CheckHeader(char* pNetPacket)
        {
            /*
            1. 먼저 패키지의 유효성을 확인하시겠습니까?
            2. 패키지의 길이
            3. 패키지의 일련 번호 
            */
            PacketHeader* pHeader = (PacketHeader*)m_pBufPos;
            
            if (pHeader->TotalSize > 1024 * 1024)
            {
                return false;
            }

            if (pHeader->TotalSize <= 0)
            {
                CLog::GetInstancePtr()->LogWarn("packetsize < 0, pHeader->nMsgID:%d", pHeader->MsgID);
                return false;
            }

            if (pHeader->MsgID > 399999 || pHeader->MsgID == 0)
            {
                CLog::GetInstancePtr()->LogWarn("Invalid MessageID roleid:%d", pHeader->MsgID);
                return false;
            }

            return true;
        }
                
        uint32_t GetIpAddr(bool bHost = true)
        {
            if (bHost)
            {
                return m_dwIpAddr;
            }

            return HostToNet(m_dwIpAddr);
        }

        

    public:
        SOCKET                      m_hSocket = INVALID_SOCKET;

        int32_t                       m_nConnStatus = NET_ST_INIT;

        bool                        m_bNotified = false;

        NetIoOperatorData           m_IoOverlapRecv;

        NetIoOperatorData           m_IoOverlapSend;

        NetIoOperatorData           m_IoOverLapPost;

        int32_t                       m_nConnID = 0;
        uint64_t                      m_uConnData = 0;

        IDataHandler* m_pDataHandler = nullptr;

        UINT32                      m_dwIpAddr = 0;

        int32_t                       m_nDataLen = 0;
        char                        m_pRecvBuf[RECV_BUF_SIZE];
        char* m_pBufPos = m_pRecvBuf;

        IDataBuffer* m_pCurRecvBuffer = nullptr;
        int32_t                       m_nCurBufferSize = 0;
        int32_t                       m_nCheckNo = 0;

        volatile bool               m_IsSending = false;

        CConnection* p_next_ = nullptr;

        uint64_t                      m_uLastRecvTick = 0;

        moodycamel::ReaderWriterQueue<IDataBuffer*> m_SendBuffList;

        sockaddr                    m_UdpAddr;

        IDataBuffer* m_pSendingBuffer;
        int32_t                       m_nSendingPos = 0;
    };


    class CConnectionMgr
    {
    private:
        CConnectionMgr()
        {
        }

        ~CConnectionMgr()
        {
            DestroyAllConnection();
            m_pFreeConnRoot = nullptr;
            m_pFreeConnTail = nullptr;
        }

    public:
        static CConnectionMgr* GetInstancePtr()
        {
            static CConnectionMgr ConnectionMgr;
            return &ConnectionMgr;
        }

    public:
        bool InitConnectionList(int32_t nMaxCons)
        {
            ERROR_RETURN_false(m_pFreeConnRoot == nullptr);
            ERROR_RETURN_false(m_pFreeConnTail == nullptr);

            m_vtConnList.assign(nMaxCons, nullptr);

            for (int32_t i = 0; i < nMaxCons; i++)
            {
                CConnection* pConn = new CConnection();

                m_vtConnList[i] = pConn;

                pConn->SetConnectionID(i + 1);

                if (m_pFreeConnRoot == nullptr)
                {
                    m_pFreeConnRoot = pConn;
                    pConn->p_next_ = nullptr;
                    m_pFreeConnTail = pConn;
                }
                else
                {
                    m_pFreeConnTail->p_next_ = pConn;
                    m_pFreeConnTail = pConn;
                    m_pFreeConnTail->p_next_ = nullptr;
                }
            }

            return true;
        }

        CConnection* CreateConnection()
        {
            CConnection* pTemp = nullptr;
            m_ConnListMutex.lock();
            if (m_pFreeConnRoot == nullptr)
            {
                // 연결 제한에 도달하여 새 연결을 만들 수 없음을 나타낸다
                m_ConnListMutex.unlock();
                return nullptr;
            }

            if (m_pFreeConnRoot == m_pFreeConnTail)
            {
                pTemp = m_pFreeConnRoot;
                m_pFreeConnTail = m_pFreeConnRoot = nullptr;
            }
            else
            {
                pTemp = m_pFreeConnRoot;
                m_pFreeConnRoot = pTemp->p_next_;
                pTemp->p_next_ = nullptr;
            }
            m_ConnListMutex.unlock();
            ERROR_RETURN_NULL(pTemp->GetConnectionID() != 0);
            ERROR_RETURN_NULL(pTemp->GetSocket() == INVALID_SOCKET);
            ERROR_RETURN_NULL(pTemp->GetConnectStatus() == NET_ST_INIT);
            return pTemp;
        }

        bool DeleteConnection(int32_t nConnID)
        {
            ERROR_RETURN_false(nConnID != 0);

            int32_t nIndex = nConnID % m_vtConnList.size();

            CConnection* pConnection = m_vtConnList.at(nIndex == 0 ? (m_vtConnList.size() - 1) : (nIndex - 1));

            ERROR_RETURN_false(pConnection->GetConnectionID() == nConnID)

             m_ConnListMutex.lock();

            if (m_pFreeConnTail == nullptr)
            {
                ERROR_RETURN_false(m_pFreeConnRoot == nullptr);

                m_pFreeConnTail = m_pFreeConnRoot = pConnection;
            }
            else
            {
                m_pFreeConnTail->p_next_ = pConnection;

                m_pFreeConnTail = pConnection;

                m_pFreeConnTail->p_next_ = nullptr;
            }

            m_ConnListMutex.unlock();

            pConnection->Reset();

            nConnID += (int32_t)m_vtConnList.size();

            if (nConnID <= 0)
            {
                nConnID = nIndex + 1;
            }

            pConnection->SetConnectionID(nConnID);

            return true;
        }

        CConnection* GetConnectionByID(int32_t nConnID)
        {
            ERROR_RETURN_NULL(nConnID != 0);

            int32_t nIndex = nConnID % m_vtConnList.size();

            CConnection* pConnect = m_vtConnList.at(nIndex == 0 ? (m_vtConnList.size() - 1) : (nIndex - 1));

            if (pConnect->GetConnectionID() != nConnID)
            {
                return nullptr;
            }

            return pConnect;
        }

        ///////////////////////////////////////////
        bool CloseAllConnection()
        {
            CConnection* pConn = nullptr;

            for (size_t i = 0; i < m_vtConnList.size(); i++)
            {
                pConn = m_vtConnList.at(i);
                if (pConn->GetConnectStatus() != NET_ST_CONN)
                {
                    continue;
                }

                if (pConn->m_hSocket == INVALID_SOCKET)
                {
                    continue;
                }

                pConn->Close();
            }

            return true;
        }

        bool DestroyAllConnection()
        {
            CConnection* pConn = nullptr;

            for (size_t i = 0; i < m_vtConnList.size(); i++)
            {
                pConn = m_vtConnList.at(i);
                if (pConn == nullptr)
                {
                    continue;
                }

                if (pConn->GetConnectStatus() != NET_ST_INIT && pConn->m_hSocket != INVALID_SOCKET)
                {
                    pConn->Close();
                }

                delete pConn;
            }

            m_vtConnList.clear();

            return true;
        }

        bool CheckConntionAvalible(int32_t nInterval)
        {
            //return true;
            uint64_t uCurTick = CommonFunc::GetTickCount();

            for (std::vector<CConnection*>::size_type i = 0; i < m_vtConnList.size(); i++)
            {
                CConnection* pConnection = m_vtConnList.at(i);
                if (pConnection->GetConnectStatus() == NET_ST_INIT)
                {
                    continue;
                }

                if (pConnection->m_uLastRecvTick <= 0)
                {
                    continue;
                }

                if (pConnection->GetConnectionData() == 1)
                {
                    continue;
                }

                // 현재 상태가 닫히기를 기다리는 경우
                if (pConnection->GetConnectStatus() == NET_ST_WAIT)
                {
                    if (uCurTick > (pConnection->m_uLastRecvTick + 10 * 1000))
                    {
                        CLog::GetInstancePtr()->LogError("CConnectionMgr::CheckConntionAvalible WAIT 시간 초과 후 활성 연결 해제 ConnID:%d", pConnection->GetConnectionID());
                        pConnection->Close();
                    }

                    continue;
                }

                if (uCurTick > (pConnection->m_uLastRecvTick + nInterval * 1000))
                {
                    pConnection->Shutdown();
                }
            }

            return true;
        }

    public:

        CConnection* m_pFreeConnRoot = nullptr;
        CConnection* m_pFreeConnTail = nullptr;
        std::vector<CConnection*>   m_vtConnList;            //연결 목록
        std::mutex                  m_ConnListMutex;
    };

}
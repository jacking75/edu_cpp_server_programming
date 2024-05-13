#pragma once

#include "Connection.h"
#include "NetManager.h"
#include "IBufferHandler.h"
#include "ConfigFile.h"


#include <queue>
#include <string>
#include <mutex>


namespace hbServerEngine
{
#define NEW_CONNECTION 1
#define CLOSE_CONNECTION 2

    class ServiceBase : public IDataHandler
    {
    protected:
        ServiceBase(void)
        {
            m_pPacketDispatcher = nullptr;
            m_pRecvDataQueue = new std::deque<NetPacket>();
            m_pDispathQueue = new std::deque<NetPacket>();
        }

        virtual ~ServiceBase(void)
        {
            delete m_pRecvDataQueue;
            delete m_pDispathQueue;

            m_pRecvDataQueue = nullptr;
            m_pDispathQueue = nullptr;
        }

    public:
        static ServiceBase* GetInstancePtr()
        {
            static ServiceBase _ServiceBase;
            return &_ServiceBase;
        }

        bool StartNetwork(uint16_t nPortNum, int32_t nMaxConn, INetEventDispatcher* pDispather, std::string strListenIp = "")
        {
            ERROR_RETURN_false(pDispather != NULL);
            ERROR_RETURN_false(nPortNum > 0);
            ERROR_RETURN_false(nMaxConn > 0);

            m_pPacketDispatcher = pDispather;

            if (!CNetManager::GetInstancePtr()->Start(nPortNum, nMaxConn, this, strListenIp))
            {
                CLog::GetInstancePtr()->LogError("네트워크 계층을 시작하지 못했다!");
                return false;
            }

            m_nRecvNum = 0;
            m_nSendNum = 0;
            m_nLastMsgID = 0;
            return true;
        }

        bool StopNetwork()
        {
            CNetManager::GetInstancePtr()->Stop();

            return true;
        }

        bool OnDataHandle(IDataBuffer* pDataBuffer, int32_t nConnID)
        {
            PacketHeader* pHeader = (PacketHeader*)pDataBuffer->GetBuffer();

            queue_lock_.lock();
            m_pRecvDataQueue->emplace_back(NetPacket(nConnID, pDataBuffer, pHeader->nMsgID));
            queue_lock_.unlock();

            return true;
        }

        bool OnCloseConnect(int32_t nConnID)
        {
            ERROR_RETURN_false(nConnID != 0);
            queue_lock_.lock();
            m_pRecvDataQueue->emplace_back(NetPacket(nConnID, NULL, CLOSE_CONNECTION));
            queue_lock_.unlock();
            return true;
        }

        bool OnNewConnect(int32_t nConnID)
        {
            ERROR_RETURN_false(nConnID != 0);
            queue_lock_.lock();
            m_pRecvDataQueue->emplace_back(NetPacket(nConnID, NULL, NEW_CONNECTION));
            queue_lock_.unlock();
            return true;
        }

        CConnection* ConnectTo(std::string strIpAddr, uint16_t sPort)
        {
            ERROR_RETURN_NULL(!strIpAddr.empty() && sPort > 0);

            return CNetManager::GetInstancePtr()->ConnectTo_Async(strIpAddr, sPort);
        }

        bool CloseConnect(int32_t nConnID)
        {
            CConnection* pConnection = GetConnectionByID(nConnID);

            ERROR_RETURN_false(pConnection != NULL);

            pConnection->Close();

            return true;
        }

        template<typename T>
        bool SendMsgStruct(int32_t nConnID, int32_t nMsgID, uint64_t u64TargetID, uint32_t dwUserData, T& Data)
        {
            if (nConnID <= 0)
            {
                return false;
            }

            m_nSendNum++;

            return CNetManager::GetInstancePtr()->SendMessageData(nConnID, nMsgID, u64TargetID, dwUserData, &Data, sizeof(T));
        }
                
        bool SendMsgRawData(int32_t nConnID, int32_t nMsgID, uint64_t u64TargetID, uint32_t dwUserData, const char* pdata, uint32_t dwLen)
        {
            if (nConnID <= 0)
            {
                return false;
            }

            m_nSendNum++;

            return CNetManager::GetInstancePtr()->SendMessageData(nConnID, nMsgID, u64TargetID, dwUserData, pdata, dwLen);
        }

        bool SendMsgBuffer(int32_t nConnID, IDataBuffer* pDataBuffer)
        {
            if (nConnID == 0)
            {
                return false;
            }

            m_nSendNum++;
            return CNetManager::GetInstancePtr()->SendMessageBuff(nConnID, pDataBuffer);
        }

        CConnection* GetConnectionByID(int32_t nConnID)
        {
            return CConnectionMgr::GetInstancePtr()->GetConnectionByID(nConnID);
        }

        int Update()
        {
            queue_lock_.lock();
            std::swap(m_pRecvDataQueue, m_pDispathQueue);
            queue_lock_.unlock();

            auto queue_count = (int)m_pDispathQueue->size();
            if (queue_count <= 0)
            {
                return queue_count;
            }
            
            for (auto itor = m_pDispathQueue->begin(); 
                itor != m_pDispathQueue->end(); 
                ++itor)
            {
                NetPacket& item = *itor;
                if (item.m_nMsgID == NEW_CONNECTION)
                {
                    m_pPacketDispatcher->OnNewConnect(item.m_nConnID);
                }
                else if (item.m_nMsgID == CLOSE_CONNECTION)
                {
                    m_pPacketDispatcher->OnCloseConnect(item.m_nConnID);
                        
                    CConnectionMgr::GetInstancePtr()->DeleteConnection(item.m_nConnID);
                }
                else
                {
                    m_nLastMsgID = item.m_nMsgID;
                    m_pPacketDispatcher->DispatchPacket(&item);

                    item.m_pDataBuffer->Release();

                    m_nRecvNum += 1;
                }
            }

            m_pDispathQueue->clear();
            
            return queue_count;
        }
                


    protected:
        INetEventDispatcher* m_pPacketDispatcher;

        //TODO: 락프리 큐로 바꾸어야 한다
        std::deque<NetPacket>* m_pRecvDataQueue;
        std::deque<NetPacket>* m_pDispathQueue; // 삭제한다
        std::mutex queue_lock_;
        //CSpinLock                           m_QueueLock;

        
        int32_t                               m_nRecvNum;
        int32_t                               m_nSendNum;
        int32_t                               m_nLastMsgID;
    };


}
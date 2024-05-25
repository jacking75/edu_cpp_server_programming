#pragma once

namespace hbServerEngine
{
    struct NetPacket;

    class IDataBuffer
    {
    public:
        virtual bool    AddRef() = 0;

        virtual bool    Release() = 0;

        virtual char* GetData() = 0;

        virtual int32_t   GetTotalLenth() = 0;

        virtual void    SetTotalLenth(int32_t nPos) = 0;

        virtual int32_t   GetBodyLenth() = 0;

        virtual char* GetBuffer() = 0;

        virtual int32_t   GetBufferSize() = 0;

        virtual int32_t   CopyFrom(IDataBuffer* pSrcBuffer) = 0;

        virtual int32_t   CopyTo(char* pDestBuf, int32_t nDestLen) = 0;
    };

    struct IDataHandler
    {
        virtual bool OnDataHandle(IDataBuffer* pDataBuffer, int32_t nConnID) = 0;
        virtual bool OnCloseConnect(int32_t nConnID) = 0;
        virtual bool OnNewConnect(int32_t nConnID) = 0;
    };

    struct INetEventDispatcher
    {
        virtual bool DispatchPacket(NetPacket* pNetPacket) = 0;
        virtual bool OnCloseConnect(int32_t nConnID) = 0;
        virtual bool OnNewConnect(int32_t nConnID) = 0;
    };

    struct NetPacket
    {
        NetPacket(int32_t nConnID = 0, IDataBuffer* pBuffer = nullptr, int32_t MsgID = 0)
        {
            m_nConnID = nConnID;
            m_pDataBuffer = pBuffer;
            m_nMsgID = MsgID;
        }

        int32_t        m_nMsgID = 0;
        int32_t        m_nConnID = 0;
        IDataBuffer* m_pDataBuffer = nullptr;
    };

}




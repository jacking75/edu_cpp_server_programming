#pragma once

#include "IBufferHandler.h"

#include <mutex>


namespace hbServerEngine
{
    template <int SIZE>
    class  CBufferManager;

#define HEADER_LEN 28

    template <int SIZE>
    class CDataBuffer : public IDataBuffer
    {
    public:
        CDataBuffer(void)
        {
            m_nDataLen = 0;
            m_nBufSize = SIZE;
            m_nRefCount = 0;
            p_prev_ = nullptr;
            p_next_ = nullptr;
            m_pManager = nullptr;
        }

        virtual ~CDataBuffer(void)
        {
            m_nDataLen = 0;
            m_nBufSize = SIZE;
            m_nRefCount = 0;
            p_prev_ = nullptr;
            p_next_ = nullptr;
            m_pManager = nullptr;
        }

        bool AddRef()
        {
            m_pManager->m_BuffMutex.lock();
            m_nRefCount++;
            m_pManager->m_BuffMutex.unlock();
            return true;
        }

        bool Release()
        {
            assert(m_pManager != nullptr);

            m_pManager->ReleaseDataBuff(this);

            return true;
        }

        char* GetData()
        {
            return m_Buffer + HEADER_LEN;
        }

        int32_t GetTotalLenth()
        {
            return m_nDataLen;
        }

        int32_t GetBodyLenth()
        {
            return m_nDataLen - HEADER_LEN;
        }

        void SetTotalLenth(int32_t nPos)
        {
            m_nDataLen = nPos;
        }

        char* GetBuffer()
        {
            return m_Buffer;
        }

        int32_t GetBufferSize()
        {
            return m_nBufSize;
        }

        int32_t  CopyFrom(IDataBuffer* pSrcBuffer)
        {
            memcpy(m_Buffer, pSrcBuffer->GetBuffer(), pSrcBuffer->GetTotalLenth());

            m_nDataLen = pSrcBuffer->GetTotalLenth();

            return m_nDataLen;
        }

        int32_t  CopyTo(CHAR* pDestBuf, int32_t nDestLen)
        {
            if (nDestLen < GetTotalLenth())
            {
                return 0;
            }

            memcpy(pDestBuf, GetBuffer(), GetTotalLenth());

            return nDestLen;
        }

        CDataBuffer<SIZE>* p_prev_ = nullptr;

        CDataBuffer<SIZE>* p_next_ = nullptr;

        CBufferManager<SIZE>* m_pManager = nullptr;

    public:
        int32_t       m_nRefCount = 0;

        int32_t       m_nBufSize = 0;

        char        m_Buffer[SIZE];

        int32_t       m_nDataLen = 0;
    };


    template <int SIZE>
    class  CBufferManager
    {
    public:
        CBufferManager()
        {
            m_pUsedList = nullptr;
            m_pFreeList = nullptr;
            m_nBufferCount = 0;
            m_EnablePool = false;
        }

        ~CBufferManager()
        {
            ReleaseAll();
        }

        IDataBuffer* AllocDataBuff()
        {
            m_BuffMutex.lock();
            CDataBuffer<SIZE>* pDataBuffer = nullptr;
            
            if (m_pFreeList == nullptr)
            {
                pDataBuffer = new CDataBuffer<SIZE>();
                pDataBuffer->m_pManager = this;
            }
            else
            {
                pDataBuffer = m_pFreeList;

                m_pFreeList = m_pFreeList->p_next_;

                if (m_pFreeList != nullptr)
                {
                    m_pFreeList->p_prev_ = nullptr;
                }

                pDataBuffer->p_next_ = nullptr;
                pDataBuffer->p_prev_ = nullptr;
            }

            assert(pDataBuffer->m_nRefCount == 0);

            pDataBuffer->m_nRefCount = 1;

            if (m_pUsedList == nullptr)
            {
                m_pUsedList = pDataBuffer;
            }
            else
            {
                pDataBuffer->p_next_ = m_pUsedList;
                m_pUsedList->p_prev_ = pDataBuffer;
                pDataBuffer->p_prev_ = nullptr;
                m_pUsedList = pDataBuffer;
            }

            m_nBufferCount += 1;
            m_BuffMutex.unlock();
            return pDataBuffer;
        }

        bool ReleaseDataBuff(CDataBuffer<SIZE>* pBuff)
        {
            assert(pBuff != nullptr);
            if (pBuff == nullptr)
            {
                return false;
            }

            assert(pBuff->m_nRefCount > 0);
            if (pBuff->m_nRefCount <= 0)
            {
                return false;
            }
            std::lock_guard<std::mutex> lock(m_BuffMutex);
            pBuff->m_nRefCount--;

            if (pBuff->m_nRefCount <= 0)
            {
                pBuff->m_nDataLen = 0;
                
                if (m_pUsedList == pBuff)
                {
                    m_pUsedList = pBuff->p_next_;
                    if (m_pUsedList != nullptr)
                    {
                        m_pUsedList->p_prev_ = nullptr;
                    }
                }
                else
                {
                    assert(pBuff->p_prev_ != nullptr);
                    pBuff->p_prev_->p_next_ = pBuff->p_next_;
                    if (pBuff->p_next_ != nullptr)
                    {
                        pBuff->p_next_->p_prev_ = pBuff->p_prev_;
                    }
                }

                if (m_EnablePool)
                {
                    pBuff->p_next_ = m_pFreeList;
                    pBuff->p_prev_ = nullptr;
                    m_pFreeList = pBuff;

                    if (pBuff->p_next_ != nullptr)
                    {
                        pBuff->p_next_->p_prev_ = pBuff;
                    }
                }
                else
                {
                    delete pBuff;
                }
                m_nBufferCount--;
            }
            return true;
        }

        void ReleaseAll()
        {
            CDataBuffer<SIZE>* pBufferNode = m_pFreeList;
            while (pBufferNode)
            {
                CDataBuffer<SIZE>* pTempNode = pBufferNode;
                pBufferNode = pTempNode->p_next_;
                delete pTempNode;
            }

            pBufferNode = m_pUsedList;
            while (pBufferNode)
            {
                CDataBuffer<SIZE>* pTempNode = pBufferNode;
                pBufferNode = pTempNode->p_next_;
                delete pTempNode;
            }

            return;
        }

        void SetEnablePool(bool bEnablePool)
        {
            m_EnablePool = bEnablePool;
        }

        void PrintOutList(CDataBuffer<SIZE>* pList)
        {
            int32_t nCount = 0;
            CDataBuffer<SIZE>* pBufferNode = pList;
            if (pBufferNode == nullptr)
            {
                return;
            }

            auto bNext = true;
            while (pBufferNode)
            {
                if (bNext)
                {
                    nCount++;
                    if (pBufferNode->p_next_ != nullptr)
                    {
                        pBufferNode = pBufferNode->p_next_;
                    }
                    else
                    {
                        bNext = false;
                        pBufferNode = pBufferNode->p_prev_;
                    }
                }
                else
                {
                    nCount++;
                    pBufferNode = pBufferNode->p_prev_;
                }
            }

            return;
        }

        CDataBuffer<SIZE>* m_pFreeList;

        CDataBuffer<SIZE>* m_pUsedList;

        std::mutex  m_BuffMutex;

        int32_t       m_nBufferCount;

        bool        m_EnablePool;
    };


    class CBufferAllocator
    {
        CBufferAllocator()
        {
            m_BufferManagerAny.SetEnablePool(false);
        }

        ~CBufferAllocator() {}

    public:
        static CBufferAllocator* GetInstancePtr()
        {
            static CBufferAllocator BufferManagerAll;

            return &BufferManagerAll;
        }

    public:
        IDataBuffer* AllocDataBuff(int nSize)
        {
            if (nSize < 64)
            {
                return m_BufferManager64B.AllocDataBuff();
            }
            if (nSize < 128)
            {
                return m_BufferManager128B.AllocDataBuff();
            }
            if (nSize < 256)
            {
                return m_BufferManager256B.AllocDataBuff();
            }
            if (nSize < 512)
            {
                return m_BufferManager512B.AllocDataBuff();
            }
            if (nSize < 1024)
            {
                return m_BufferManager1K.AllocDataBuff();
            }
            else if (nSize < 2048)
            {
                return m_BufferManager2K.AllocDataBuff();
            }
            else if (nSize < 4096)
            {
                return m_BufferManager4K.AllocDataBuff();
            }
            else if (nSize < 8192)
            {
                return m_BufferManager8K.AllocDataBuff();
            }
            else if (nSize < 16384)
            {
                return m_BufferManager16K.AllocDataBuff();
            }
            else if (nSize < 32768)
            {
                return m_BufferManager32K.AllocDataBuff();
            }
            else if (nSize < 65536)
            {
                return m_BufferManager64K.AllocDataBuff();
            }

            return m_BufferManagerAny.AllocDataBuff();
        }

        CBufferManager<64>     m_BufferManager64B;      //64B 이하의 메모리 풀 관리，
        CBufferManager<128>    m_BufferManager128B;     //128B 이하의 메모리 풀 관리，
        CBufferManager<256>    m_BufferManager256B;     //256B 이하의 메모리 풀 관리，
        CBufferManager<512>    m_BufferManager512B;     //512B 이하의 메모리 풀 관리，
        CBufferManager<1024>   m_BufferManager1K;       //1k 이하의 메모리 풀 관리，
        CBufferManager<2048>   m_BufferManager2K;       //2k 이하의 메모리 풀 관리，
        CBufferManager<4096>   m_BufferManager4K;       //4k 이하의 메모리 풀 관리，
        CBufferManager<8192>   m_BufferManager8K;       //8k 이하의 메모리 풀 관리，
        CBufferManager<16384>  m_BufferManager16K;      //16k 이하의 메모리 풀 관리，
        CBufferManager<32768>  m_BufferManager32K;      //32k 이하의 메모리 풀 관리，
        CBufferManager<65536>  m_BufferManager64K;      //64k 이하의 메모리 풀 관리，

        CBufferManager<10 * 1024 * 1014> m_BufferManagerAny;        //10M 이하의 메모리 풀 관리. 풀 관리는 없으며 요청과 릴리스만 가능
    };

}
#pragma once

#include <cstdint>
#include <tuple>


namespace UVC2NetLibz
{
	class ReadWriteRingBuffer
	{
	public:
		ReadWriteRingBuffer() = default;

		~ReadWriteRingBuffer()
		{
			delete[] mRingBuffer;
		}
				
		uint32_t GetMaxTry1RecvBufferSize() { return mMaxTry1RecvBufferSize;  }

		void Create(const uint32_t ringBufferSize_, uint32_t mMaxTry1RecvBufferSize_)
		{
			mMaxTry1RecvBufferSize = mMaxTry1RecvBufferSize_;

			mRingBuffer = new char[ringBufferSize_];
			mRingBufferSize = ringBufferSize_;

			Clear();
		}
				
		void Clear()
		{
			mWritePos = 0;
			mReadPos = 0;
		}

		std::tuple<uint32_t, char*> Read()
		{
			uint32_t readAbleSize = mWritePos - mReadPos;
			auto pData = &mRingBuffer[mReadPos];						
			return { readAbleSize, pData };
		}

		void IncremantReadPos(const uint32_t pos_) { mReadPos += pos_; }
		
		char* GetWriteablePtr() 
		{ 
			RotateReadWritePos(mMaxTry1RecvBufferSize);			
			return &mRingBuffer[mWritePos];
		}

		bool Write(const uint16_t dataSize_, const char* pData)
		{
			RotateReadWritePos(dataSize_);

			memcpy(&mRingBuffer, pData, dataSize_);
			mWritePos = dataSize_;
			return true;
		}


	private:		
		void RotateReadWritePos(const uint16_t writeDataSize_)
		{
			if ((mWritePos + writeDataSize_) >= mRingBufferSize)
			{
				auto remaindSize = mWritePos - mReadPos;
				if (remaindSize == 0)
				{
					mWritePos = 0;
					mReadPos = 0;

				}
				else if (remaindSize > 0)
				{
					memcpy(&mRingBuffer, &mRingBuffer[mReadPos], remaindSize);
					mWritePos = remaindSize;
					mReadPos = 0;
				}
			}
		}


		uint32_t mRingBufferSize = 0;
		char* mRingBuffer = nullptr;
		uint32_t mMaxTry1RecvBufferSize = 0;
		uint32_t mWritePos = 0;
		uint32_t mReadPos = 0;
	};

}
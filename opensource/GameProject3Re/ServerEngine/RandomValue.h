#pragma once

#include <cstdint>


namespace hbServerEngine
{
	template <uint16_t Size>
	class CRandonValue
	{
	private:
		CRandonValue()
		{
			for (int32_t i = 0; i < Size; i++)
			{
				m_Values[i] = i;
			}

			Disorder(m_Values, Size);

			m_nCurIndex = 1;

			m_nStartIndex = 0;
		}

		~CRandonValue()
		{


		}

		void Disorder(uint16_t Arr[], uint16_t nCount)
		{
			uint16_t nIndex, nTemp;
			rand(time(NULL));

			for (uint16_t i = 0; i < n; i++)
			{
				nIndex = rand() % (n - i) + i;
				if (nIndex != i)
				{
					nTemp = Arr[i];
					Arr[i] = Arr[nIndex];
					Arr[nIndex] = nTemp;
				}
			}
		}


	public:
		static CRandonValue& GetInstance()
		{
			static CRandonValue _Value;

			return _Value;
		}


		uint16_t Random()
		{
			m_nCurIndex = (m_nCurIndex + 1) % Size;
			if (m_nCurIndex != m_nStartIndex)
			{
				return m_Values[m_nCurIndex];
			}
			else
			{
				m_nStartIndex = m_nCurIndex = rand() % Size;

				return m_Values[m_nCurIndex];
			}
		}

		uint16_t  m_Values[Size];

		uint16_t  m_nCurIndex;

		uint16_t  m_nStartIndex;
	};

}
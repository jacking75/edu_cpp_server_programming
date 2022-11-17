#pragma once

#include <vector>
#include <string>

namespace NaveNetLib {

	typedef struct IPBAND
	{
		DWORD	start;
		DWORD	end;
	}*LPIPBAND;

	class NFIPSec
	{
	public:
		NFIPSec(void);
		~NFIPSec(void);

	public:
		void LoadAllowIP(wchar_t* lpIPFileName);
		void LoadBlockIP(wchar_t* lpIPFileName);
		void LoadAllowIPZ(wchar_t* lpIPFileName);
		void LoadBlockIPZ(wchar_t* lpIPFileName);

		int SerializeOut(int iType, int iPos, int iCount, char* lpBuffer_Out);
		void SerializeIn(int iType, int iCount, char* lpBuffer_In);

		inline void ClearAllowIP() { m_vecAllowIP.clear(); }
		inline void ClearBlockIP() { m_vecBlockIP.clear(); }

		bool IsAliveIP(const char * strIP);
		bool IsAliveIP(unsigned long dwIP);

		bool CheckAllowIP(const char * strIP);
		bool CheckAllowIP(unsigned long dwIP);

		bool CheckBlockIP(const char * strIP);
		bool CheckBlockIP(unsigned long dwIP);

		inline int GetAllowIPCount() { return (int)m_vecAllowIP.size(); }
		inline int GetBlockIPCount() { return (int)m_vecBlockIP.size(); }

	private:
		unsigned long IPStringToNumber(const char* pIPString);

		std::vector<std::string> Tokenizer(const std::string& input, const std::string& regex);

		std::vector<IPBAND> m_vecAllowIP;
		std::vector<IPBAND> m_vecBlockIP;

	};

}
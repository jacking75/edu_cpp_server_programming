//#include "NFTokenizer.h"
#include <regex>

#include "NaveServer.h"
#include "NFIPSec.h"

//#include <SDK/GZip/zlib.h>
//#pragma comment(lib,"zlib.lib")

namespace NaveNetLib {

	NFIPSec::NFIPSec(void)
	{
	}

	NFIPSec::~NFIPSec(void)
	{
	}

	void NFIPSec::LoadAllowIP(wchar_t* lpIPFileName)
	{
		m_vecAllowIP.clear();

		FILE * fp = nullptr;
		auto error = _wfopen_s(&fp, lpIPFileName, L"rt");
		if (error != 0) {
			return;
		}

		char strBuff[256];
		IPBAND ip;

		while (true)
		{
			fgets(strBuff, 256, fp);

			if (feof(fp)) {
				break;
			}

			auto strtok = Tokenizer(strBuff, ",");

			//int cnt = strtok.CountTokens();
			ip.start = IPStringToNumber(strtok[0].c_str());
			ip.start = htonl(ip.start);

			ip.end = IPStringToNumber(strtok[1].c_str());
			ip.end = htonl(ip.end);

			m_vecAllowIP.push_back(ip);
		}
		fclose(fp);
	}

	void NFIPSec::LoadBlockIP(wchar_t* lpIPFileName)
	{
		m_vecBlockIP.clear();

		FILE * fp = nullptr;
		auto error = _wfopen_s(&fp, lpIPFileName, L"rt");
		if (error != 0) {
			return;
		}

		char strBuff[256];
		IPBAND ip;

		while (true)
		{
			fgets(strBuff, 256, fp);

			if (feof(fp)) {
				break;
			}

			auto strtok = Tokenizer(strBuff, ",");

			//int cnt = strtok.CountTokens();
			ip.start = IPStringToNumber(strtok[0].c_str());
			ip.start = htonl(ip.start);

			ip.end = IPStringToNumber(strtok[1].c_str());
			ip.end = htonl(ip.end);

			m_vecBlockIP.push_back(ip);
		}

		fclose(fp);
	}

	bool GetLine(const CHAR* sBuf, INT maxsBuf, CHAR* tBuf, INT maxtBuf, LONG& index)
	{
		ZeroMemory(tBuf, sizeof(tBuf));
		INT DestIndex = index;

		CHAR* pLine = "\n";
		auto pLen = (INT)strlen(pLine);

		for (INT i = 0; i < maxsBuf; i++)
		{
			if (index + i >= maxsBuf) {
				break;
			}

			if (strncmp(&sBuf[index + i], pLine, pLen) == 0)
			{
				CopyMemory(tBuf, sBuf + index, i);
				index += (i + pLen);
				tBuf[i] = 0;
				return true;
			}
		}
		INT len = maxsBuf - index;
		if (maxtBuf <= len) {
			len = maxtBuf - index;
		}

		CopyMemory(tBuf, sBuf + index, len);
		tBuf[len] = 0;
		index += len;
		return false;
	}

	void NFIPSec::LoadAllowIPZ(wchar_t* lpIPFileName)
	{
		//TODO: 그냥 텍스트 파일 로딩 혹은 외부에서(DB or 웹서버) 정보를 가져오도록 한다.
		/*
		m_vecAllowIP.clear();

		FILE * fp = _wfopen(lpIPFileName, L"rb");
		if (!fp) {
			return;
		}

		long len;
		long datalen;
		char szMD5[40];
		memset(szMD5, 0, sizeof(szMD5));

		fread(&len, sizeof(long), 1, fp);
		fread(&datalen, sizeof(long), 1, fp);
		fread(szMD5, 32, 1, fp);

		char* fileDat = new char[datalen];
		fread(fileDat, datalen, 1, fp);

		fclose(fp);

		// MD5를 이용해 암호화를 합니다.
		for (long i = 0; i < datalen; ++i)
		{
			fileDat[i] = fileDat[i] ^ szMD5[i % 32];
		}

		char* outDat = new char[len * 2];

		// 파일을 디코딩 합니다.
		uLongf destLen = len * 2;
		uncompress((Bytef*)outDat, &destLen, (Bytef*)fileDat, datalen);
		outDat[destLen] = 0;

		if (fileDat) {
			delete[]fileDat;
		}

		LONG index = 0;
		char strBuff[256];
		IPBAND ip;

		bool bRet = true;
		while (bRet)
		{
			bRet = GetLine(outDat, destLen, strBuff, 256, index);

			if (strlen(strBuff) == 0) {
				break;
			}

			Nave::NFTokenizerA strtok = Nave::NFTokenizerA(strBuff, "\t");

			int cnt = strtok.CountTokens();
			ip.start = inet_addr(strtok.NextToken().c_str());
			ip.start = htonl(ip.start);

			ip.end = inet_addr(strtok.NextToken().c_str());
			ip.end = htonl(ip.end);

			m_vecAllowIP.push_back(ip);
		}

		if (outDat) {
			delete[]outDat;
		}
		*/
	}

	void NFIPSec::LoadBlockIPZ(wchar_t* lpIPFileName)
	{
		//TODO: 그냥 텍스트 파일 로딩 혹은 외부에서(DB or 웹서버) 정보를 가져오도록 한다.
		/*
		m_vecBlockIP.clear();

		FILE * fp = _wfopen(lpIPFileName, L"rb");
		if (!fp) {
			return;
		}

		long len;
		long datalen;
		char szMD5[40];
		memset(szMD5, 0, sizeof(szMD5));

		fread(&len, sizeof(long), 1, fp);
		fread(&datalen, sizeof(long), 1, fp);
		fread(szMD5, 32, 1, fp);

		char* fileDat = new char[datalen];
		fread(fileDat, datalen, 1, fp);

		fclose(fp);

		// MD5를 이용해 암호화를 합니다.
		for (long i = 0; i < datalen; ++i)
		{
			fileDat[i] = fileDat[i] ^ szMD5[i % 32];
		}

		char* outDat = new char[len * 2];

		// 파일을 디코딩 합니다.
		uLongf destLen = len * 2;
		uncompress((Bytef*)outDat, &destLen, (Bytef*)fileDat, datalen);
		outDat[destLen] = 0;

		if (fileDat) {
			delete[]fileDat;
		}

		LONG index = 0;
		char strBuff[256];
		IPBAND ip;

		bool bRet = true;
		while (bRet)
		{
			bRet = GetLine(outDat, destLen, strBuff, 256, index);

			if (strlen(strBuff) == 0) {
				break;
			}

			Nave::NFTokenizerA strtok = Nave::NFTokenizerA(strBuff, "\t");

			int cnt = strtok.CountTokens();
			ip.start = inet_addr(strtok.NextToken().c_str());
			ip.start = htonl(ip.start);

			ip.end = inet_addr(strtok.NextToken().c_str());
			ip.end = htonl(ip.end);

			m_vecBlockIP.push_back(ip);
		}

		if (outDat) {
			delete[]outDat;
		}
		*/
	}

	bool NFIPSec::IsAliveIP(const char * strIP)
	{
		return IsAliveIP(IPStringToNumber(strIP));
	}

	bool NFIPSec::IsAliveIP(unsigned long dwIP)
	{
		if (CheckBlockIP(dwIP)) {
			return false;
		}

		return CheckAllowIP(dwIP);
	}

	bool NFIPSec::CheckAllowIP(const char * strIP)
	{
		return CheckAllowIP(IPStringToNumber(strIP));
	}

	bool NFIPSec::CheckAllowIP(unsigned long dwIP)
	{
		// 바이트를 뒤집어준다.
		dwIP = htonl(dwIP);

		int iCount = (int)m_vecAllowIP.size();
		for (int i = 0; i < iCount; ++i)
		{
			if (m_vecAllowIP[i].start <= dwIP && dwIP <= m_vecAllowIP[i].end) {
				return true;
			}
		}

		return false;
	}

	bool NFIPSec::CheckBlockIP(const char * strIP)
	{
		return CheckBlockIP(IPStringToNumber(strIP));
	}

	bool NFIPSec::CheckBlockIP(unsigned long dwIP)
	{
		// 바이트를 뒤집어준다.
		dwIP = htonl(dwIP);

		int iCount = (int)m_vecBlockIP.size();
		for (int i = 0; i < iCount; ++i)
		{
			if (m_vecBlockIP[i].start <= dwIP && dwIP <= m_vecBlockIP[i].end) {
				return true;
			}
		}

		return false;
	}

	int NFIPSec::SerializeOut(int iType, int iPos, int iCount, char* lpBuffer_Out)
	{
		LPIPBAND ipOut = (LPIPBAND)lpBuffer_Out;
		LPIPBAND ipCur;

		int iLast = iPos + iCount;


		if (iType == 0)
		{
			if (m_vecAllowIP.empty()) {
				return 0;
			}

			if (iLast >= (int)m_vecAllowIP.size())
			{
				iLast = (int)m_vecAllowIP.size();
				iCount = iLast - iPos;
			}

			for (int i = iPos; i < iLast; ++i, ++ipOut)
			{
				ipCur = &m_vecAllowIP[i];
				ipOut->start = ipCur->start;
				ipOut->end = ipCur->end;
			}
			return iCount;
		}
		else if (iType == 1)
		{
			if (m_vecBlockIP.empty()) {
				return 0;
			}

			if (iLast >= (int)m_vecBlockIP.size())
			{
				iLast = (int)m_vecBlockIP.size();
				iCount = iLast - iPos;
			}

			for (int i = iPos; i < iLast; ++i, ++ipOut)
			{
				ipCur = &m_vecBlockIP[i];
				ipOut->start = ipCur->start;
				ipOut->end = ipCur->end;
			}

			return iCount;
		}

		return 0;
	}

	void NFIPSec::SerializeIn(int iType, int iCount, char* lpBuffer_In)
	{
		LPIPBAND ipIn = (LPIPBAND)lpBuffer_In;

		if (iType == 0)
		{
			for (int i = 0; i < iCount; ++i, ++ipIn) {
				m_vecAllowIP.push_back(*ipIn);
			}
		}
		else if (iType == 1)
		{
			for (int i = 0; i < iCount; ++i, ++ipIn) {
				m_vecBlockIP.push_back(*ipIn);
			}
		}
	}

	std::vector<std::string> NFIPSec::Tokenizer(const std::string& input, const std::string& regex)
	{
		std::regex re(regex);
		std::sregex_token_iterator
			first{ input.begin(), input.end(), re, -1 },
			last;
		return { first, last };
	}


	unsigned long NFIPSec::IPStringToNumber(const char* pIPString)
	{
		SOCKADDR_IN serveraddr;
		ZeroMemory(&serveraddr, sizeof(serveraddr));
		inet_pton(AF_INET, pIPString, (void *)&serveraddr.sin_addr.s_addr);
		auto upNumber = static_cast<unsigned long>(serveraddr.sin_addr.S_un.S_addr);

		return upNumber;
	}
}
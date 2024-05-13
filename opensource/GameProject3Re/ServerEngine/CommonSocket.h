#pragma once

#include <cstdint>
#include <string>

#ifdef WIN32
    #pragma comment(lib, "ws2_32")

    #include <WinSock2.h>
    #include <ws2tcpip.h>
    #include <mswsock.h>    
#endif

namespace hbServerEngine
{
#define SOCKET_ERROR        (-1)


int32_t GetSocketLastError()
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}


// 소켓을 재사용 가능한 상태로 설정
bool SetSocketReuseable(SOCKET hSocket)
{
    int nReuse = 1;

    if (0 != setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&nReuse, sizeof(int)))
    {
        return false;
    }

    return true;
}

bool SetSocketBlock(SOCKET hSocket, bool bBlock)
{
#ifdef WIN32
    u_long iMode = bBlock ? 0 : 1;
    ioctlsocket(hSocket, FIONBIO, &iMode);
#else
    int flags = fcntl(hSocket, F_GETFL, 0);
    fcntl(hSocket, F_SETFL, bBlock ? (flags & (~O_NONBLOCK)) : (flags | O_NONBLOCK));
#endif

    return true;
}

bool SetSocketNoDelay(SOCKET hSocket)
{
    int bOn = 1;

    if (0 != setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&bOn, sizeof(bOn)))
    {
        return false;
    }

    return true;
}

bool InitSocket()
{
#if WIN32
    WSADATA wsaData;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        return false;
    }
#else
    IgnoreSignal();
#endif
    return true;
}

bool UninitSocket()
{
#if WIN32
    return (0 == WSACleanup());
#endif
    return true;
}

SOCKET CreateSocket( int af = AF_INET, int type = SOCK_STREAM, int protocol = 0)
{
#ifdef WIN32
    return WSASocket(af, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
    return socket(af, type, protocol);
#endif
}

bool BindSocket( SOCKET hSocket, const struct sockaddr* pAddr, int nNamelen)
{
    if (0 != bind(hSocket, pAddr, nNamelen))
    {
        return false;
    }

    return true;
}

bool ListenSocket( SOCKET hSocket, int nBacklog)
{
    if (0 != listen(hSocket, nBacklog))
    {
        return false;
    }

    return true;
}

bool ConnectSocket(SOCKET hSocket, const char* pAddr, short sPort)
{
    if (pAddr == NULL)
    {
        return false;
    }

    sockaddr_in  svrAddr;
    memset(&svrAddr, 0, sizeof(svrAddr));
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(sPort);
    inet_pton(AF_INET, pAddr, &svrAddr.sin_addr);

    if (0 == connect(hSocket, (const sockaddr*)&svrAddr, sizeof(svrAddr)))
    {
        return true;
    }

    int nError = GetSocketLastError();
    if ((WSAEWOULDBLOCK == nError) || (WSAEINPROGRESS == nError))
    {
        return true;
    }

    return false;
}




bool IsSocketValid(SOCKET hSocket)
{
    if ((hSocket == 0) || (hSocket == INVALID_SOCKET))
    {
        return false;
    }

    return true;
}

void ShutdownSend(SOCKET hSocket)
{
    shutdown(hSocket, 1);
}

void ShutdownRecv(SOCKET hSocket)
{
    shutdown(hSocket, 0);
}

void CloseSocket(SOCKET hSocket)
{
#ifdef WIN32
    closesocket(hSocket);
#else
    close(hSocket);
#endif

    hSocket = INVALID_SOCKET;

    return;
}

void _ShutdownSocket(SOCKET hSocket)
{
#ifdef WIN32
    return CloseSocket(hSocket);
#else
    return CommonSocket::ShutdownSend(hSocket);
#endif
}

uint32_t IpAddrStrToInt(char* pszIpAddr)
{
    sockaddr_in SvrAddr;

    if (inet_pton(AF_INET, pszIpAddr, &SvrAddr.sin_addr) <= 0)
    {
        return 0;
    }

    return SvrAddr.sin_addr.s_addr;
}

uint32_t IpAddrStrToInt(const char* pszIpAddr)
{
    sockaddr_in SvrAddr;

    if (inet_pton(AF_INET, pszIpAddr, &SvrAddr.sin_addr) <= 0)
    {
        return 0;
    }

    return SvrAddr.sin_addr.s_addr;
}

std::string IpAddrIntToStr(uint32_t dwIpAddr)
{
    in_addr Addr;

    Addr.s_addr = dwIpAddr;

    CHAR szIpBuffer[100] = { 0 };
    inet_ntop(AF_INET, &Addr, szIpBuffer, 100);
    return std::string(szIpBuffer);
}

bool SetSocketBuffSize(SOCKET hSocket, int32_t nRecvSize, int32_t nSendSize)
{
    if (nRecvSize > 0)
    {
        if (0 != setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvSize, sizeof(int32_t)))
        {
            return false;
        }
    }

    if (nSendSize > 0)
    {
        if (0 != setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&nSendSize, sizeof(int32_t)))
        {
            return false;
        }
    }

    return true;
}

bool SetSocketTimeOut(SOCKET hSocket, int32_t nSendTime, int32_t nRecvTime)
{
    if (nSendTime > 0)
    {
#ifdef WIN32
        if (0 != setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nSendTime, sizeof(int32_t)))
        {
            return false;
        }
#else
        struct timeval timeout;
        timeout.tv_sec = nSendTime;
        timeout.tv_usec = 0;
        if (0 != setsockopt(hSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)))
        {
            return false;
        }
#endif
    }

    if (nRecvTime > 0)
    {
#ifdef WIN32
        if (0 != setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nRecvTime, sizeof(int32_t)))
        {
            return false;
        }
#else
        struct timeval timeout;
        timeout.tv_sec = nRecvTime;
        timeout.tv_usec = 0;
        if (0 != setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)))
        {
            return false;
        }
#endif
    }

    return true;
}

std::string GetRemoteIP(SOCKET hSocket)
{
    sockaddr_in     _sockAddr;

    socklen_t       _sockAddr_len = sizeof(_sockAddr);

    memset(&_sockAddr, 0, sizeof(_sockAddr));

    getpeername(hSocket, (sockaddr*)&_sockAddr, &_sockAddr_len);

    CHAR szIpBuffer[100] = { 0 };

    inet_ntop(AF_INET, &_sockAddr.sin_addr, szIpBuffer, 100);

    return std::string(szIpBuffer);
}

uint32_t HostToNet(int32_t nValue)
{
    return htonl(nValue);
}

uint32_t NetToHost(int32_t nValue)
{
    return ntohl(nValue);
}

void IgnoreSignal()
{
#ifdef WIN32

#else
    struct sigaction sig;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    sig.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sig, 0);
    sigaction(SIGHUP, &sig, 0);
#endif
}

std::string HttpGet(std::string strHost, int32_t nPort, std::string strPath, std::string strContent)
{
    char szReq[20480] = { 0 };
    sprintf_s(szReq, "GET %s?%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; zh-CN; rv:1.9.2.3) Gecko/20100401 Firefox/3.6.3\r\n\r\n",
        strPath.c_str(), strContent.c_str(), strHost.c_str());

    SOCKET hSocket = CreateSocket();
    if (hSocket == INVALID_SOCKET)
    {
        return "Create Socket Failed!";
    }

    if (!SetSocketTimeOut(hSocket, 2, 5))
    {
        CloseSocket(hSocket);
        return "Set Socket Time out Value Failed!";
    }

    if (!ConnectSocket(hSocket, strHost.c_str(), nPort))
    {
        CloseSocket(hSocket);
        return "Connect Socket Failed!";
    }

    send(hSocket, szReq, (int)strlen(szReq), 0);

    char szRecvBuff[10240] = { 0 };

    int32_t nDataLen = 0;
    int32_t nBytes;
    while (nBytes = recv(hSocket, szRecvBuff + nDataLen, 10240 - nDataLen, 0))
    {
        nDataLen += nBytes;
        if (nDataLen >= 10240)
        {
            break;
        }
    }

    CloseSocket(hSocket);

    std::string strRet = szRecvBuff;

    size_t  nPos = strRet.find("\r\n\r\n");
    if (nPos == std::string::npos)
    {
        return "";
    }

    return strRet.substr(nPos + 4);
}

#ifdef WIN32
bool ConnectSocketEx(SOCKET hSocket, const char* pAddr, short sPort, LPOVERLAPPED lpOverlapped)
{
    static LPFN_CONNECTEX lpfnConnectEx = NULL;

    if (lpfnConnectEx == NULL)
    {
        DWORD nBytes;
        GUID GuidConnectEx = WSAID_CONNECTEX;
        if (SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &GuidConnectEx, sizeof(GuidConnectEx),
            &lpfnConnectEx, sizeof(lpfnConnectEx),
            &nBytes, NULL, NULL))
        {
            return false;
        }
    }

    sockaddr_in  svrAddr;

    svrAddr.sin_family = AF_INET;

    svrAddr.sin_port = htons(0);

    svrAddr.sin_addr.s_addr = INADDR_ANY;

    BindSocket(hSocket, (const sockaddr*)&svrAddr, sizeof(sockaddr_in));

    svrAddr.sin_port = htons(sPort);

    inet_pton(AF_INET, pAddr, &svrAddr.sin_addr);

    if (!lpfnConnectEx(hSocket, (const sockaddr*)&svrAddr, sizeof(sockaddr_in), NULL, NULL, NULL, lpOverlapped))
    {
        if (ERROR_IO_PENDING != GetSocketLastError())
        {
            return false;
        }
    }

    return true;
}

bool AcceptSocketEx(SOCKET hListenSocket, SOCKET hAcceptSocket, CHAR* pBuff, LPOVERLAPPED lpOverlapped)
{
    static LPFN_ACCEPTEX lpfnAcceptEx = NULL;

    if (pBuff == NULL)
    {
        return false;
    }

    DWORD nBytes = 0;
    if (lpfnAcceptEx == NULL)
    {

        GUID GuidAcceptEx = WSAID_ACCEPTEX;
        if (SOCKET_ERROR == WSAIoctl(hListenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &GuidAcceptEx, sizeof(GuidAcceptEx),
            &lpfnAcceptEx, sizeof(lpfnAcceptEx),
            &nBytes, NULL, NULL))
        {
            return false;
        }
    }

    nBytes = 0;

    if (!lpfnAcceptEx(hListenSocket, hAcceptSocket, pBuff, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &nBytes, lpOverlapped))
    {
        if (ERROR_IO_PENDING != GetSocketLastError())
        {
            return false;
        }
    }

    return true;
}

bool GetSocketAddress(SOCKET hSocket, CHAR* pDataBuffer, sockaddr_in*& pAddrClient, sockaddr_in*& pAddrLocal)
{
    static  LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs = NULL;
    if (lpfnGetAcceptExSockaddrs == NULL)
    {
        DWORD nBytes;
        GUID GuidAddressEx = WSAID_GETACCEPTEXSOCKADDRS;
        if (SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &GuidAddressEx, sizeof(GuidAddressEx),
            &lpfnGetAcceptExSockaddrs, sizeof(lpfnGetAcceptExSockaddrs),
            &nBytes, NULL, NULL))
        {
            return false;
        }
    }

    sockaddr_in* pClient = NULL, * pLocal = NULL;
    int nAddrLen = sizeof(sockaddr_in);

    lpfnGetAcceptExSockaddrs(pDataBuffer, 0,
        sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
        (LPSOCKADDR*)&pLocal, &nAddrLen,
        (LPSOCKADDR*)&pClient, &nAddrLen);

    pAddrClient = pClient;
    pAddrLocal = pLocal;

    return true;
}

bool DisconnectEx(SOCKET hSocket, LPOVERLAPPED lpOverlapped, bool bReuse)
{
    static  LPFN_DISCONNECTEX lpfnDisconnectEx = NULL;

    if (lpfnDisconnectEx == NULL)
    {
        DWORD nBytes;
        GUID GuidAddressEx = WSAID_DISCONNECTEX;
        if (SOCKET_ERROR == WSAIoctl(hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
            &GuidAddressEx, sizeof(GuidAddressEx),
            &lpfnDisconnectEx, sizeof(lpfnDisconnectEx),
            &nBytes, NULL, NULL))
        {
            return false;
        }
    }

    lpfnDisconnectEx(hSocket, lpOverlapped, bReuse ? TF_REUSE_SOCKET : 0, 0);

    return true;
}

#endif
}


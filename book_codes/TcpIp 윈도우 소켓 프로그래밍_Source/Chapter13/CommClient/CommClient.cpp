#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512

void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

int main(int argc, char *argv[])
{
	int retval;

	// 직렬 포트 열기
	HANDLE hComm;
	hComm = CreateFile("COM1", GENERIC_READ|GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	if(hComm == INVALID_HANDLE_VALUE) err_quit("CreateFile()");

	// 직렬 포트 설정값 얻기
	DCB dcb;
	if(!GetCommState(hComm, &dcb)) err_quit("GetCommState()");

	// 직렬 포트 설정값 변경하기
	dcb.BaudRate = CBR_57600;
	dcb.ByteSize = 8;
	dcb.fParity = FALSE;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	if(!SetCommState(hComm, &dcb)) err_quit("SetCommState()");

	// 읽기와 쓰기 타임아웃 설정하기
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	if(!SetCommTimeouts(hComm, &timeouts)) err_quit("SetCommTimeouts()");

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE+1];
	int len;
	DWORD BytesRead, BytesWritten;

	// 서버와 데이터 통신
	while(1){
		// 데이터 입력
		ZeroMemory(buf, sizeof(buf));
		printf("\n[보낼 데이터] ");
		if(fgets(buf, BUFSIZE+1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if(buf[len-1] == '\n') buf[len-1] = '\0';
		if(strlen(buf) == 0) break;

		// 데이터 보내기
		retval = WriteFile(hComm, buf, BUFSIZE, &BytesWritten, NULL);
		if(retval == 0) err_quit("WriteFile()");
		printf("[클라이언트] %d바이트를 보냈습니다.\n", BytesWritten);

		// 데이터 받기
		retval = ReadFile(hComm, buf, BUFSIZE, &BytesRead, NULL);
		if(retval == 0) err_quit("ReadFile()");

		// 받은 데이터 출력
		buf[BytesRead] = '\0';
		printf("[클라이언트] %d바이트를 받았습니다.\n", BytesRead);
		printf("[받은 데이터] %s\n", buf);
	}

	// 직렬 포트 닫기
	CloseHandle(hComm);
	return 0;
}
#pragma once

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>
#include <stdio.h>
#include <time.h>

#include "commonDef.h"
#include "singleton.h"


//TOOD 유니코드로 바꾸기
namespace CQNetLib
{
	class WindowsService : public Singleton<WindowsService >
	{
	public:
		WindowsService()
		{
			m_dwCurrentStatus = 0;
			m_hServiceStatus = NULL;
			m_hLogFile = NULL;
			m_hEndEvent = CreateEvent(NULL, true, false, NULL);
		}

		virtual ~WindowsService()
		{
			m_hServiceStatus = NULL;
			if (m_hEndEvent != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hEndEvent);
				m_hEndEvent = INVALID_HANDLE_VALUE;
			}
		}

		//윈도우 서비스 외부 인터페이스
		void InitService(LPSTR szServiceName)
		{
			//Output( "call InitService" );
			//Output( szServiceName );
			m_szServiceName = szServiceName;
		}

		bool ServiceStart()
		{
			if (!CheckService())
				return false;

			Dispatch();

			return true;
		}
		
		/*-----------------------------------------------------------------------------------------------------*/
		//서비스 프로세스 컨트롤 관련 함수
		/*-----------------------------------------------------------------------------------------------------*/
		//서비스 프로세스 등록
		static	bool UploadService(WindowsService* self, const char* serviceName, const char* displayName, char* binaryPath,
			char* serviceLogin = 0, char* password = 0, bool autoStart = false)
		{
			SC_HANDLE	handleManager;
			SC_HANDLE	handleService;

			handleManager = ::OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
			if (handleManager == 0)
			{
				// 로그 살리기
				self->OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::UploadService() | OpenSCManager serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return false;
			}

			if (autoStart == true)
			{
				handleService = ::CreateServiceA(handleManager, serviceName, displayName, SERVICE_ALL_ACCESS,
				SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, binaryPath, 0, 0, 0, serviceLogin, password);
			}
			else
			{
				handleService = ::CreateServiceA(handleManager, serviceName, displayName, SERVICE_ALL_ACCESS,
					SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, binaryPath, 0, 0, 0, serviceLogin, password);
			}

			if (handleService == 0)
			{
				::CloseServiceHandle(handleManager);
				self->OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::UploadService() | CreateService serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return false;
			}

			::CloseServiceHandle(handleService);
			::CloseServiceHandle(handleManager);
			
			self->OutputLog(LogLevel::Info,"SYSTEM | cWindowsService::UploadService() | CreateService Success!! ServiceName(%s) DisplayName(%s)",serviceName, displayName);
			return true;
		}

		//서비스 프로세스 삭제
		static bool RemoveService(WindowsService* self, const char* serviceName)
		{
			SC_HANDLE	handleManager;
			SC_HANDLE	handleService;

			handleManager = ::OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
			if (handleManager == 0)
			{
				self->OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::RemoveService() | OpenSCManager serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return false;
			}

			handleService = ::OpenServiceA(handleManager, serviceName, DELETE);
			if (handleService == 0)
			{
				::CloseServiceHandle(handleManager);
				self->OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::RemoveService() | OpenService serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return false;
			}

			::DeleteService(handleService);

			::CloseServiceHandle(handleService);
			::CloseServiceHandle(handleManager);
			
			self->OutputLog(LogLevel::Info,"SYSTEM | cWindowsService::RemoveService() | RemoveService Success!! ServiceName(%s)",serviceName);
			return true;
		}

		//서비스 프로세스 시작
		UINT32 StartService(const char* netMachine, const char* serviceName)
		{
			UINT32 dwFailedReturnValue = static_cast<UINT32>(-1);
			SC_HANDLE	handleService;
			SC_HANDLE	handleServerConnect;

			handleService = ::OpenSCManagerA(netMachine, 0, SC_MANAGER_CONNECT);
			if (handleService == 0)
			{
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::StartService() | OpenSCManager serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return dwFailedReturnValue;
			}

			handleServerConnect = ::OpenServiceA(handleService, serviceName, SERVICE_START | SERVICE_QUERY_STATUS);
			if (handleServerConnect == 0)
			{
				::CloseServiceHandle(handleService);
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::StartService() | OpenService serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return dwFailedReturnValue;
			}

			if (::StartService(handleServerConnect, 0, 0) == FALSE)
			{
				::Sleep(_ssQueryControl.dwWaitHint);
				::QueryServiceStatus(handleServerConnect, &_ssQueryControl);
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::StartService() | StartService serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return dwFailedReturnValue;
			}

			::CloseServiceHandle(handleServerConnect);
			::CloseServiceHandle(handleService);

			OutputLog(LogLevel::Info,"SYSTEM | cWindowsService::StartService() | StartService Success!! ServiceName(%s)", serviceName);
			return _ssQueryControl.dwCurrentState;
		}

		//서비스 프로세스 종료
		UINT32 StopService(const char* netMachine, const char* serviceName)
		{
			UINT32 dwFailedReturnValue = static_cast<UINT32>(-1);
			SC_HANDLE	handleService;
			SC_HANDLE	handleServerConnect;

			handleService = ::OpenSCManagerA(netMachine, 0, SC_MANAGER_CONNECT);
			if (handleService == 0)
			{
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::StopService() | OpenSCManager serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return dwFailedReturnValue;
			}

			handleServerConnect = ::OpenServiceA(handleService, serviceName, GENERIC_EXECUTE);
			if (handleServerConnect == 0)
			{
				::CloseServiceHandle(handleService);
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::StopService() | OpenService serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return dwFailedReturnValue;
			}

			::ControlService(handleServerConnect, SERVICE_CONTROL_STOP, &_ssQueryControl);

			::CloseServiceHandle(handleServerConnect);
			::CloseServiceHandle(handleService);

			OutputLog(LogLevel::Info,"SYSTEM | cWindowsService::StartService() | StopService Success!! ServiceName(%s)",serviceName);
			return _ssQueryControl.dwCurrentState;
		}

		//서비스 프로세스 상태 가져오기
		UINT32 QueryService(const char* netMachine, const char* serviceName, QUERY_SERVICE_CONFIG* config, UINT32 bufSize)
		{
			SC_HANDLE	handleService;
			SC_HANDLE	handleServerConnect;

			handleService = ::OpenSCManagerA(netMachine, 0, GENERIC_READ);
			if (handleService == 0)
			{
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::QueryService() | OpenSCManager serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return 0;
			}

			//  해당 서비스의 정보를 얻어온다.
			if (config != 0)
			{
				DWORD		configBytes;
				SC_HANDLE	handleConfig;

				handleConfig = ::OpenServiceA(handleService, serviceName, SERVICE_QUERY_CONFIG);

				::QueryServiceConfig(handleConfig, config, bufSize, &configBytes);
				::CloseServiceHandle(handleConfig);
			}

			//  해당 서비스의 상태를 얻어온다.
			handleServerConnect = ::OpenServiceA(handleService, serviceName, SERVICE_INTERROGATE);
			if (handleServerConnect == 0)
			{
				::CloseServiceHandle(handleService);
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::QueryService() | OpenService serviceName(%s) ErrorCode(%d)", serviceName, GetLastError());
				return 0;
			}

			do
			{
				::ControlService(handleServerConnect, SERVICE_CONTROL_INTERROGATE, &_ssQueryControl);
			} while (_ssQueryControl.dwCurrentState != SERVICE_STOPPED &&
				_ssQueryControl.dwCurrentState != SERVICE_RUNNING &&
				_ssQueryControl.dwCurrentState != SERVICE_PAUSED &&
				_ssQueryControl.dwCurrentState != 1 && handleServerConnect != 0);

			switch (_ssQueryControl.dwCurrentState)
			{
			case SERVICE_STOPPED:
			case SERVICE_RUNNING:
			case SERVICE_PAUSED:
				break;
			default:
				_ssQueryControl.dwCurrentState = 0;
				break;
			}

			::CloseServiceHandle(handleServerConnect);
			::CloseServiceHandle(handleService);

			OutputLog(LogLevel::Info,"SYSTEM | cWindowsService::StartService() | QueryService Success!! ServiceName(%s)",serviceName);

			return _ssQueryControl.dwCurrentState;
		}


		// 서비스 실행 관련
		bool CheckService()
		{
			SC_HANDLE hdlSCM = OpenSCManager(0, 0, STANDARD_RIGHTS_REQUIRED);

			if (!hdlSCM)
				return false;

			SC_HANDLE hdlServ = OpenServiceA(hdlSCM, m_szServiceName, SERVICE_ALL_ACCESS);

			bool bRet = false;

			if (hdlServ)
			{
				bRet = true;
			}

			CloseServiceHandle(hdlServ);

			return bRet;
		}

		//윈도우 서비스 Dispatch 함수 (윈도우 서비스에 등록되어 있어야 한다.)
		void Dispatch()
		{
			SERVICE_TABLE_ENTRYA DispTbl[] =
			{
				{ m_szServiceName, (LPSERVICE_MAIN_FUNCTIONA)ApiServiceMainStarter },
				{ 0 , 0}
			};

			if (!::StartServiceCtrlDispatcherA(DispTbl)) // does not return until service stopped
			{
				INT32 Err = ::GetLastError();
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::Dispatch() | StartServiceCtrlDispatcher Error : %d", Err);
			}

		}
		
		//윈도우 서비스 시작 함수 (called by Windows)
		void ServiceStart(UINT32 argc, LPTSTR *argv)
		{
			UNREFERENCED_PARAMETER(argv);

			if (argc != 100)
			{
				m_hServiceStatus = ::RegisterServiceCtrlHandlerA(m_szServiceName, (LPHANDLER_FUNCTION)ApiServiceControlHandler);

				if (!m_hServiceStatus)
				{
					INT32 Err = ::GetLastError();
					OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::ServiceStart() | RegisterServiceHandler Error : %d", Err);
					return;
				}

				ServiceControlHandler(SERVICE_RUNNING); // Set status Running
			}
			
			OutputLog(LogLevel::Info, "SYSTEM | cWindowsService::ServiceStart() | Service Start..");

			WaitForSingleObject(m_hEndEvent, INFINITE);
			ServiceControlHandler(SERVICE_CONTROL_SHUTDOWN);
			OutputLog(LogLevel::Info, "SYSTEM | cWindowsService::ServiceStart() | Service End..");
		}
		
		//윈도우 서비스 핸들러로 서비스의 이벤트를 처리한다. (시작/종료)
		void ServiceControlHandler(UINT32 Opcode)
		{
			SERVICE_STATUS  strctStat;
			strctStat.dwServiceType = SERVICE_WIN32;
			strctStat.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
			strctStat.dwWin32ExitCode = NO_ERROR;
			strctStat.dwServiceSpecificExitCode = 0; // returned Status code
			strctStat.dwCheckPoint = 0;      // returned Error code
			strctStat.dwWaitHint = 0;

			if (Opcode == SERVICE_CONTROL_STOP)
			{
				strctStat.dwCurrentState = SERVICE_STOPPED;
				OutputLog(LogLevel::Info, "SYSTEM | cWindowsService::ServiceControlHandler() | Service Stop..");
			}
			else if (Opcode == SERVICE_CONTROL_SHUTDOWN)
			{
				strctStat.dwCurrentState = SERVICE_STOPPED;
				OutputLog(LogLevel::Info, "SYSTEM | cWindowsService::ServiceControlHandler() | Service Shutdown..");
			}
			else if (Opcode == SERVICE_CONTROL_INTERROGATE)
			{
				strctStat.dwCurrentState = SERVICE_RUNNING;
				OutputLog(LogLevel::Info, "SYSTEM | cWindowsService::ServiceControlHandler() | Service Interrogate..");
			}
			else
			{
				OutputLog(LogLevel::Info, "SYSTEM | cWindowsService::ServiceControlHandler() | Service unknown Opcode(%d)", Opcode);
			}


			if (!SetServiceStatus(m_hServiceStatus, &strctStat))
			{
				INT32 Err = ::GetLastError();
				OutputLog(LogLevel::Error, "SYSTEM | cWindowsService::ServiceControlHandler() | Set Status Error : %d", Err);
				return;
			}

			OutputLog(LogLevel::Info, "SYSTEM | cWindowsService::ServiceControlHandler() | SetStatus : %d", Opcode);

		}
		
		void SetServiceCurrentStatus(UINT32 dwStatus, UINT32 dwAccept = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN)
		{
			SERVICE_STATUS	service_status;

			service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
			service_status.dwCurrentState = dwStatus;
			service_status.dwControlsAccepted = dwAccept;
			service_status.dwWin32ExitCode = 0;
			service_status.dwServiceSpecificExitCode = 0; // returned Status code
			service_status.dwCheckPoint = 0;      // returned Error code
			service_status.dwWaitHint = 0;

			//현재 상태 보관
			m_dwCurrentStatus = dwStatus;

			SetServiceStatus(m_hServiceStatus, &service_status);
		}
			   		
		BOOL						SetEndEvent() { return SetEvent(m_hEndEvent); }
		HANDLE						GetEndEvent() { return m_hEndEvent; }


		//로그 관련
		bool CreateLog(const char* szLogFile) { return UNREFERENCED_PARAMETER(szLogFile);  true; }
		
		void OutputLog(LogLevel logLevel, const char* szOutputString, ...)
		{
			if (NULL == m_hLogFile) {
				return;
			}

			const int MAX_OUTPUT_LENGTH = 512;
			char g_szOutStr[MAX_OUTPUT_LENGTH] = "";


			va_list	argptr;
			va_start(argptr, szOutputString);
			_vsnprintf_s(g_szOutStr, MAX_OUTPUT_LENGTH, szOutputString, argptr);
			va_end(argptr);

			char        szTime[25];
			time_t      curTime;
			struct tm   locTime;
						
			curTime = time(NULL);
			localtime_s(&locTime, &curTime);
			strftime(szTime, 25, "%Y/%m/%d(%H:%M:%S)", &locTime);
			g_szOutStr[MAX_OUTPUT_LENGTH - 1] = NULL;
			szTime[24] = NULL;

			_snprintf_s(m_szOutputLog, MAX_OUTPUT_LENGTH * 2, "%s | %s | %s | %s%c%c"
				, szTime, ((int)logLevel >> 4) ? "에러" : "정보"
				, g_LogLevelStr[(int)logLevel]
				, g_szOutStr, 0x0d, 0x0a);


			DWORD dwWrittenBytes = 0;

			//화일에 끝으로 파일 포인터를 옮긴다. 
			SetFilePointer(m_hLogFile, 0, 0, FILE_END);
			/*BOOL bRet = */WriteFile(m_hLogFile, m_szOutputLog, (DWORD)strlen(m_szOutputLog), &dwWrittenBytes, NULL);
		}
	
	
	private:	

		LPSTR						m_szServiceName;
		UINT32						m_dwCurrentStatus;
		SERVICE_STATUS_HANDLE		m_hServiceStatus;
		SERVICE_STATUS				_ssQueryControl;
		HANDLE						m_hEndEvent;

		HANDLE						m_hLogFile;
				
		char						m_szOutputLog[512];
	};



	#define g_pWindowsService					WindowsService::GetSingleton()


	/*------------------------------------------------------------------------------------------
	 * 윈도우 서비스 시작 함수 (Windows API callback method)
	 *-----------------------------------------------------------------------------------------*/
	void ApiServiceMainStarter(UINT32 argc, LPTSTR* argv)
	{
		g_pWindowsService->ServiceStart(argc, argv);
	}

	/*------------------------------------------------------------------------------------------
	 * 윈도우 서비스 핸들러 함수 (Windows API callback method)
	 *-----------------------------------------------------------------------------------------*/

	void ApiServiceControlHandler(UINT32 Opcode)
	{
		g_pWindowsService->ServiceControlHandler(Opcode);
	}
}
#pragma once

#include <CommonFunc.h>
#include <Log.h>
#include <ConfigFile.h>
#include <IBufferHandler.h>
#include <ServiceBase.h>

#include <thread>
#include <chrono>


class EchoServer : public hbServerEngine::INetEventDispatcher
{
public:
	EchoServer()
	{
	}

	~EchoServer()
	{
	}

	bool Init()
	{
        p_logger_ = hbServerEngine::CLog::GetInstancePtr();

        CommonFunc::SetCurrentWorkDir("");

        if (p_logger_->Start("EchoServer", "log") == false)
        {
            return false;
        }


        p_logger_->LogInfo("---------서버 시작-----------");
        auto p_config_file = hbServerEngine::ConfigFile::GetInstancePtr();

        if (p_config_file->Load("servercfg.ini") == false)
        {
            p_logger_->LogError("load error servercfg.ini !");
            return false;
        }

        if (CommonFunc::IsAlreadyRun("EchoServer"))
        {
            p_logger_->LogError("EchoServer 중복 실행!");
            return FALSE;
        }

        p_logger_->SetLogLevel(p_config_file->GetIntValue("echo_log_level"));

        auto nPort = (uint16_t)p_config_file->GetIntValue("echo_svr_port");
        if (nPort <= 0)
        {
            p_logger_->LogError("invalid echo_svr_port");
            return FALSE;
        }

        auto server_ip = p_config_file->GetStringValue("echo_svr_ip");
        int32_t  nMaxConn = p_config_file->GetIntValue("echo_svr_max_con");
        auto p_service = hbServerEngine::ServiceBase::GetInstancePtr();
        if (p_service->StartNetwork(nPort, nMaxConn, this, server_ip) == false)
        {
            p_logger_->LogError("failed StartNetwork!");
            return false;
        }
                
        p_logger_->LogHiInfo("---------서버 시작 성공!--------");

        return true;
	}

	bool Uninit()
	{
        p_logger_->LogError("==========Uninit() begin=======================");

        hbServerEngine::ServiceBase::GetInstancePtr()->StopNetwork();
        
        p_logger_->LogError("==========Uninit() end=======================");

        return true;
	}

    // 1개의 스레드에서만 호출 되어야 한다
	bool Run()
	{
        while (true)
        {
            auto count = hbServerEngine::ServiceBase::GetInstancePtr()->Update();

            if (count <= 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        return true;
	}


    bool DispatchPacket(hbServerEngine::NetPacket* pNetPacket) override
    {
        return true;
    }

    bool OnCloseConnect(int32_t nConnID) override
    {
        return true;
    }

    bool OnNewConnect(int32_t nConnID) override
    {
        return true;
    }


private:	
    hbServerEngine::CLog* p_logger_ = nullptr;
};
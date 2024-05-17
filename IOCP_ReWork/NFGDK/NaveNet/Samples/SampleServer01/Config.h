#pragma once

#include <string>

//typedef struct GLOBAL
//{
//	std::string	Title;
//	unsigned int	ID;
//	unsigned int	Port;
//	unsigned int	MaxConn;
//	unsigned int	LogSave;
//	std::string	Version;
//
//	void Serialize() const
//	{
//	}
//
//	void Deserialize() 
//	{
//		Title = "SampleServer01";
//		ID = 1;
//		Port = 23452;
//		MaxConn = 100;
//		LogSave = 1;
//		Version = "1";
//	}
//}GLOBAL, *LPGLOBAL;
//
//typedef struct CONFIG
//{
//	GLOBAL		Global;
//
//	void Serialize() const
//	{
//	}
//
//	void Deserialize()
//	{
//		Global.Deserialize();
//	}
//}CONFIG, *LPCONFIG;

class Config
{
public:
	Config(void) = default;
	~Config(void) = default;

	const char*	GetTitle() { return mTitle.c_str(); };
	unsigned int	GetID() { return mID; };
	unsigned int	GetPort() { return mPort; };
	unsigned int	GetMaxConn() { return mMaxConn; };
	unsigned int	GetLogSave() { return mLogSave; };
	const char*	GetVersion() { return mVersion.c_str(); };

private:
	//CONFIG	m_Config;

	std::string	mTitle = "SampleServer01";
	unsigned int	mID = 1;
	unsigned int	mPort = 32452;
	unsigned int	mMaxConn = 100;
	unsigned int	mLogSave = 1;
	std::string	mVersion = "1";
};

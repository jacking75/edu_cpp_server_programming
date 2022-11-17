#pragma once

#include <string>

typedef struct GLOBAL
{
	std::string	Title;
	unsigned int	ID;
	unsigned int	Port;
	unsigned int	MaxConn;
	unsigned int	LogSave;
	std::string	Version;

	void Serialize() const
	{
	}

	void Deserialize() 
	{
		Title = "SampleServer01";
		ID = 1;
		Port = 23452;
		MaxConn = 100;
		LogSave = 1;
		Version = "1";
	}
}GLOBAL, *LPGLOBAL;

typedef struct CONFIG
{
	// 이팩트에는 가속이 없다.
	GLOBAL		Global;

	void Serialize() const
	{
	}

	void Deserialize()
	{
		Global.Deserialize();
	}
}CONFIG, *LPCONFIG;

class Config
{
public:
	Config(void) = default;
	~Config(void) = default;

	void	Load(const wchar_t* strFile = L"Config.xml") { m_Config.Deserialize(); }
	void	Save(const wchar_t* strFile = L"Config.xml") {}

	const char*	GetTitle() { return m_Config.Global.Title.c_str(); };
	unsigned int	GetID() { return m_Config.Global.ID; };
	unsigned int	GetPort() { return m_Config.Global.Port; };
	unsigned int	GetMaxConn() { return m_Config.Global.MaxConn; };
	unsigned int	GetLogSave() { return m_Config.Global.LogSave; };
	const char*	GetVersion() { return m_Config.Global.Version.c_str(); };

private:
	CONFIG	m_Config;
};

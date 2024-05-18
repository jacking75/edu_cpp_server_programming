#pragma once

namespace CQNetLib
{
#define DECLEAR_SINGLETON( className )\
public:\
	static className* Instance();\
	virtual void releaseInstance();\
private:\
	static className* m_pInstance;


	template <typename T>
	class Singleton
	{
	public:
		Singleton(void) {}
		virtual ~Singleton(void) {}

		static T*   GetSingleton(void)
		{
			if (nullptr == ms_Singleton)
				ms_Singleton = new T;
			return (ms_Singleton);
		}

		inline static void Release()
		{
			SAFE_DELETE(ms_Singleton);
		}


	private:
		static T* ms_Singleton;

	};


	template <typename T> T* Singleton<T>::ms_Singleton = nullptr;

}
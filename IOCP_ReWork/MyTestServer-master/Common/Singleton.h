#pragma once

///////////////////////////////////////////////////////////////////////////////
// 싱글톤 상속용 클래스
// <Derived Class>
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class CSingleton
{	
public:
	/// 해당 파생 클래스에 대한 인스턴스를 가지고 온다
	static T& GetInstance()
	{
		static T instance;    
		return instance;
	}
	
private:
	CSingleton(const CSingleton& val_) {}
	const CSingleton& operator=(const CSingleton& val_) {}

protected:
	CSingleton() {}
	virtual ~CSingleton() {}
};


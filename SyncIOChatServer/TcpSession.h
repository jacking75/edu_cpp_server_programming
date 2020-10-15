#include "ErrorCode.h"

namespace NServerNetLib
{
	class TcpSession
	{
	public:
		TcpSession();
		virtual ~TcpSession();

		void Init(const int index);
		
		

	private:
		int mIndex = 0; 
	};
}

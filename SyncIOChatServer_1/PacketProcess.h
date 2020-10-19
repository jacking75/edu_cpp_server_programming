#pragma once

#include <memory>
#include "TcpNetwork.h"


namespace ChatServerLib
{		
	class PacketProcess
	{		
	public:
		PacketProcess();
		~PacketProcess();

		void Init(NServerNetLib::TcpNetwork* pNetwork);

		void Process();

	
	private:
		NServerNetLib::TcpNetwork* m_pRefNetwork;
				
								
	private:
		
	};
}
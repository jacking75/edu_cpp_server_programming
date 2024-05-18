#define MAX_CHATMSG 300
#define MAX_IP		20

enum ePacket{

	PACKET_CHAT				=	1,	
};

struct Packet_Chat
{
	unsigned int	s_nLength;
	unsigned short	s_sType;
	char			s_szIP[ MAX_IP ];
	char			s_szChatMsg[ MAX_CHATMSG ];
};
	


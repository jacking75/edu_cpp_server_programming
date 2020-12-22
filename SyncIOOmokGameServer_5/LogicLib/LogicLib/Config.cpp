#include "Config.h"
#include <iostream>
#include <fstream>


using namespace std::string_view_literals;

namespace OmokServerLib
{
	ERROR_CODE Config::Load()
	{
		auto config = toml::parse_file("configuration.toml");

		redisAddress = config["ServerConfig"]["RedisAddress"].value<std::string>();
		redisPortNum = config["ServerConfig"]["RedisPortNum"].value_or(0);
		port = config["ServerConfig"]["Port"].value_or(0);
		maxClientCount = config["ServerConfig"]["MaxClientCount"].value_or(0);
		extraClientCount = config["ServerConfig"]["ExtraClientCount"].value_or(0);
		backLogCount = config["ServerConfig"]["BackLogCount"].value_or(0);
		maxClientRecvBufferSize = config["ServerConfig"]["MaxClientRecvBufferSize"].value_or(0);
		maxClientSendBufferSize = config["ServerConfig"]["MaxClientSendBufferSize"].value_or(0);
		maxRoomUserCount = config["ServerConfig"]["MaxRoomUserCount"].value_or(0);
		maxRoomCountByLobby = config["ServerConfig"]["MaxRoomCountByLobby"].value_or(0);
		
		
		return ERROR_CODE::NONE;

	}

}

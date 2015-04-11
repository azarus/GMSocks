#define GMMODULE
#include "GarrysMod/Lua/Interface.h"
#include <stdio.h>
#include "CSocket.h"
#include "CAddress.h"

using namespace GarrysMod::Lua;
#define GMOD_FUNCTION(fn) int fn(lua_State* state)
CSocket *UDPSock = 0;

GMOD_FUNCTION(UDPSend)
{
	int ServerPort = (int)LUA->GetNumber(1);
	const char *ServerIP = LUA->GetString(2);
	const char *Buffer = LUA->GetString(3);
	int Length = (int)LUA->GetNumber(4);
	CAddress addr(ServerIP, ServerPort, SOCK_DGRAM);

	int Sent = UDPSock->sendPacket((char *)Buffer, Length, &addr);
	LUA->PushNumber(Sent);
	return 1;
}

GMOD_FUNCTION(UDPReceive)
{

	int ServerPort = (int)LUA->GetNumber(1);
	const char *ServerIP = LUA->GetString(2);



	CAddress addr (ServerIP, ServerPort, SOCK_DGRAM);
	char g_szBuffer[2048];
	sprintf(g_szBuffer, "");
	int nReturn = UDPSock->receivePacket(g_szBuffer, 2048, &addr);
	if (nReturn > 0)
	{
		LUA->PushString(g_szBuffer, nReturn);
		return 1;
	}
	LUA->PushNil();
	

	return 1;
}

//
// Called when you module is opened
//
GMOD_MODULE_OPEN()
{
	
	UDPSock = new CSocket();
	UDPSock->initialize(SOCK_DGRAM, IPPROTO_UDP);
	UDPSock->setBlocking(false);


	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);	// Push global table
	LUA->PushString("UDPSend");					// Push Name
	LUA->PushCFunction(UDPSend);			// Push function
	LUA->SetTable(-3);								// Set the table 

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->PushString("UDPReceive");					// Push Name
	LUA->PushCFunction(UDPReceive);			// Push function
	LUA->SetTable(-3);								// Set the table 

	return 0;
}

//
// Called when your module is closed
//
GMOD_MODULE_CLOSE()
{
	delete UDPSock;
	return 0;
}

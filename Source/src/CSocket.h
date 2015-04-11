#ifndef H_CSOCKET
#define H_CSOCKET

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef WIN32
#define WIN32
#endif
#include <WinSock.h>
#include <Windows.h>
#define SOCKET_WOULDBLOCK WSAEWOULDBLOCK
#endif

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#define INVALID_SOCKET -1
typedef int SOCKET;
typedef timeval TIMEVAL;
#define WOULDBLOCK EWOULDBLOCK
#endif

#include "ISocket.h"

class CSocket : public ISocket
{
private:
	int m_iAF;
	int m_iType;
	int m_iProtocol;
	bool m_bBlocking;
	unsigned int m_iTimeout;
	SOCKET m_sSocket;
public:
	CSocket();
	CSocket(int type, int protocol, int af=AF_INET);
	virtual bool initialize(int type, int protocol, int af=AF_INET);
	virtual bool isInitialized();
	virtual bool setBlocking(bool blocking);
	virtual bool setNoDelay(bool nodelay);
	virtual bool bindAddress(IAddress * addr);
	virtual bool connectAddress(IAddress * addr);
	virtual bool startListening(int backlog);
	virtual int sendPacket(char * buf, int buflen = -1, IAddress * dest = 0);
	virtual int receivePacket(char * buf, int buflen, IAddress * from = 0);
	virtual ISocket * acceptIncoming(IAddress * from=0);
	virtual int getLastError();
	void setSocket(SOCKET sock);
	virtual void setTimeout(unsigned int seconds);
	virtual ~CSocket();
};

#endif

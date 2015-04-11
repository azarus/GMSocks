#ifndef H_CADDRESS
#define H_CADDRESS

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "IAddress.h"

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

#endif

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

class CAddress : public IAddress
{
private:
	int m_iAF;
	sockaddr_in m_sAddr;
public:
	CAddress(sockaddr_in addr);
	CAddress(const char * ip = 0, unsigned int port = 0, int type = AF_INET);
	virtual bool setAddress(void * addr);
	virtual bool setAddress(const char * ip = 0, unsigned int port = 0, int type = AF_INET);
	virtual void * getSockAddr();
	virtual bool getIP(char * out, unsigned int maxlen);
	virtual unsigned int getPort();
};

#endif

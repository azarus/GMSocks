#include "CSocket.h"
#ifndef _WIN32
#include <netinet/tcp.h>
#endif
#include <iostream>
#include <cstring>

CSocket::CSocket()
{
	m_iType = -1;
	m_iProtocol = -1;
	m_iAF = -1;
	m_bBlocking = true;
	m_iTimeout = 0;

	m_sSocket = INVALID_SOCKET;
}

CSocket::CSocket(int type, int protocol, int af)
{
	m_bBlocking = true;
	m_iTimeout = 0;
	initialize(type, protocol, af);
}

bool CSocket::initialize(int type, int protocol, int af)
{
	m_iType = type;
	m_iProtocol = protocol;
	m_iAF = af;

#if defined _WIN32
	WSAData m_sData;
	WSAStartup(MAKEWORD(2,2), &m_sData);
#endif

	m_sSocket = socket(af, type, protocol);
	if(m_sSocket == INVALID_SOCKET)
		return false;
	return true;
}

bool CSocket::isInitialized()
{
	if(m_sSocket == INVALID_SOCKET)
		return false;
	return true;
}

bool CSocket::setNoDelay(bool nodelay)
{
	int flag = nodelay;
	int result = setsockopt(m_sSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
	if (result < 0)
		return false;
	return true;
}

bool CSocket::setBlocking(bool blocking)
{
#if defined _WIN32
	unsigned long nonBlocking;
	if(blocking)
		nonBlocking = 0;
	else
		nonBlocking = 1;
	if(ioctlsocket(m_sSocket, FIONBIO, &nonBlocking ) != NO_ERROR)
		return false;
#else
	int m_iFlags = fcntl(m_sSocket, F_GETFL, 0);
	if(blocking)
		m_iFlags &= ~O_NONBLOCK;
	else
		m_iFlags |= O_NONBLOCK;
	if(fcntl(m_sSocket, F_SETFL, m_iFlags) == -1)
		return false;
#endif
	
	m_bBlocking = blocking;
	return true;
}

bool CSocket::bindAddress(IAddress * addr)
{
	if(addr == 0)
		return false;
	if(bind(m_sSocket, (sockaddr*)addr->getSockAddr(), sizeof(sockaddr)) == 0)
		return true;
	return false;
}

bool CSocket::startListening(int backlog)
{
	if(listen(m_sSocket, backlog)==0)
		return true;
	return false;
}

bool CSocket::connectAddress(IAddress * addr)
{
	if(addr == 0)
		return false;
	bool blocking = m_bBlocking;
	if(m_iTimeout > 0)
	{
		setBlocking(false);
		if(connect(m_sSocket, (sockaddr*)addr->getSockAddr(), sizeof(sockaddr)) == -1)
		{
			timeval m_tTimeout;
			m_tTimeout.tv_sec = m_iTimeout;
			m_tTimeout.tv_usec = 0;
			fd_set m_fdWrite;
			FD_ZERO(&m_fdWrite);
			FD_SET(m_sSocket, &m_fdWrite);
 
			select(0,0,&m_fdWrite,0,&m_tTimeout);			
			if(FD_ISSET(m_sSocket, &m_fdWrite)) 
			{
				setBlocking(blocking);
				return true;
			}
		}
	}
	else if(connect(m_sSocket, (sockaddr*)addr->getSockAddr(), sizeof(sockaddr)) == 0)
		return true;
	return false;
}

void CSocket::setTimeout(unsigned int seconds)
{
	m_iTimeout = seconds;
}

#if defined _WIN32
void CSocket::setSocket(SOCKET sock)
#else
void CSocket::setSocket(int sock)
#endif
{
	m_sSocket = sock;
}

int CSocket::sendPacket(char * buf, int buflen, IAddress * dest)
{
	if(buf == 0)
		return -1;
	if(m_iType == SOCK_DGRAM)
		if(dest == 0)
			return -1;
		else
			return sendto(m_sSocket, buf, (buflen == -1?strlen(buf)+1:buflen), 0, (sockaddr*)dest->getSockAddr(), sizeof(sockaddr));
	else
		return send(m_sSocket, buf, (buflen == -1?strlen(buf)+1:buflen), 0);
}

int CSocket::receivePacket(char * buf, int buflen, IAddress * from)
{
	if (buf == 0)
	{
		printf("buf is not set\n");
		return -1;
	}
	int m_iBytes = -1;
	if(m_iTimeout > 0 && m_bBlocking)
	{
		TIMEVAL m_tTimeout;
		m_tTimeout.tv_sec = m_iTimeout;
		m_tTimeout.tv_usec = 0;
		fd_set m_fdRead;
		FD_ZERO(&m_fdRead);
		FD_SET(m_sSocket, &m_fdRead);
 
		select(0,&m_fdRead,0,0,&m_tTimeout);			
		if (!FD_ISSET(m_sSocket, &m_fdRead))
		{
			printf("!FD_ISSET\n");
			return -1;
		}
	}
	if(m_iType == SOCK_DGRAM)
	{
#if defined _WIN32
		int m_iLen = sizeof(sockaddr);
#else
		unsigned int m_iLen = sizeof(sockaddr);
#endif
		sockaddr_in m_sFrom;
		m_iBytes = recvfrom(m_sSocket, buf, buflen, 0, (sockaddr*)&m_sFrom, &m_iLen);
		
		if(from != 0)
			from->setAddress((void*)(&m_sFrom));
	}
	else
		m_iBytes = recv(m_sSocket, buf, buflen, 0);
	return m_iBytes;
}

ISocket * CSocket::acceptIncoming(IAddress * from)
{
	SOCKET m_sIncoming;
	if(from == 0)
		m_sIncoming = accept(m_sSocket, NULL, NULL);
	else
	{
		sockaddr_in m_sAddr;
#if defined _WIN32
		int m_iLen = sizeof(sockaddr);
#else
		unsigned int m_iLen = sizeof(sockaddr);
#endif
		m_sIncoming = accept(m_sSocket, (sockaddr*)&m_sAddr, &m_iLen);
		if(m_sIncoming != -1)
			from->setAddress((void*)(&m_sAddr));
	}
	
	if(m_sIncoming == -1)
		return 0;
	CSocket * m_pRet = new CSocket();
	m_pRet->setSocket(m_sIncoming);
	return m_pRet;
}

int CSocket::getLastError()
{
#if defined _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

CSocket::~CSocket()
{
#if defined _WIN32
	closesocket(m_sSocket);
#else
	close(m_sSocket);
#endif
}
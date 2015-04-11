#include "CAddress.h"

CAddress::CAddress(sockaddr_in addr)
{
	setAddress((void*)&addr);
}

CAddress::CAddress(const char * ip, unsigned int port, int type)
{
	setAddress(ip, port, type);
}

bool CAddress::setAddress(void* addr)
{
	memset(&m_sAddr, 0, sizeof(m_sAddr));

	memcpy(&m_sAddr, addr, sizeof(sockaddr_in));
	m_iAF = m_sAddr.sin_family;
	return true;
}

bool CAddress::setAddress(const char * ip, unsigned int port, int type)
{
	memset(&m_sAddr, 0, sizeof(m_sAddr));
	m_iAF = -1;
	if(ip == 0)
		return false;
	m_sAddr.sin_family = AF_INET;
	m_sAddr.sin_addr.s_addr = inet_addr(ip);
	m_sAddr.sin_port = htons(port);
	m_iAF = type;
	return true;
}

void * CAddress::getSockAddr()
{
	if(m_iAF == -1)
		return 0;
	return &m_sAddr;
}

bool CAddress::getIP(char * out, unsigned int maxlen)
{
	if(m_iAF == -1)
		return false;
	int m_iParts[4];
	for(int i=0;i<4;++i)
		m_iParts[i] = m_sAddr.sin_addr.s_addr >> (i)*8 & 0xFF;
	char m_szIP[16];
	int len = sprintf(m_szIP, "%d.%d.%d.%d", m_iParts[0], m_iParts[1], m_iParts[2], m_iParts[3])+1;
	if(maxlen < len)
		len = maxlen;
	memcpy(out, m_szIP, len);
	return true;
}

unsigned int CAddress::getPort()
{
	if(m_iAF == -1)
		return 0;
	return htons(m_sAddr.sin_port);
}

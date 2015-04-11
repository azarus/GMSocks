#ifndef H_ISOCKET
#define H_ISOCKET

#include "IAddress.h"

class ISocket
{
public:
	virtual bool initialize(int type, int protocol, int af)=0;
	virtual bool isInitialized()=0;
	virtual bool setBlocking(bool blocking)=0;
	virtual bool setNoDelay(bool nodelay)=0;
	virtual bool bindAddress(IAddress * addr)=0;
	virtual bool connectAddress(IAddress * addr)=0;
	virtual bool startListening(int backlog)=0;
	virtual int sendPacket(char * buf, int buflen = -1, IAddress * dest = 0)=0;
	virtual int receivePacket(char * buf, int buflen, IAddress * from = 0)=0;
	virtual ISocket * acceptIncoming(IAddress * from=0)=0;
	virtual int getLastError()=0;
	virtual void setTimeout(unsigned int seconds)=0;
	virtual ~ISocket(){};
};

#endif
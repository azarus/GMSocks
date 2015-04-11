#ifndef H_IADDRESS
#define H_IADDRESS

class IAddress
{
public:
	virtual bool setAddress(void * addr)=0;
	virtual bool setAddress(const char * ip, unsigned int port, int type)=0;
	virtual void * getSockAddr()=0;
	virtual bool getIP(char * out, unsigned int maxlen)=0;
	virtual unsigned int getPort()=0;
};

#endif

#include "SocketManager.h"
#include "notifications.h"

class fpolicy
{
public:
	fpolicy(void);
    ~fpolicy(void);
	string ProcessReq(string s);
	void start();
private:
bool handshakDone;
};


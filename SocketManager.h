#ifndef SocketManager_H
#define SocketManager_H
#include "TypeDef.h"


class SocketManager
{
public:
    SocketManager();
    ~SocketManager();

int  writeNotification(string conacatenatedResp );
string  readNotfication ( );

   private:
     bool initializeConn();
    bool acceptConn();
int readHeaderLen( int & retLen );
int readMsg( char *msgBuf, int msgLength,int timeout);
void extractHeaderLen( char * lengthHeader,int & payloadLength);
int writeMsg( char *msgBuf, int msgLength, int timeout);
    SOCKET clientSocket, listening ;


};

#endif // SocketManager_H

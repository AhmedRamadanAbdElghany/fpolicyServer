#include "SocketManager.h"


SocketManager::SocketManager(void)
{
	if (initializeConn() && acceptConn())
        cout << "initialization and Accept OK " << endl;
    else
        cout <<"can't initialize or accept connection "<< endl;
}

SocketManager::~SocketManager(void)
{
	closesocket(clientSocket);
	closesocket(listening);
	WSACleanup();
}

bool SocketManager::initializeConn()
{
    WSADATA wsData;
    WORD ver = MAKEWORD(2, 2);
    int wsOk = WSAStartup(ver, &wsData);
    if (wsOk != 0)
    {
        cerr << "Can't Initialize winsock! Quitting" << endl;
        return 0;
    }
    // Create a socket
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == INVALID_SOCKET)
    {
        cout << "Can't create a socket! Quitting" << endl;
        return 0;
    }
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton ....

    if (bind(listening, (sockaddr*)&hint, sizeof(hint))!= 0 )
    {
        cout << "bind failed with error " <<  WSAGetLastError() << endl;
        return 0;
    }
    cout << "waiting listening " << endl;

    if (listen(listening, SOMAXCONN) == SOCKET_ERROR )
    {
        cout << "Listen failed with error: "<< WSAGetLastError() <<endl;
        return 0 ;
    }
    return 1;
}

bool SocketManager::acceptConn()
{

    struct hostent *cp;
    // Wait for a connection
    sockaddr_in client;
    int clientSize = sizeof(client);
    clientSocket= accept(listening, (sockaddr*)&client, &clientSize);

    cp = gethostbyaddr((char *) &client.sin_addr, sizeof(struct in_addr), AF_INET);
    printf("Connected from %s\n",
           ((cp && cp->h_name) ? cp->h_name : inet_ntoa(client.sin_addr)));

    if (clientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        return 0;
    }
    cout << "connection accepted " << endl;
    return 1;
}

// receive header len
int SocketManager::readHeaderLen( int & retLen )
{
    char msgLengthHeader[6];
    memset(msgLengthHeader,  '\0', 6);

    int bytesReceived = readMsg(msgLengthHeader, 6, READ_TIMEOUT );
//cout << "received bytes of header len is : "<<bytesReceived << endl;
    if (bytesReceived > 0 )
        extractHeaderLen(msgLengthHeader, retLen );
    return bytesReceived;
}


int SocketManager::readMsg( char *msgBuf, int msgLength,int timeout)
{
    ///At this point of time, I assume SSL handshake is done.
    struct timeval   endTime     = {};
    struct timeval   currentTime = {};
    int32_t  numBytesRead    = 0;
    int32_t  retval = 0;

    gettimeofday(&endTime, NULL);
    endTime.tv_sec += timeout;

    do {
        retval = recv(clientSocket , msgBuf+ numBytesRead, msgLength - numBytesRead, 0);

        numBytesRead += retval;

        if (numBytesRead == msgLength) {
        //   cout << "Successfully Read the full message " << endl;
            break;
        }
      gettimeofday(&currentTime, NULL);

    } while(timercmp(&endTime, &currentTime, >));

    if ((numBytesRead < msgLength) && !(timercmp(&endTime, &currentTime, >))) {
     cout <<  "Unsuccessful in reading message. Read msg " << endl;
        return(numBytesRead);
    }
    return(numBytesRead);
}

// return 1 if I can read.
// 0 otherwise .
// you must free allocated memory by your own
string SocketManager::readNotfication ()
{
    int msgLen;
    int bytesReceived= readHeaderLen(msgLen);  // read start 6 bytes "   a" length
    if (bytesReceived <= 0 ) return "" ;
    char  recvbuf [msgLen];
//     cout << "header len "<<msgLen << endl;
    memset(recvbuf, '\0', sizeof(recvbuf));
    bytesReceived = readMsg(recvbuf, msgLen, READ_TIMEOUT );
     string ret (recvbuf);
  //   cout << recvbuf << endl;
     if (bytesReceived == msgLen)
     return ret;
     return "";
}





// for write message over tcp
int SocketManager::writeMsg( char *msgBuf, int msgLength, int timeout)
{
    struct timeval   endTime     = {};
    struct timeval   currentTime = {};
    int  numBytesWrote = 0;
    int  retval = 0;

    gettimeofday(&endTime, NULL);
    endTime.tv_sec += timeout;

    do {
          retval  = send(clientSocket, msgBuf + numBytesWrote, msgLength - numBytesWrote , 0);
          numBytesWrote += retval;
        if (numBytesWrote == msgLength) {
          //  cout <<  "Successfully Wrote the full message " << endl;
            break;
        }
        gettimeofday(&currentTime, NULL);
    } while(timercmp(&endTime, &currentTime, >));
    if ((numBytesWrote < msgLength) && !(timercmp(&endTime, &currentTime, >)))
      cout << "Unsuccessful in writing message.Wrote msg " << endl;

    return(numBytesWrote);
}



int SocketManager::writeNotification(string conacatenatedResp )
{
   // cout << conacatenatedResp << endl;
    char sentt[conacatenatedResp.size()];
    for (int i = 0 ; i < (int)conacatenatedResp.size(); i++ )
        sentt[i] = conacatenatedResp[i];
    sentt[conacatenatedResp.size()] = '\0';
    // cout << "written message " << endl;
    //for (int i = 0 ; i <conacatenatedResp.size() ; i ++ )
      //  printf("%c" , sentt[i]);

    int writeBytes = writeMsg(sentt, conacatenatedResp.size(), WRITE_TIMEOUT );
    return writeBytes ;
}


// handling message header


void SocketManager::extractHeaderLen( char * lengthHeader,int & payloadLength){

int         hFormatLength = 0;
int         msgLength     = 0;
         memcpy(&msgLength, lengthHeader + 1, 4);
 hFormatLength = ntohl(msgLength);
payloadLength = hFormatLength;
//cout << payloadLength << endl;
}


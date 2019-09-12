#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sstream>
#include<unistd.h>
#include"tinyxml2.h"
#include <sys/time.h>
#include <fstream>
#include "SocketManager.h"


using namespace std ;

using namespace tinyxml2;

#define PORT 6789
#define HEADER_LEN 6
#define READ_TIMEOUT 2
#define WRITE_TIMEOUT 2
#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif



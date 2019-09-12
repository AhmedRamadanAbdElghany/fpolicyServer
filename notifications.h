#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H
#include "TypeDef.h"







class notification
{
public:
     notification();
     notification(char * headerD , char * bodyD );
    ~notification();

   string handShakeHeader(int respLen );
   string handShakeResp( );
   string screeningResp();

   // helper

    void formMsgLengthHeader(char *msgLengthHeader, int msgLength);
    bool filterPrintData();

	char * header;
	int headerLen ;
	char * body;
	int bodyLen;
	private:
};


#endif // NOTIFICATIONS_H


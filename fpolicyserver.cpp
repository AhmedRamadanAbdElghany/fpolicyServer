#include"fpolicyserver.h"

fpolicy::fpolicy(void)
{
    handshakDone = 0 ;
}
void fpolicy::start(){
 SocketManager connection ;
    // start read notification.
    bool writeOrNot = 0 ;
    while(1)
    {
        string readed = connection.readNotfication();
        if (readed.size() > 1)  // 0
        {
//                cout << readed << endl << readed.size() << endl;
            if (readed.size() > 130 && writeOrNot == 0 ) // not keep alive : not best implementation but for now we must check the tag itself
            {
                string conacatenatedResp = ProcessReq(readed);// get needed xml nodes and header len
                if (conacatenatedResp == "") continue; // no error in build xml response
                if (connection.writeNotification(conacatenatedResp) == conacatenatedResp.size())
                    handshakDone = 1;
            }
            readed.clear();
        }
        else
        {
            cout << "disconecting " << endl;
            break;
        }
        writeOrNot != writeOrNot;
    }
}

fpolicy::~fpolicy(void)
{
}

string fpolicy::ProcessReq( string s )
{
    char respBuf[s.size()];
    for (int i = 0 ; i < (int)s.size() ; i ++ )
        respBuf[i] = s[i];
    char respXmlBuf [s.size()+1 ];
    memcpy(respXmlBuf,respBuf,s.size());
    respXmlBuf[s.size()] = '\0';
    /*  Get handshake resp header */

    char *token = strtok(respXmlBuf, "\n");
    char *temp = token ;
    token = strtok(NULL, "\n");
    string xmlresp;
    bool canParse = 1 ;
if (temp.size() < 2 || token < 2 ) return "";

    notification notification1 (temp , token);

    if (handshakDone == 0 )
        xmlresp= notification1.handShakeResp();

    else if ( handshakDone == 1 )
    {
        xmlresp= notification1.screeningResp();
        canParse = notification1.filterPrintData();
    }
    if (xmlresp == "" || canParse == 0 ) return "";

    string xmlheader = notification1.handShakeHeader(xmlresp.size());
    string conacatenatedResp =xmlheader+"\n\n"+xmlresp;

// add header length
    char headerLenn[HEADER_LEN];
    memset(headerLenn, '\0', HEADER_LEN);
    notification1.formMsgLengthHeader(headerLenn,conacatenatedResp.size()+1 );
    // add header
    string   msgLengthStr(reinterpret_cast<const char *> (headerLenn), HEADER_LEN);
    conacatenatedResp = msgLengthStr + conacatenatedResp+'\0';
    return  conacatenatedResp;
}


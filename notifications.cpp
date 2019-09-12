#include"notifications.h"



 notification::notification(){
 }
 notification::notification(char * headerD , char * bodyD ){
 header = headerD;
 headerLen = strlen(headerD);
 body = bodyD;
 bodyLen = strlen(bodyD);
 }
 notification::~notification(){
}


string readXmlFile()
{
    ifstream inFile;
    inFile.open("test.xml");
    if (!inFile)
    {
        cerr << "Unable to open file datafile.txt";
        return "";
    }
    string text ="";
    string line ;
    while( std::getline( inFile, line ) ) // for each line read from the file
    {
        text+= line;
    }
    string filtered ="";
    int haveopen = 0 ;
    for (int i = 0 ; i < text.size() ; i ++ )
    {
        if (haveopen == 0 && (text[i] ==' ' || text[i] == '\n'))
            continue;
        else if (text[i] =='<')haveopen = 1 ;
        else if (text[i] =='>')haveopen = 0 ;
        filtered.push_back(text[i]);
    }
    //cout << filtered << endl << filtered.size() << endl;
    return filtered ;
}





string notification::handShakeResp()
{
    XMLDocument xmlDoc;
    string ret ="";
    XMLError eResult = xmlDoc.Parse(body, bodyLen);
    if (eResult != XML_SUCCESS)
    {
        cout << "couldn't parse string  " << endl;
        return "";
    }
    try
    {
        XMLNode * pRoot = xmlDoc.FirstChild();
        if (pRoot == nullptr)
        {
            cout << "couldn't parse root " << endl;
            return "";
        }

        const char * szAttributeText = nullptr;
        XMLElement * notify= pRoot->NextSiblingElement("Handshake");
        notify->SetName("HandshakeResp");
        notify = notify->FirstChildElement("VsUUID");
        szAttributeText = notify->GetText();
        notify = notify->NextSiblingElement("PolicyName");
        notify = notify->NextSiblingElement("SessionId");
        notify = notify->NextSiblingElement("ProtVersion");
        notify->DeleteChildren();
        notify->SetText("1.0");

        xmlDoc.SaveFile("test.xml");
        ret = readXmlFile() ;
    }
    catch(...)
    {
        cout<<"parsing Exception occur." << endl;
        return "";
    }
    return ret ;
}


string notification::screeningResp()
{
    XMLDocument xmlDoc;

    XMLError eResult = xmlDoc.Parse(body, bodyLen);
    if (eResult != XML_SUCCESS)
    {
        cout << "couldn't parse string  " << endl;
        return "";
    }

    XMLNode * pRoot = xmlDoc.FirstChild();
    if (pRoot == nullptr)
    {
        cout << "couldn't parse root " << endl;
        return "";
    }

    XMLElement * notify= pRoot->NextSiblingElement("FscreenReq");
    notify->SetName("FscreenResp");

    notify = notify->FirstChildElement("ReqId");
    notify = notify->NextSiblingElement("ReqType");
    notify = notify->NextSiblingElement("NotfInfo");
    notify->SetName("NotfResp");
    notify->DeleteChildren();
    notify->SetText(1);
    xmlDoc.SaveFile("test.xml");
    string ret = readXmlFile() ;
    return ret ;
}


bool notification::filterPrintData()
{
    XMLDocument xmlDoc;
    string retData="";
    XMLError eResult = xmlDoc.Parse(body, bodyLen);
    if (eResult != XML_SUCCESS)
    {
        cout << "couldn't parse string  " << endl;
        return "";
    }

    XMLNode * pRoot = xmlDoc.FirstChild();

    if (pRoot == nullptr)
    {
        cout << "couldn't parse root " << endl;
        return "";
    }
    try
    {
        XMLElement * notify= pRoot->NextSiblingElement("FscreenReq");
        notify->SetName("FscreenResp");

        notify = notify->FirstChildElement("ReqId");
        notify = notify->NextSiblingElement("ReqType");
        retData += notify->GetText();

        notify = notify->NextSiblingElement("NotfInfo");

        notify = notify->FirstChildElement();
        cout <<"op type : ******  "<< notify->Name() << endl; // type

        notify = notify->FirstChildElement(); // common info

        notify = notify->FirstChildElement(); // ProtCommonInfo

        XMLElement* notify2 = notify->FirstChildElement(); // ip

        cout <<"ip address : ******  "<<notify2->GetText()<<endl;

        notify = notify->NextSiblingElement("DisplayPath");
        cout <<"file path : ******  "<<  notify->GetText() <<endl;
    }
    catch(...)
    {
        cout<<"parsing Exception occur." << endl;
        return false;
    }
    return true;
}


//string
char * headerTypeResp(const char * type  )
{
    if (strcmp(type, "NEGO_REQ") == 0 )
    {
        return "NEGO_RESP";
    }
    if (strcmp(type, "SCREEN_REQ") == 0 )
    {
        return "SCREEN_RESP";
    }
    return (char *)type;
}



// update xml data of first node .
string notification::handShakeHeader( int respLen )
{
    XMLDocument xmlDoc;
    XMLError eResult = xmlDoc.Parse(header, headerLen );
    string ret="";
    if (eResult != XML_SUCCESS)
    {
        cout << "couldn't parse string  " << endl;
        return "";
    }
    try
    {
        XMLNode * pRoot = xmlDoc.FirstChild();
        if (pRoot == nullptr)
        {
            cout << "couldn't parse root " << endl;
            return "";
        }
        XMLElement * notify= pRoot->NextSiblingElement("Header");

        notify = notify->FirstChildElement("NotfType");
        if (notify == nullptr)
        {
            cout << "couldn't parse NotfType " << endl;
            return "";
        }
        const char * type = NULL;
        type = notify->GetText();
        notify->SetText(headerTypeResp(type));

        notify = notify->NextSiblingElement("ContentLen");
        if (notify == nullptr)
        {
            cout << "couldn't parse ContentLen " << endl;
            return "";
        }
        notify->SetText(respLen);

        xmlDoc.SaveFile("test.xml");
        ret = readXmlFile() ;
    }
    catch(...)
    {
        cout<<"parsing Exception occur." << endl;
        return "";
    }
//cout << ret << endl << ret.size() << endl;
    return ret ;
}



// convert to put at the start of header ."   a"
// format length of sent message
void notification::formMsgLengthHeader(char *msgLengthHeader, int msgLength)
{
    int    lengthNwFormat = 0;
    ///Add the record marker at the beginning and end of the buffer.
    msgLengthHeader[0] = '"';
    msgLengthHeader[HEADER_LEN-1] = '"';

    lengthNwFormat = htonl(msgLength);
//    cout << lengthNwFormat << endl;
    memcpy(msgLengthHeader + 1, &lengthNwFormat, 4);
}




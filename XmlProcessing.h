#pragma once
#include "SocketManager.h"
#include "Task.h"

class Process
{
public:
	Process(void);
	virtual ~Process(void);
	void initElection();
	void sendMessage(const string & message, const string & data = string());

	static unsigned int processCount;
	static Task task;
private:
	void enterSlaveryMode();
	void enterCommandMode();
	bool enterElectionMode();
	string getMyIp();

	SocketManager m_socketManager;
	unsigned int m_processId;
};


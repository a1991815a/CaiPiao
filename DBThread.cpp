#include "DBThread.h"

DBThread* DBThread::_instance = nullptr;

DBThread* DBThread::getInstance()
{
	if (_instance==nullptr)
	{
		_instance = new DBThread();
	}
	return _instance;
}


DBThread::DBThread()
	:loop(true)
{

}

void DBThread::run()
{

}

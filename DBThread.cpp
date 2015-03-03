#include "DBThread.h"
#include "MysqlUtils.h"
#include "CustomDefine.h"

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
	:loop(true), can_start(false)
{}

void DBThread::pushCall(const char* format, ...)
{
	va_list _list;
	va_start(_list, format);
	QString _sql = "";
	_sql.vsprintf(format, _list);
	_sql.push_front("CALL ");
	return pushSql(_sql);
}

void DBThread::pushFunc(const char* format, ...)
{
	va_list _list;
	va_start(_list, format);
	QString _sql = "";
	_sql.vsprintf(format, _list);
	_sql.push_front("SELECT ");
	return pushSql(_sql);
}

void DBThread::pushSql(const char* format, ...)
{
	va_list _list;
	va_start(_list, format);
	QString _sql = "";
	_sql.vsprintf(format, _list);
	return pushSql(_sql);
}

void DBThread::pushSql(QString _sql)
{
	_sql_list.push_back(_sql);
}

void DBThread::excuteNext()
{
	if (!_sql_list.isEmpty())
	{
		for (auto& obj : _sql_list){
			_sqlUT->excute(obj);
			qDebug() << obj;
			_sql_list.removeOne(obj);
		}
	}
}

void DBThread::startSendSql()
{
	can_start = true;
}

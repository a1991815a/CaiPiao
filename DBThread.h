#ifndef __DBTHREAD__
#define __DBTHREAD__
#include <QThread>

#define _dbThread DBThread::getInstance()

class DBThread{
public:
	void pushSql(QString);
	void pushSql(const char* format, ...);
	void pushCall(const char* format, ...);
	void pushFunc(const char* format, ...);
	void excuteNext();
	void clearAll();

	void turnLoop(bool loop){ this->loop = loop; };

	void startSendSql();

	inline bool isEmpty(){ return _sql_list.isEmpty(); };
private:
	bool loop;
	DBThread();

	QList<QString> _sql_list;
public:
	static DBThread* getInstance();
private:
	static DBThread* _instance;

	volatile bool can_start;

};
#endif
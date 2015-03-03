#ifndef __DBTHREAD__
#define __DBTHREAD__
#include <QThread>

class DBThread:public QThread{
public:
	virtual void run() override;

	void pushSql(QString);
	void excuteNext();
	void clearAll();

private:
	bool loop;
	DBThread();

	QList<QString> _sql;
public:
	static DBThread* getInstance();
private:
	static DBThread* _instance;
};
#endif